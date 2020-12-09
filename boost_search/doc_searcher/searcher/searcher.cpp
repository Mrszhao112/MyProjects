#include "searcher.h"
#include <algorithm>
#include "../common/util.hpp"
#include <jsoncpp/json/json.h>

namespace searcher{
  const char* const DICT_PATH = "../jieba_dict/jieba.dict.utf8";
  const char* const HMM_PATH = "../jieba_dict/hmm_model.utf8";
  const char* const USER_DICT_PATH = "../jieba_dict/user.dict.utf8";
  const char* const IDF_PATH = "../jieba_dict/idf.utf8";
  const char* const STOP_WORD_PATH = "../jieba_dict/stop_words.utf8";

 void  Index::PrintForward(){
   for(auto& e:forward_index){
     std::cout << e.doc_id <<e.content<< std::endl;
   }
  }
 void Index::PrintInvert(){
   for(auto& e : inverted_index){
       std::cout << e.first << " ";
     for(auto& v : e.second){
       
       std::cout << v.doc_id << " ";
     }
     std::cout <<" " << std::endl;
   }
 }
  Index::Index() : jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH){}
  
  const DocInfo* Index::GetDocInfo(int64_t doc_id){
    if(doc_id >= forward_index.size()){
      return nullptr;
    }
    return &forward_index[doc_id];
  }

   const InvertedList* Index::GetInvertedList(const string& key){
    auto it = inverted_index.find(key);
    if(it == inverted_index.end()){
      return nullptr;
    }
    return &it->second;
  }

  bool Index::Build(const string& input_path){
    //1.按行读取文件内容 row_input \3分割
    std::cout << "start Build" << std::endl;
    std::ifstream file(input_path.c_str());
    if(file.is_open() == false){
      std::cout << "open file error" << std::endl;
      return false;
    }
    string line;
    while(std::getline(file, line)){
    //2.将当前行解析成DocInfo对象
    DocInfo* doc_info = BuildForward(line);
    if(doc_info == nullptr){
      std::cout << "BuildForward error" << std::endl;
      continue;
    }
    //3.根据DocInfo进行解析 构造成倒排索引
    BuildInverted(*doc_info);


    //打印日志 模100防止打印过多
    if(doc_info->doc_id % 100 == 0){
      std::cout << doc_info->doc_id << std::endl;
    }
   }

    std::cout << "Buid end" << std::endl;
    file.close();
    return true;
  }

  //按行切分即可 \3 为标志
  DocInfo* Index::BuildForward(const string& line){
    vector<string> tokens;
    common::util::Split(line, "\3", &tokens);
    if(tokens.size() != 3){
      return nullptr;
    }
    //将切分结果填充到doc_info
    DocInfo doc_info;
    //填充forward_index 将doc_id 和 数组下标 一一对应
    doc_info.doc_id = forward_index.size();
    doc_info.title = tokens[0];
    doc_info.url = tokens[1];
    doc_info.content = tokens[2];
    forward_index.push_back(std::move(doc_info));
    //返回结果
    return &forward_index.back();
  }

  //构建倒排 key->val(vecotr<Weight>) val为一个倒排拉链
  void Index::BuildInverted(const DocInfo& doc_info){
    // 创建统计词频的结构
    struct WordCnt{
      int title_cnt;
      int content_cnt;
      WordCnt():title_cnt(0), content_cnt(0){}
    };
    unordered_map<string, WordCnt> word_cnt_map;

    //1. 对标题进行分词  分词不区分大小写 根据百度得到
    vector<string> title_token;
    CutWord(doc_info.title, &title_token);
    //2. 遍历分词结果 统计出现次数
    for(string word : title_token){
      boost::to_lower(word);
      ++word_cnt_map[word].title_cnt;
    }
    //3. 对正文进行分词
    vector<string> content_token;
    CutWord(doc_info.content, &content_token);
    //4. 根据分词结果 统计出现次数
    for(string word : title_token){
      boost::to_lower(word);
      ++word_cnt_map[word].content_cnt;
    }
    //5. 根据统计结果整合weight对象 将结果加入倒排索引中
    for(const auto& word_pair : word_cnt_map){
      //构造weight对象
      Weight weight;
      weight.doc_id = doc_info.doc_id;
      //计算权重
      weight.weight = 10*word_pair.second.title_cnt + word_pair.second.content_cnt;
      weight.word = word_pair.first;

      //将weight加入倒排拉链
      InvertedList &inverted_list = inverted_index[word_pair.first];
      inverted_list.push_back(weight);
    }
  }

  void Index::CutWord(const string& input, vector<string>* output){
    jieba.CutForSearch(input, *output);
  }

////////////////////////////////////////////////////////////////////////////////////////////////
//searcher 模块
  bool Searcher:: Init(const string& input_path){
    return index->Build(input_path);
  }
  bool Searcher::Search(const string& query, string* output){
    //分词 对查询词进行分词
    vector<string> tokens;
    index->CutWord(query, &tokens);
    //触发 根据分词结果在构造好的倒排中查询
    vector<Weight> all_token_result;
    for(string word : tokens){
      boost::to_lower(word);
      //将每一个分词的搜索结果都保存下来
      auto* inverted_list = index->GetInvertedList(word);
      if(inverted_list == nullptr){
        continue;
      }
      all_token_result.insert(all_token_result.end(),inverted_list->begin(), inverted_list->end());
    }
    //排序 合并查询结果的倒排拉链到一起并根据权重进行排序
    std::sort(all_token_result.begin(), all_token_result.end(),[](const Weight& w1, const Weight& w2){return w1.weight > w2.weight;});
    //包装结果 获取文档id并查正排 将doc_info中的内容构造成预期的结果
    //将doc_info 中的内容构建成最终预期的格式 JSON
    //使用jsoncpp这个库来实现json的操作
    Json::Value results;
    for(const auto& weight : all_token_result){
      //根据weight中的doc_id查正排
      const DocInfo* doc_info = index->GetDocInfo(weight.doc_id);
      //将这个doc_info对象进一步包装成Json对象
      Json::Value result;
      result["title"] = doc_info->title;
      result["url"] = doc_info->url;
      result["desc"] = GenerateDesc(doc_info->content, weight.word);
      results.append(result);
    }
    //将results序列化成字符串 写入output
    Json::FastWriter writer;
    *output = writer.write(results);
    return true;
  }

  string Searcher::GenerateDesc(const string& content, const string& word){
    //根据正文找到word出现的位置 以该位置为中心 往前找60个字节 作为描述的起始位置 
    //从起始位置开始往后找160个字节 最后加上...
    //前后位置不够则 到边界即可
    //1. 找到word在正文中出现的位置
    size_t first_pos = content.find(word);
    size_t desc_begin = 0;
    if(first_pos == string::npos){
      if(content.size() < 160 ){
        return content;
      } 
      string desc = content.substr(0, 160);
      desc[desc.size() - 1] = '.';
      desc[desc.size() - 2] = '.';
      desc[desc.size() - 3] = '.';
      return desc;
    }

    //2. 以该位置为基准 向前找一些字节 
    desc_begin = first_pos < 60 ? 0 : first_pos - 60;
    if(desc_begin + 160 >= content.size()){
      return content.substr(desc_begin);
    }
      string desc = content.substr(desc_begin, 160);
      desc[desc.size() - 1] = '.';
      desc[desc.size() - 2] = '.';
      desc[desc.size() - 3] = '.';
      return desc;
  }











}//end searcher




























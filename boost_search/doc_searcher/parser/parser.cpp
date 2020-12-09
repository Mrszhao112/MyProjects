//实现预处理模块
//读取并分析boost的html
//解析每个文档的标题 url 正文(去除html标签)
//最终将结果输出成行文本文件
#include <iostream>
#include <vector>
#include <string>
//#include <unordered_map>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "../common/util.hpp"
using std::string;
//using std::unordered_map;
using std::vector;
//定义一些相关的结构和变量 g全局

//读取路径
string g_input_path = "../data/input";
//输出路径
string g_output_path = "../data/tmp/raw_input";

//描述文档的结构体 对应一个文档的 标题 url 正文
struct DocInfo{
  string title;
  string url;
  string content;
};
//注意编码规范
bool EnumFile(const string& input_path, vector<string>* file_list){
  //枚举路径使用boost库完成
  namespace fs = boost::filesystem;
  fs::path root_path(input_path);
  if(!fs::exists(root_path)){
    std::cout << "isn't exists" <<std::endl;
    return false;
  }
  //递归遍历的时候使用核心类
  fs::recursive_directory_iterator end_iter;//默认生成的哨兵
  for(fs::recursive_directory_iterator iter(root_path); iter != end_iter; iter++){
    //当前路径对应的是不是普通文件 是目录则跳过
    if(!fs::is_regular_file(*iter)){
      continue;
    }
    //判断是不是html文件 不是则跳过
    if(iter->path().extension() != ".html"){
      continue;
    }
    file_list->push_back(iter->path().string());
  }
  return true;
}
//根据文件内容解析出标题
bool ParseTitle(const string& html, string *title){
  size_t beg = html.find("<title>");
  if(beg == string::npos){
    std::cout << "not find"<<std::endl;
  }
  size_t end = html.find("</title>");
  if(end == string::npos){
    std::cout << "not find"<< std::endl;
  }
  beg += string("<title>").size();
  if(beg >= end){
    std::cout << "find errro" << std::endl;
  }
  *title = html.substr(beg, end - beg);
  return true;
}
//根据本地路径获取在线文档的路径 两个路径的后半部分是相同的
bool ParseUrl(const string& file_path, string* url){
  string url_head = "http://www.boost.org/doc/libs/1_74_0/doc/";
  string url_tail = file_path.substr(g_input_path.size());
  *url = url_head + url_tail;
  return true;
}
//对html去除标签 将正文保存起来
bool ParseContent(const string& html, string* content){
  bool is_content = true;//判断是否是标签
  for(auto c : html){
    if(is_content == true){//当前是正文
      if(c == '<'){
        is_content = false;
      }else{
        if(c == '\n'){
          c = ' ';
        }
        content->push_back(c);
      }
    }else{
      if(c == '>'){
        is_content = true;
      }
    }
  }
  return true;
}
bool Parsefile(const string& file_path, DocInfo* doc_info){
  //1. 读取文件内容到内存中
  string html;
  bool ret = common::util::Read(file_path, &html);//read通用函数放置在common中
  if(ret == false){
    std::cout << "Read error" << std::endl;
    return false;
  }
  //2. 根据文件内容解析出标题 html中的title
  ret = ParseTitle(html, &doc_info->title);
  if(ret == false){
    std::cout << "analize title error"<< std::endl;
    return false;
  }
  //3. 根据文件路径 构造对应的url
  ret = ParseUrl(file_path, &doc_info->url);
  if(ret == false){
    std::cout << "analize URL error" <<std::endl;
    return false;
  }
  //4. 根据文件内容进行去标签 填充DocInfo
  ret = ParseContent(html, &doc_info->content);
  if(ret == false){
    std::cout << "analize content error"<< std::endl;
    return false;
  }
  return true;
}
//将解析到的内容装docinfo 参数二无拷贝构造因此&
//需要解决粘包问题 面向自己流
bool  writeOutput(const DocInfo& doc_info, std::ofstream& ofstream){
  ofstream << doc_info.title << "\3" << doc_info.url << "\3" <<doc_info.content << std::endl;
  return true;
}
//预处理核心内容
//1. input中的html路径全部枚举出来
//2. 根据枚举的路径 依次读取每个文件内容 进行解析
//3. 把解析结果写入到最终的输出文件中

int main(){
  //1.枚举路径
  vector<string> file_list;
  bool ret  = EnumFile(g_input_path, &file_list);
  if(ret == false){
    std::cout << "EnumFile error" <<std::endl;
    return 1;
  }
  // 2.遍历枚举路径 针对每个文件 进行单独处理
  std::ofstream output_file(g_output_path.c_str());
  if(output_file.is_open() == false){
  std::cout <<  "open output_file error" << std::endl;
  }
  for (const auto& e : file_list){
    std::cout << e << std::endl; 
    DocInfo doc_info;
    //通过函数进行解析工作
    ret = Parsefile(e, &doc_info);
    if(!ret){
      std::cout << "analize error"<<std::endl;
      continue;
    }
  //3. 把解析出来的结果写入到最终的输出文件中
  ret = writeOutput(doc_info, output_file);
  if(ret == false){
    std::cout << "write file error" << std::endl;
  }
  }
  output_file.close();
  return 0;
}

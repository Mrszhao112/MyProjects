#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <unordered_map>
#include <fstream>
#include "cppjieba/Jieba.hpp"
using std::fstream;
using std::string;
using std::vector;
using std::unordered_map;

namespace searcher{
//索引模块代码
///////////////////////////////////////////////////////////////////////////

  //索引的描述
  //正排索引描述
  struct DocInfo{
    int64_t doc_id;
    string title;
    string url;
    string content;
  };
  //倒排索引描述
  struct Weight{
    int64_t doc_id;
    int weight;//权重
    string word;
  };

  //倒排拉链
  typedef vector<Weight> InvertedList;

  //描述整个索引结构 并提供API
  class Index{
    public:
      Index();
      //查正排
      const DocInfo* GetDocInfo(int64_t doc_id);
      //查倒排
      const InvertedList* GetInvertedList(const string& key);
      //构建索引
      bool Build(const string& input_path); 
      //构建正排序
      DocInfo* BuildForward(const string& line);
      //构建倒排
      void BuildInverted(const DocInfo& doc_info);
      //进行分词
      void CutWord(const string& input, vector<string>* output);
      void PrintForward();
      void PrintInvert();
    private:
      //正排索引
      vector<DocInfo> forward_index;
      //倒排索引
      unordered_map<string, InvertedList> inverted_index;
      //jieba对象实现分词
      cppjieba::Jieba jieba;
  };
//搜索模块代码
///////////////////////////////////////////////////////////////////////////
  class Searcher{
    public:
      Searcher():index(new Index()){}//此处由进程结束即可 无需自己手动释放
     bool Search(const string& query, string* results);
     bool Init(const string& input_path);
     string GenerateDesc(const string& content, const string& word);
    private:
     Index* index;
  };
}//end searcher

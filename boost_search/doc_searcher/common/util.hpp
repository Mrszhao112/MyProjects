#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <boost/algorithm/string.hpp>

using std::string;
using std::vector;
namespace common{
class util{
  public:
    //将input_path路径的文件读取到output中
    static bool Read(const string& input_path, string* output){
      std::ifstream file(input_path.c_str());
      if(file.is_open() == false){
        return false;
      }
      //按行读取文件 将每行的结果追加到output中
      string line;
      while(std::getline(file, line)){
        *output += (line + "\n");
      } 
      file.close();
      return true;
    }

    //基于boost中的字符串切分 封装一个字符串切分函数
    //输入输出 分隔符 关闭压缩 "" 视为空字符串
    static void Split(const string& input, const string& delimter, vector<string>* output){
      boost::split(*output, input, boost::is_any_of(delimter), boost::token_compress_off);
    }
};
}//namespace

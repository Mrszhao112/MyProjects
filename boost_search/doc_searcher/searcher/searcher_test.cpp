#include "searcher.h"
//用户通过控制台输入一个query
int main(){
  searcher::Searcher searcher;
  bool ret = searcher.Init("../data/tmp/raw_input");
  if(ret == false){
    std::cout << "init errro" << std::endl;
  }
  while(true){
    //刷新缓冲区
    std::cout << "searcher>" << std::flush;
    string query;
    std::cin >> query;
    if(std::cin.good() == false){
      //读到EOF
      std::cout << "goodbye" << std::endl;
      break;
    }
    string results;
    searcher.Search(query, &results);
    std::cout << results << std::endl;
  }
  
  return 0;
}

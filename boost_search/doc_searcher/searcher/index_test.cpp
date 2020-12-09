#include "searcher.h"
int main(){
  //创建index
  searcher::Index index;
  bool ret = index.Build("../data/tmp/raw_input");
  if(ret == false){
    std::cout << "build error" << std::endl;
    return 1;//不能return-1的原因 status没有负数这一回事
  }
  //index.PrintForward();
  //index.PrintInvert();
  //构建成功 调用索引的相关函数 查正排 查倒排
  auto inverted_list = index.GetInvertedList("Filesystem");
  if(inverted_list == nullptr){
    std::cout << "find nullptr error" <<std::endl;
    return 1;
  }

  for(const auto& weight : *inverted_list){
    std::cout << "doc_id : " << weight.doc_id << "weight : " << weight.weight << std::endl;
    auto* doc_info = index.GetDocInfo(weight.doc_id);
    std::cout << "title : " << doc_info->title << std::endl;
    std::cout << "url : " << doc_info->url << std::endl;
    std::cout << "content : " << doc_info->content << std::endl;
    std::cout << "==============================================================================================================" << std::endl;
  }
  return 0; 
}

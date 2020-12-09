#include "httplib.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "../searcher/searcher.h"
using std::vector;
using std::string;
using std::unordered_map;

int main(){
  using namespace httplib;
  //创建searcher对象
  searcher::Searcher searcher;
  bool ret = searcher.Init("../data/tmp/raw_input");
  if(ret == false){
    std::cout << "init error" <<std::endl;
    return 1;
  }
//创建server对象
  Server server;
  server.Get("/searcher", [&searcher](const Request& req, Response& resp){
      if(!req.has_param("query")){
      resp.set_content("query error", "text/plain; charset = utf_8");
      return;
      }
      string query = req.get_param_value("query");
      string results;
      searcher.Search(query, &results);
      resp.set_content(results, "application/json; charset = utf_8");
      });
  //告诉服务器静态资源存储的路径 html css js 图片
  server.set_base_dir("./wwwroot");
  //启动服务器
  server.listen("0.0.0.0", 9000);
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include "coroutine.h"
//搭建协程服务器进行测试

//tcp 套接字创建并初始化
int tcp_init(){
  int lfd = socket(AF_INET, SOCK_STREAM, 0);
  if(lfd == -1){
    perror("socket error");
    exit(1);
  }
  
  int op = 1;
  setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));//问题
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(9000);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  int r = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
  if(r < 0){
    perror("bind error");
    exit(1);
  }
  listen(lfd, SOMAXCONN);
  return  lfd;
}
//将套接字设置成非阻塞
void set_nonblock(int fd){
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
//接受连接 非阻塞进行接收 通过返回值判断 创建新连接 or 切换协程运行
void accept_conn(int lfd, schedule_t* s, int co_ids[], void*(*call_back)(schedule_t*, void* agrs)){
  while(1){
    int cfd = accept(lfd, NULL, NULL);
    if(cfd > 0){
      set_nonblock(cfd);
      int args[] = {lfd, cfd};
      int id = coroutine_create(s, call_back, args);
      int i;
      for(i = 0; i < CORSE; i++){
        if(co_ids[i] == -1){
          co_ids[i] = id;
          break;
        }
      }
      if(i == CORSE){
        printf("more connections\n");
      }
      coroutine_running(s, id);
    }else {
      for(int i = 0; i < CORSE; i++){
        int cid = co_ids[i];
        if(cid == -1){
          continue;
        }
        coroutine_resume(s, cid);
      }
    }
  }
}
//回调函数
void* handle(schedule_t* s, void* args){
  int *arr = (int*)args;
  int cfd = arr[1];

  char buf[1024] = {};
  while(1){
    memset(buf, 0x00, sizeof(buf));
    int r = read(cfd, buf, 1024);
    if(r == -1){
      coroutine_yield(s);
    }else if(r == 0){
      break;
    }else{
      printf("recv: %s\n", buf);
      if(strncasecmp(buf, "exit", 4) == 0){
        break;
      }
      write(cfd, buf, r);
    }
  }
  return NULL;
}
int main(){
  int lfd = tcp_init();
  set_nonblock(lfd);
  schedule_t* s = schedule_create();
  int co_ids[CORSE];
  for(int i = 0; i < CORSE; i++){
    co_ids[i] = -1;
  }
  accept_conn(lfd, s, co_ids, handle);
  schedule_destory(s);
  return 0;
}


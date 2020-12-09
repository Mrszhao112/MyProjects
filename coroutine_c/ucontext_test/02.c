#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h>
void fun(){
  printf("func()\n");
  sleep(1);
  
}
int main( void  ) {
  //利用ucontext实现 循环执行一个函数``
  int i = 0;
  ucontext_t fun_ctx, main_ctx;
  getcontext(&fun_ctx);//保存当前数据
  getcontext(&main_ctx);
  printf("i = %d", i++); 
  char* stack = (char*)malloc(1024*8*sizeof(char*));//为fun_ctx设置新环境
  fun_ctx.uc_link           = &main_ctx;
  fun_ctx.uc_stack.ss_flags = 0;
  fun_ctx.uc_stack.ss_size  = 1024*8;
  fun_ctx.uc_stack.ss_sp    = stack;
  makecontext(&fun_ctx,fun , 1);
  setcontext(&fun_ctx);
  return 0;
}



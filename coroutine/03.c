#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h>

  ucontext_t f1_ctx,f2_ctx, main_ctx;
void func1(void){
  printf("func1 begin!\n");
  printf("step 1\n");
  swapcontext(&f1_ctx, &f2_ctx);
  printf("step 2\n");
  printf("func1 ending\n");
}
void func2(void){
  printf("func2 begin!\n");
  printf("step 3\n");
  swapcontext(&f2_ctx, &f1_ctx);
  printf("step 4\n");
  printf("func2 ending\n");
}

int main(void){
  //目的 实现在两个函数内部进行切换 
  //具体 一个函数执行一半切换到另一个函数内部进行执行 另一个函数执行一般切换到原先函数执行 最后执行另一个函数
  char stack1[1024*8] = {0};
  char stack2[1024*8] = {0};
  getcontext(&f1_ctx);
  getcontext(&f2_ctx);
  f1_ctx.uc_stack.ss_flags = 0;
  f1_ctx.uc_stack.ss_size  = 1024*8;
  f1_ctx.uc_stack.ss_sp    = stack1;
  f1_ctx.uc_link = &f2_ctx;
  f2_ctx.uc_stack.ss_flags = 0;
  f2_ctx.uc_stack.ss_size  = 1024*8;
  f2_ctx.uc_stack.ss_sp    = stack2;
  f2_ctx.uc_link = &main_ctx;
  
  makecontext(&f1_ctx, func1, 0);
  makecontext(&f2_ctx, func2, 0);
  printf("step 5\n");
  swapcontext(&main_ctx, &f1_ctx);
  printf("step 6\n");
  printf("main exit\n");
  return 0;
}

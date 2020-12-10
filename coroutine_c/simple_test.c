#include "coroutine.h"
#include <stdio.h>
#include <stdlib.h>

void* func1(schedule_t* s, void* args){
  printf("func1\n");
  coroutine_yield(s);
  int a = *(int*)args;
  printf("func1 exit %d\n", a);
  return NULL;
}
void* func2(schedule_t* s, void* args){
  printf("func2\n");
  coroutine_yield(s);
  int b = *(int*)args;
  printf("func2 exit %d\n", b);
  return NULL;
}

int main(){
  schedule_t* s =  schedule_create();
  int* a = (int*)malloc(sizeof(int));
  *a = 1;
  int* b = (int*)malloc(sizeof(int));
  *b = 2;
  int id1 = coroutine_create(s, func1, a);
  int id2 = coroutine_create(s, func2, b);
  coroutine_running(s, id1);
  coroutine_running(s, id2);
  while(schedule_finished(s) == 0){
    coroutine_resume(s, id2);
    coroutine_resume(s, id1);
  }
  return 0;
}

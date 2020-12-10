#include "coroutine.hpp"
#include <iostream>

void* func1(void* args){
  int* a = (int*)args;
  printf("%d\n", *a);
  printf("func1\n");
  
return nullptr;
}
void* func2(void* args){
  int* a = (int*)args;
  printf("%d\n", *a);
  printf("func2\n");
return nullptr;
}

int main(){
  Corout c;
/*  int *a = new int;
  *a = 1;
  int id1 = c.coroutine_create(func1, a);
  int id2 = c.coroutine_create(func2, a);
  c.coroutine_running(id1);
  c.coroutine_running(id2);
  delete a;*/
  return 0;
}

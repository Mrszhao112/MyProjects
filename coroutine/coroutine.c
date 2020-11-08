#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coroutine.h"

//创建调度器 分配协程挂载空间 初始化调度器
//调度器缺点 协程数量固定->改进方式 红黑树
schedule_t *schedule_create(){
  schedule_t* s = (schedule_t*)malloc(sizeof(schedule_t));
  if(s != NULL){
    s->coroutines = (coroutine_t**)malloc(sizeof(coroutine_t*)*CORSE);//问题*
    memset(s->coroutines, 0X00, sizeof(coroutine_t*)*CORSE);
    s->max_id = 0;
    s->current_id = -1;
  }
  return s;
}
static void* main_fun(schedule_t* s){
  int id = s->current_id;
  if(id != -1){
    coroutine_t *c = s->coroutines[id];
    c->call_back(s, c->argc);//argc->args
    c->state = DEAD;
    s->current_id = -1;
  }
  return NULL;
}
//创建协程 返回协程在调度器中的下标
int coroutine_create(schedule_t* s, void*(*call_back)(schedule_t*, void* args), void* args){
  //判断调度器有无空协程 有使用没有创建
  int i;
  coroutine_t *c = NULL;
  for(i = 0; i < s->max_id; i++){
    c = s->coroutines[i];
    if(c->state == DEAD){
      break;
    }
  }
  if(i == s->max_id || c == NULL){
    s->coroutines[i] = (coroutine_t*)malloc(sizeof(coroutine_t));
    s->max_id++;
  }
  c = s->coroutines[i];
  c->call_back = call_back;
  c->argc = args;
  c->state = READY;

  getcontext(&c->ctx);
  
  c->ctx.uc_stack.ss_sp = c->stack;
  c->ctx.uc_stack.ss_size = STACKSZ;
  c->ctx.uc_flags = 0;
  c->ctx.uc_link = &s->ctx_main;
  makecontext(&c->ctx, (void(*)())&main_fun, 1, s);
  return i;
  
}
//启动协程
void coroutine_running(schedule_t* s, int id){
  int st = get_state(s, id);
  if(st == DEAD){
   return;
  }
  coroutine_t* c = s->coroutines[id];
  c->state = RUNNING;
  s->current_id = id;
  swapcontext(&s->ctx_main, &c->ctx);
}
//让出CPU
void coroutine_yield(schedule_t* s){
  if(s->current_id != -1){
    coroutine_t* c = s->coroutines[s->current_id];
    c->state = SUSPEND;
    s->current_id = -1;
    swapcontext(&c->ctx, &s->ctx_main);
  }
}
//恢复CPU
void coroutine_resume(schedule_t* s, int id){
  coroutine_t* c = s->coroutines[id];
  if(c != NULL && c->state == SUSPEND){
    s->current_id = id;
    c->state = RUNNING;
    swapcontext(&s->ctx_main, &c->ctx);
  }
}
//判断当前调度器中的协程是否已经全部执行结束
int schedule_finished(schedule_t* s){
  if(s->current_id != -1 ){
    return 0;
  }
  for(int i = 0; i < s->max_id; i++ ){
    if(s->coroutines[i]->state != DEAD){
      return 0;
    }
  }
  return 1;
}
//删除调度器释放资源
void schedule_destory(schedule_t* s){
  for(int i = 0; i < s->max_id; i++ ){
   delete_coroutine(s, i);
  }
  free(s->coroutines);
  free(s);
}
//删除协程
static void delete_coroutine(schedule_t* s, int id){
  coroutine_t* c = s->coroutines[id];
  if(c != NULL){
    free(c);
    s->coroutines[id] = NULL;
  }
}
//获取协程状态
static enum State get_state(schedule_t* s, int id){
  coroutine_t* c = s->coroutines[id];
  if(c == NULL){
    return DEAD;
  }
  return c->state;
}

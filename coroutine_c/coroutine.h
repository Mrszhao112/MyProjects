#ifndef __COROUTINE_H__
#define __COROUTINE_H__
#include <ucontext.h>

#define CORSE   (1024)//最多分配的协程数目
#define STACKSZ (1024*8)

struct schedule;

//协程状态
enum State {DEAD, RUNNING, READY, SUSPEND};

//协程结构体
typedef struct {
  ucontext_t ctx; //协程上下文数据
  char* stack[STACKSZ];//独有栈
  enum State state;//协程状态
  void* argc;//函数参数
  void* (*call_back)(struct schedule* s, void* args);//回调函数
}coroutine_t;

//协程调度器结构体
typedef struct schedule {
  ucontext_t ctx_main;//中控流程上下文
  int max_id;//最大下标
  int current_id;//当前工作协程ID
  coroutine_t **coroutines;//所有协程
}schedule_t;

//创建协程调度器
schedule_t* schedule_create();
//协程执行函数
static void* main_fun(schedule_t* s);
//创建协程,并返回当前协程在调度器的下标
int coroutine_create(schedule_t* s, void*(*call_back)(schedule_t* s, void* args), void *args);
//启动协程
void coroutine_running(schedule_t* s, int id);
//让出CPU
void coroutine_yield(schedule_t* s);
//恢复CPU
void coroutine_resume(schedule_t* s, int id);
//删除协程
static void delete_coroutine(schedule_t* s, int id);
//释放调度器(释放所有协程)
void schedule_destory(schedule_t* s);
//判断是否所有协程已经工作完毕
int schedule_finished(schedule_t*s);
//获取协程状态
static enum State get_state(schedule_t* s, int id);
#endif

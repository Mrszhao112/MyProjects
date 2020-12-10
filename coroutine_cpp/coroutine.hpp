#pragma once
#include <map>
#include <ucontext.h>
using std::map;
//c++版本协程 基本思路与c语言版本相同 
//底层采用红黑树对每个协程进行组织

struct schedule_t;
//协程状态
enum State{DEAD, RUNNING, READY, SUSPEND};
//协程结构体
struct coroutine_t{
  ucontext_t ctx;//协程上下文
  char* stack[8192];//独有栈
  enum State state;//协程状态
  void* argc;//函数参数
  void* (*call_back)(void*);//回调哈数/////////////
  coroutine_t():state(DEAD){}
};
//协程调度器结构体
//使用红黑树进行组织 id -》一个协程
struct schedule_t{
  ucontext_t ctx_main;//主控流程上下文
  int current_id;//当前工作协程id
  map<int, coroutine_t*> coroutines;
};
class Corout;
//协程执行函数
    void* main_fun(Corout* C);
class Corout{
  friend void* main_fun(Corout* C);
  
public:
    
  Corout(){
      schedule_t* s = new schedule_t;
        s->current_id = -1;
    }

    ~Corout(){
      delete s;
    }


//创建协程
    int coroutine_create(void*(*call_back)(void*), void* args){
      //创建协程 初始化其上下文
      coroutine_t* c = new coroutine_t;
      c->call_back = call_back;
      c->argc = args;
      c->state = READY;

      getcontext(&c->ctx);

      c->ctx.uc_stack.ss_sp = c->stack;
      c->ctx.uc_stack.ss_size = 8192;
      c->ctx.uc_flags = 0;
      c->ctx.uc_link = &s->ctx_main;
      makecontext(&c->ctx,(void(*)())&main_fun, 1, s);

      s->coroutines[s->coroutines.size()] = c;
      return s->coroutines.size() - 1;
    }


//启动协程
    void coroutine_running(int id){ 
      int st = get_state(id);
      if(st == DEAD){
        return;
      }
      coroutine_t* c = s->coroutines[id];
      c->state = RUNNING;
      s->current_id = id;
      swapcontext(&s->ctx_main, &c->ctx);
    }


//让出cpu
    void coroutine_yield(){
      if(s->current_id != -1){
        coroutine_t* c = s->coroutines[s->current_id];
        c->state = SUSPEND;
        s->current_id = -1;
        swapcontext(&c->ctx, &s->ctx_main);
      }
    }


//恢复cpu
    void coroutine_resume(int id){ 
      int st = get_state(id);
      if(st == DEAD || st != SUSPEND){
        return;
      }
      coroutine_t* c = s->coroutines[id];
      s->current_id = id;
      c->state = RUNNING;
      swapcontext(&s->ctx_main, &c->ctx);
    }


//判断所有协程是否已经工作完成
    bool schedule_finish(){
      if(s->coroutines.size()>0){
        return false;
      }
      return true;
    }


//获取协程状态
    enum State get_state(int id){
      if(s->coroutines.find(id) == s->coroutines.end()){
        return DEAD;
      }
    return s->coroutines[id]->state;
    }


  private:
    schedule_t* s;
};
//协程执行函数
    void* main_fun(Corout* C){
     int id = C->s->current_id;
     if(id != -1){
      coroutine_t* c = C->s->coroutines[id];
      c->call_back(c->argc);
      c->state = DEAD;
      C->s->current_id = -1;
     }
     return nullptr;
    }

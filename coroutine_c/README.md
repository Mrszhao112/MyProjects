schedule_creat创建协程调度器，初始化数据

```cpp
// 协程调度器
typedef struct schedule {
    coroutine_t** coroutines; // 所有协程
    int current_id;           // 当前运行的协程id
    int max_id;               // 最大下标
    ucontext_t ctx_main;      // 主流程上下文
} schedule_t;
```

coroutine_creat创建协程，每一个协程有自己的回调函数、上下文数据、协程栈、运行状态等信息

创建协程后默认状态为READY，makecontext制造上下文保存到协程结构体的ctx中，当启动协程时先保存主流程上下文到调度器的ctx_main中，然后执行协程上下文，此时协程状态为RUNNING，当调用coroutine_yield时，保存当先上下文到协程结构体的ctx中，然后执行主流程上下文ctx_main，此时协程状态为SUSPEND，当调用coroutine_resume时，保存主流程上下文到调度器的ctx_main中，然后执行协程结构体的ctx上下文，当执行完回调函数后，协程进入DEAD状态

```cpp
// 协程结构体
typedef struct coroutine {
    void* (*call_back)(struct schedule*, void*); // 回调函数指针
    void* args;          // 回调函数参数
    ucontext_t ctx;      // 协程上下文
    char stack[STACKSZ]; // 协程栈
    enum State state;    // 协程状态
} coroutine_t;
```

## 使用协程实现一个TCP服务器

tcp_init: 创建tcp通信套接字，设置端口复用，绑定地址信息，进入listen状态，返回监听套接字文件描述符
set_nonblock: 将监听套接字文件描述符设置为非阻塞
accept_conn: 创建epoll对象，并将监听套接字加入。当有描述符就绪时，首先判断描述符类型，若为监听套接字则执行accept，为其创建协程结构体，并将新来的cfd加入epoll中，执行协程，以参数的形式传递cfd和epfd，协程函数遇到阻塞则让出cpu；若为通信套接字，则遍历调度器中的协程，找到协程回调参数中cfd等于就绪文件描述符的协程，恢复协程即可

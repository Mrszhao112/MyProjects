# coroutine / 协程

## 开发语言
C
## 开发环境
CentOS7、vim、gcc、gdb、git、MakeFile
## 项目介绍
“协程”即用户态下的非抢占式的轻量级线程，是一种在程序开发中处理多任务的组件。
由于在C/C++中并没有引入协程这一概念，而大部分开源的库又过于重量，所以我基于ucontext组件实现了一个简单的协程库
## 项目特点
- 用户态实现协程的调度切换，减少了内核切换的开销。
- 非抢占式，用户自己实现调度，同一时间只能有一个协程在执行，由协程主动交出控制权。
- 基于非对称(asymmetric)模式, 控制流更加简单，程序更加结构化。
- 协程具有独立的栈，确保运行效率。
## 适用场景
协程主要适用于I/O密集型的场景，如示例中的TCP服务器。在传统的多路复用+多线程/多进程的做法，每并发一个进程/线程就会消耗内存，并且最严重的问题就是由系统来进行调度切换带来的严重损耗，而协程刚好能够解决这些问题。
## 博客
[ucontext族函数的使用及原理分析](https://github.com/Mrszhao112/MyProjects/blob/main/coroutine_c/doc/ucontext.md)

[使用说明及示例](https://github.com/Mrszhao112/MyProjects/blob/main/coroutine_c/doc/coroutine.md)

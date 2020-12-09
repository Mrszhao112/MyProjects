//协程的类和对象的实现
//封装一个类 提供功能接口 每一个实例化的类都可以提供协程的功能接口
//进行上下文切换


//封装调度器
class schedule_t{

};
//封装协程
class coroutine_t{
  
};

class A {
  public:
    A(){}
    ~A(){}
//协程功能函数
  private:
    schedule_t   _schedule;
    coroutine_t  _coroutine;
};
int main(){
  
  return 0;
}

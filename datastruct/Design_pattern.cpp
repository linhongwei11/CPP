//1、单例模式，分为懒汉式和饿汉式

#include<bits/stdc++.h>
// #include<memory>
using namespace std;
//饿汉式 提前就已经确定好了，此时就没有所谓的线程安全问题 但这种程序加载时会慢一点，毕竟是static，运行时进行初始化
#if 0
class A
{
public:
    static A* getInstance()
    {
        return &_a;      
    }
    
private:
    A(string c)
    {
        cout<<c<<endl;
    };
    A(const A& a)=delete;
    A& operator=(const A& a)=delete;
    static A _a;
};
A A::_a("_a init():");
#endif


#if 0
class A
{
public:
    static A* getInstance()
    {
        static A _a("_a init():");
        return &_a;      
    }
private:
    A(string c)
    {
        cout<<c<<endl;
    };
    A(const A& a)=delete;
    A& operator=(const A& a)=delete;

};
#endif
#if 0
mutex m;
class A
{
public:
    // 锁加双重判断是最安全的
    static A* getInstance()
    {
       if(_a==nullptr)
       {
           unique_lock<mutex> lock(m);
           if(_a==nullptr)
            _a=new A("_a init():");
       }    
       return _a;       
    }
private:
    A(string c)
    {
        cout<<c<<endl;
    };
    A(const A& a)=delete;
    A& operator=(const A& a)=delete;
    static A* volatile  _a;
};
A* volatile A::_a=nullptr;
#endif




// 工厂模式的使用
//主要是3种
//简单工厂、工厂方法、抽象工厂
// 其实简单工厂也能实现工厂方法的情况，但是不满足封装闭合的特性，增加新的情况比较麻烦。
//factory(codec*)->respondFactory(respondcodec*)->requestfactory(requestcodec*)  项目里面的3个工厂
class CAR
{
public:
};
class AUDI:public CAR
{
public:
    AUDI(string c)
    {
        cout<<c<<endl;
    }
};
class BWM:public CAR
{
public:
    BWM(string c)
    {
        cout<<c<<endl;
    }
};
class simpleFactory
{
public:
    CAR* create(int mode)
    {
        switch(mode)
        {
            case 0: return new AUDI("a6");
            case 1: return new BWM("s");
            default:
            cout<<"error"<<endl;
            return nullptr;
        }
    }
};
//工厂方法模式的使用
//对于抽象工厂来说，完全就是在工厂方法的继承上增加东西
class Factory
{
public:
    //如果是抽象工厂的话，则可以再加如车灯这些的。
    virtual CAR* create()=0;
    Factory(){};
};
class BWMFactory:public Factory
{
public:
    virtual CAR* create()
    {
        if(_mode)
        {
            return new BWM(_s);
        }   
        else
            return new BWM(".");
    }
    BWMFactory(string s=""):_s(s)
    {
        _mode=false;
        if(!s.empty())
        {
            _mode=true;
        }
    }
private:
    string _s;
    bool _mode;
};
class AUDIFactory:public Factory
{
public:
    virtual CAR* create()
    {
        if(_mode)
        {
            return new AUDI(_s);
        }   
        else
            return new AUDI(".");
    }
    AUDIFactory(string s=""):_s(s)
    {
        _mode=false;
        if(!s.empty())
        {
            _mode=true;
        }
    }
private:
    string _s;
    bool _mode;
};

//3、代理模式
//通过代理类来控制实际对象的访问权限


//装饰器和适配器这两个倒是有一点相似之处
//装饰器类里面有基类car，因此是把car的工能与自己的功能结合到一起了，就是利用子类扩展功能的
//适配器这个也是利用子类，只不过是利用子类进行转换

//4、装饰器模式
//其实就是有好几个装饰的类，其实就是相当于把一些想要的功能单独的独立出来了，然后利用多态的方式又能加到任何一个子类里面

//5、适配器模式
//让不兼容的接口可以一起工作，
//适配器继承了原本需要调用的VGA类，然后，在里面初始化的时候却接收了待转换的HDMI接口，最后在自己的需要被调用的函数里面掉用了HDMI里的功能。所以后面只需要把转换器再送回去就行了

//6、观察者模式
//主要关注的是对象之间的通信。
//其实就是对于一个主题来说，
int main()
{
    #if 0
    A* a=A::getInstance();
    A* b=A::getInstance();
    #endif

    #if 1 
    unique_ptr<Factory>f(new AUDIFactory("a6"));
    unique_ptr<CAR>car(f->create());

    #endif
    cin.get();
    

}
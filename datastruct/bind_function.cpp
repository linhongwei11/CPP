#include<bits/stdc++.h>
using namespace std;
//占位用的
using namespace placeholders;

/*
实现了bind1st bind2st find_if
*/

template<typename Container>
void show(Container&con)
{
    typename Container::iterator p=con.begin();
    for(;p!=con.end();++p)
    {
        cout<<*p<<" ";
    }
    cout<<endl;
}


//要知道是传入一个2元的比较函数，现在你要将其转化成一个一元的比较函数，其实就是绑定了一个数了，很简单实现
template<typename Compare,typename T>
class _mybind1st
{
public:
    _mybind1st(Compare cmp,const T&val):_comp(cmp),_val(val)
    {       
    }
    bool operator()(const T& second)
    {
        return _comp(_val,second);
    }
private:
    Compare _comp;
    T _val;
};

//STL绑定器的实现
template<typename T,typename Compare>
_mybind1st<Compare,T> mybind1st(Compare cmp,const T&val)
{
    return _mybind1st<Compare,T>(cmp,val);
}

template<typename T,typename Compare>
class _mybind2st
{
public:
    _mybind2st(const T& val,Compare cmp):val(val),cmp(cmp){}
    bool operator()(const T& first)
    {
        return cmp(first,val);
    }
private:
        T val;
        Compare cmp;
};

template<typename T,typename Compare>
_mybind2st<T,Compare> mybind2st(const T& val,Compare cmp)
{
    return _mybind2st<T,Compare>(val,cmp);    
}
//自己实现的find_if函数
//其实就是找一个比较一下
template<typename Iterator,typename Compare>
Iterator myFind_if(Iterator first,Iterator last,Compare cmp)
{
    auto p=first;
    for(;p!=last;++p)
    {
        if(cmp(*p))
        {
            return p;
        }
    }
    return last;
}









/*
 实现了function,function的用法其实类似vector这种，它是一个模板类，完全是可以和map这些一起用的，也没什么大不了的
 当成一个类去处理就行了
*/


template<typename Ty>
class myfunction
{};


//如果都这么写的话，就太复杂了，毕竟还有3个、4个参数的
#if 0
// 部分特例化版本 1个参数
template<typename T,typename Ty>
class myfunction<Ty(T)>
{
public:
    using PFUNC=Ty(*)(T);
    myfunction(PFUNC pfunc):_pfunc(pfunc){}
    Ty operator ()(T arg)
    {
        return _pfunc(arg);
    }
private:
    PFUNC _pfunc;

};

// 部分特例化版本 2个参数
template<typename Ty,typename T1,typename T2>
class myfunction<Ty(T1,T2)>
{
public:
    using PFUNC =Ty(*)(T1,T2);
    myfunction(PFUNC pfunc):_pfunc(pfunc){}
    Ty operator ()(T1 arg1,T2 arg2)
    {
        _pfunc(arg1,arg2);
    }
private:
    PFUNC _pfunc;
};
#endif
// 

//可变参的写法,这种就是无敌的，不管怎么样，都能拿下
template<typename Ty,typename ... T>
class myfunction<Ty(T ...)>
{
public:
    using PFUNC=Ty(*)(T ...);
    myfunction(PFUNC pfunc):_pfunc(pfunc){}
    Ty operator ()(T ... arg)
    {
        return _pfunc(arg ...);
    }
private:
    PFUNC _pfunc;

};
void fun1(string s)
{
    cout<<s<<endl;
}
void fun2(string s,string s2)
{
    cout<<s<<s2<<endl;
}
void fun3(string s)
{
    cout<<s<<endl;
}



//bind和function结合使用，实现线程池

//线程类就是建立线程
class Thread
{
public:
    //利用func来获取函数
    Thread(function<void(int)>func,int no):_func(func),_no(no)
    {}

    thread start()
    {
        thread t(_func,_no);
        return t;
    }
private:
    function<void(int)>_func;
    int _no;
};
class ThreadPool
{
public:
    ThreadPool(){}
    ~ThreadPool()
    {
        for(int i=0;i<_pool.size();++i)
        {
            delete _pool[i];
        }
    }
    void startPool(int size)
    {
        for(int i=0;i<size;++i)
        {
            _pool.push_back(new Thread(bind(&ThreadPool::work,this,_1),i));
        }
        for(int i=0;i<size;++i)
        {
            _handler.push_back(_pool[i]->start());
        }
        for(thread &t :_handler)
        {
            t.join();
        }

    }
private:
    vector<Thread*> _pool;
    vector<thread> _handler;
    void work(int id)
    {
        cout<<"call thread id:"<<id<<endl;
    }
};




int main()
{

    bind(fun1,"hello")();
    bind(fun1,_1)("hello");
    //bind和function的结合使用
    //placeholders::_1 占位  
    function<void(string)> f=bind(fun1,_1);
    f("hello");

    ThreadPool t;
    t.startPool(10);
    
    
#if 0
    myfunction<void(string)>f=fun1;
    myfunction<void(string,string)>f2=fun2;

    f("tmp");
    f2("tmp1","tmp2");
    cin.get();
#endif
    

#if 0
    vector<int> p;
    for(int i=0;i<20;++i)
    {
        p.push_back(rand()%100);
    }
    sort(p.begin(),p.end(),greater<int>());
    show(p);
    //
    //greater  a>b
    //less a<b
    // 插入70
    auto ptr=myFind_if(p.begin(),p.end(),mybind1st(greater<int>(),70));
    p.insert(ptr,70);

    show(p);
    

    auto ptr1=myFind_if(p.begin(),p.end(),mybind2st(90,less<int>()));
    p.insert(ptr1,90);
    show(p);
#endif
    cin.get();
}
#include<iostream>
#include<functional>
#include<future>
using namespace std;




#if 0
class Test
{
public:
    void show(string s)
    {
        cout<<s<<endl;
    }
    ~Test()
    {
        cout<<"~Test()"<<endl;
    }
};

template<typename T>
class func
{
};

template<typename Ay,typename ...T>
class func<Ay(T ...)>
{
public:
    using FUN =Ay (*)(T ...);
    func(FUN mf):mf_(mf){}
    Ay operator()(T ...arg)
    {
        return mf_(arg ...);
    }
private:
    FUN mf_;
};



// template<typename Compare,typename T>
// class _mybind1st
// {
// public:
//     _mybind1st(Compare cmp,const T&val):_comp(cmp),_val(val)
//     {       
//     }
//     bool operator()(const T& second)
//     {
//         return _comp(_val,second);
//     }
// private:
//     Compare _comp;
//     T _val;
// };
// template<typename T,typename Compare>
// _mybind1st<Compare,T> mybind1st(Compare cmp,const T&val)
// {
//     return _mybind1st<Compare,T>(cmp,val);
// }

template<typename cmp,typename T>
class _mybind1
{
    public:
    _mybind1(cmp c,T t):cmp_(c),arg(t)
    {

    }
    bool operator()(const  T arg2)
    {
        return cmp(arg,arg2);
    }
    private:
    cmp cmp_;
    T arg; 
};



template<typename cmp,typename T>
_mybind1<cmp,T> getbind1(cmp c,T t)
{
    return _mybind1<cmp,T>(c,t);
}



void hello(string c)
{
    cout<<c<<endl;
}

#endif









//利用函数指针实现的部分特例化  这里依然是将所有的参数合一起作为一个函数指针类型参数，这种部分特例化要记住


template<typename T>
class myfun
{};
template<typename Ay,typename ... Args>
class myfun<Ay(Args ...)>
{
public:
    using FUNC=Ay(*)(Args ...);
    myfun(FUNC f):fun(f)
    {

    }
    Ay operator()(Args ...args)
    {
        return fun(args...);
    }
private:
    FUNC fun;

};








template<typename Func,typename T>
class mybind1
{
public:
    mybind1(Func f,T a):f(f),val(a)
    { }
    bool operator()(const T& a)
    {
        return f(val,a);
    }
private:
    Func f;
    T val;
};

template<typename Func,typename T>
mybind1<Func,T> _mybind(Func f,T a)
{
    return mybind1<Func,T>(f,a);
}



template<typename Func,typename T>
class mybind2
{
public:
    mybind2(Func f,T val):fun_(f),val_(val)
    {}
    bool operator()(const T& val)
    {
        return fun_(val,val_);
    }
private:
    Func fun_;
    T val_;
};

template<typename Func,typename T>
mybind2<Func,T> _mybind2(Func f,T val)
{
    return mybind2<Func,T>(f,val);
}

template<typename T>
class funct
{       
};

template<typename Ay,typename ... Args>
class funct<Ay(Args ...)>
{
public:
    using PFUNC=Ay(*)(Args ...);       
    funct(PFUNC f):fun_(f)
    {}
    Ay operator ()(Args ... arg)
    {
        return fun_(arg ...);
    }
private:
    PFUNC fun_;
};

int hello(string c)
{
    cout<<c<<endl;
    return c[0]-'0';
}
int main()
{
    std::packaged_task<int(string)> task(hello);
    future<int> f=task.get_future();
    task("ss");
    cout<<f.get()<<endl;
    #if 0
    int a=10;
    mybind2<std::greater<int>,int> p(std::greater<int>(),a);
    cout<<p(1)<<endl;
    // bind(&Test::show,Test(),"nihao")();
    // //临时变量是不能取地址的，所以如果想要用取地址的话就不能用临时变量的。
    // function<void(string)> f;
    // {
    //     Test a;
    //     //bind这里可以用Test()作为值传递，也可以使用局部变量取地址的
    //     f=bind(&Test::show,&a,std::placeholders::_1);
    // }
    // f("buhao");
    // f("xixi");
    // function<void(Test*,string)> f2=&Test::show;
    // // f2(&Test(),"hehe");  用临时变量取地址会报错

    funct<void(string)> fs=hello;
    fs("ssssssss");
    #endif
    std::cin.get();

}


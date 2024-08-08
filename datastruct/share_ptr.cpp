#include<iostream>
#include<bits/stdc++.h>
using namespace std;
//自己写的一个智能指针类
//1、先考虑清楚数据域部分，use_count就应该是指针,指向数据域的指针，只有这样才能保证计数的同步
//2、对= * ->的重载
//3、构造函数（特别是对于普通指针的那个构造函数，要记得是new int,创建一个指针）


//资源进行引用计数的类
template<typename T>
class RefCnt
{
public:
    // friend class Share_ptr;s
    RefCnt(T* ptr):mptr(ptr)
    {
        if(mptr)
        {
            mcount=1;
        }
    }
    void addRef()
    {
        mcount++;
    }
    int delRef()
    {
        return --mcount;
    }
private:
    int mcount; //在使用的时候要用原子类，不然会出问题的
    T* mptr;

};

template <typename T>
class Share_Ptr
{
public:
    Share_Ptr(T *ptr=nullptr):m_ptr(ptr){
        mpRefCnt=new RefCnt<T>(m_ptr);
    }
    Share_Ptr(const Share_Ptr<T> &e):mpRefCnt(e.mpRefCnt),m_ptr(e.m_ptr)
    {
        if(m_ptr)
            mpRefCnt->addRef();
    }
    Share_Ptr<T>& operator =(const Share_Ptr<T>& e)
    {
        if(this!=&e)
        {
            if(0==mpRefCnt->delRef())
            {
                cout<<"use count 0"<<endl;
                delete mpRefCnt; 
                delete m_ptr;
            }
            m_ptr=e.m_ptr;
            mpRefCnt=e.mpRefCnt;
            mpRefCnt->addRef();
        }
        return *this;
    }
    int get_count(){return *use_count;};
    T& operator*(){return *(this->m_ptr);};
    //对于指向符的重载：一般是ptr.operator->()->fun()
    T* operator->() {return m_ptr;}
    ~Share_Ptr()
    {
        
        if(mpRefCnt->delRef()==0)
        {
            cout<<"use count 0"<<endl;
            delete mpRefCnt; 
            mpRefCnt=nullptr;  
            delete m_ptr; 
            m_ptr=nullptr;
        }
    }
    private:
        // 引用计数作为一个全局的变量，大家应该是只有其指针
        int* use_count;
        RefCnt<T>* mpRefCnt;    
        T* m_ptr;
 };

template<typename T>
class myDeleter
{
public:
    void operator()(T* ptr) 
    {
        cout<<"void operator()(T* ptr) "<<endl;
        delete [] ptr;
        ptr=nullptr;
    }
};


//
template<typename T>
class myFileDeleter
{
public:
    void operator()(T* ptr) const
    {
        cout<<"void fileoperator()(T* ptr) "<<endl;
        fclose(ptr);
    }
};

int main()
{
    #if 0
    Share_Ptr<int> ptr1(new int) ;
    Share_Ptr<int> ptr2(ptr1) ;
    Share_Ptr<int> ptr3 ;
    ptr3=ptr2;
    *ptr1=20;
    cout<<*ptr2<<*ptr3<<endl;
    #endif
    //当智能指针管理的是一个数组的首地址时，默认的删除器就不好使了
    {
        unique_ptr<int,myDeleter<int>> ptr1(new int[100]);
        unique_ptr<FILE,myFileDeleter<FILE>> ptr2(fopen("data.txt","w"));

        // 定义模板类太麻烦了，直接用lambda来弄删除器
        unique_ptr<int,function<void(int*)>> ptr3(new int[100],[](int *p)->void
        {
            cout<<"call lambda"<<endl;
            delete [] p;
        });
        unique_ptr<FILE,function<void(FILE*)>> ptr4(fopen("data.txt","w"),[](FILE *p)->void
        {
            cout<<"file call lambda"<<endl;
            fclose(p);
        });
    }

    return 0;
}




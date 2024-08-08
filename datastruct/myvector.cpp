#include<bits/stdc++.h>
#include<map>
using namespace std;
// 默认空间大小

/*
    1、实现基本的vector
    2、增加内存分配器
    3、增加迭代器
    4、给容器提供失效处理
    5、右值引用的应用(move forward)
    6、增加emplace_back的实现
*/


// 进行
class CMyString
{
public:
    CMyString(const char* ptr)
    {
         cout<<"CMyString()"<<endl;
        if(ptr==nullptr)
        {
            m_ptr=new char[1];
            m_ptr[0]='\0';
        }
        else
        {
            m_ptr=new char[strlen(ptr)+1];
            strcpy(m_ptr,ptr);
        }
    }
    ~CMyString()
    {
        cout<<"~CMyString()"<<endl;
        delete []m_ptr;
        m_ptr=nullptr;

    }
     CMyString(const CMyString& str)
    {
         cout<<"CMyString(const CMyString& str)"<<endl;

         m_ptr=new char[strlen(str.m_ptr)+1];
         strcpy(m_ptr,str.m_ptr);
    }
    //接收右值引用的拷贝构造函数
     CMyString(CMyString&& str)
    {
         cout<<"CMyString(CMyString&& str)"<<endl;
         m_ptr=str.m_ptr;
         //让它空，这样避免了冲突（释放内存出错）
         str.m_ptr=nullptr;
    }

    // 这种就避免了拷贝构造的
    CMyString& operator=(const CMyString& str)
    {
        cout<<"CMyString& operator=(const CMyString& str)"<<endl;
        if(this==&str)
        {
            return *this;
        }
        delete [] m_ptr;
        m_ptr=new char[strlen(str.m_ptr)+1];
        strcpy(m_ptr,str.m_ptr);
        return *this;
    }
    //接收右值引用的=重载函数
    CMyString& operator=(CMyString&& str)
    {
        cout<<"CMyString& operator=(const CMyString& str)"<<endl;
        if(this==&str)
        {
            return *this;
        }
        delete [] m_ptr;
        m_ptr=str.m_ptr;
        str.m_ptr=nullptr;
        return *this;
    }
    friend CMyString operator+(const CMyString& lhs,const CMyString& rhs);
private:
    char* m_ptr;
};

// 这里生成的是一个临时变量，不能用指针或者引用作为返回
CMyString operator+(const CMyString& lhs,const CMyString& rhs)
{
    // 首先是开辟内存
    char* ptmp=new char[strlen(lhs.m_ptr)+strlen(rhs.m_ptr)+1];
    strcpy(ptmp,lhs.m_ptr);
    strcat(ptmp,rhs.m_ptr);
    CMyString s(ptmp);
    delete[] ptmp;
    ptmp=nullptr;
    return s;
}


// 创建allocator类
template<typename T>
class ALocator
{
public:
    T* allocate(size_t size)
    {
        return (T*)malloc(sizeof(T)*size);
    }

    void deallocate(void *p)
    {
        free(p);
    }
    //改造一下,

    template<typename ... Ty>
    void construct(T* p,Ty&&...val)
    {
        new (p) T(forward<Ty>(val)...);
    }
#if 0
    // 定位new
    void construct(T* p,const T&val)
    {
        new (p) T(val);
    }
    // 右值引用
     void construct(T* p,const T&&val)
    {
        new (p) T(move(val)  );
    }
#endif
    void destroy(T* p)
    {
        p->~T();
    }
};


// 这里有2版 vector的实现方法，一版没有借助内存分配器，因此就有点尴尬，无法将类的建立和内存的开辟融合在一起
template<typename T,typename Alloc=ALocator<T>>
class myvector
{
public:
    // 在vector中加入迭代器
    class Iterator
    {
    private:
        T* ptr;
        //增加容器的地址，能发现迭代器所代表的那个容器有没有失效
        myvector<T,Alloc>*_pvec;
    public:
        friend class myvector<T,Alloc>;
        Iterator(myvector<T,Alloc>*pvec,T* ptr=nullptr):ptr(ptr),_pvec(pvec){

            //每次新的节点都插在头节点的后面
            Iterator_base *itb=new Iterator_base(this,_pvec->_head.next);
            _pvec->_head.next=itb;
        }
        bool operator !=(const Iterator& tmp)
        {
            /* 迭代器里的不等主要还是针对于其代表的元素，
            而不是其他，因此不应该是比较  &tmp!=this
            而应该是相应的元素之间的比较
            */
            //检查迭代器的有效性  要明确知道_pvec的含义，是数组的指针，这个就是要保证你比较的迭代器是一个数组的才行，不同数组的就没有意义
            if(_pvec==nullptr||_pvec!=tmp._pvec)
            {
                cout<< "iterator incompatable"<<endl;
                return false;
            }
            return tmp.ptr!=ptr;
        }
        // 前置++
        void operator++()
        {
           ++ptr;   
        }

        //去vector的首地址就可以&*v.begin()    先取首元素，再取首元素地址
        T& operator *()
        {
            return *ptr;
        }
        //  friend void verify(T*fir,T* last);
    };
    Iterator begin()
    {
        return Iterator(this,start);
    }
    Iterator end()
    {
        return Iterator(this,last);
    }

   



    // 直接写int size=20，用这种默认初始化的方式，更好
    // myvector():size(20),curSize(0){
    //     start=new T[20];
    // }

    // 有这种默认初始化，相当于2个函数了，带不带参都行
    myvector(int size=5){
        //将开辟和对象生成分开
        // start=new T[size];


        // 内存的开辟（借助内存分配器版本）
        start=_allocater.allocate(size);
        last=start;
        // 这里这个T一定是要重载过加法的，不然这么直接加肯定是有问题的
        end1=start+size;
    }

    //拷贝构造函数
    myvector(const myvector<T>& t)
    {
        int size=t.end1-t.start;
        int len=t.last-t.start;

        // start =new T[size];
        //开辟空间
        start=_allocater.allocate(size);

        //在开辟的空间上生成对象
        for(int i=0;i<len;++i)
        {
            // start[i]=t.start[i];
            _allocater.construct(start+i,t.start[i]);
        }
        last=start+len;
        end1=start+size;

    }
    //这种写两个函数来区分左值右值太麻烦了，直接利用模板来
    template<typename Ty>
    void push_back(Ty &&val)   //这里有个引用折叠的概念，&&&=& &&&&=&&,就是其实就是个幌子，没啥用的
    {
        if(isFull())
        {
            resize();
        }
        // forward是类型的完美转发，能完美转发左值或者是右值
        _allocater.construct(last,std::forward<Ty>(val));
        ++last;
    }
    template<typename ... Ty>
    void emplace_back(Ty&& ... val )
    {
        _allocater.construct(last,std::forward<Ty>(val)...);
        ++last;

    }


#if 0
    //去类外实现了
    void push_back(const T& t);
    // 右值引用
    void push_back(T&& val)
    {
         if(isFull())
        {
            resize();
        }
        // val虽然是一个右值引用，但它本身却是一个左值，因此这里应该用move将其转化为右值
        _allocater.construct(last,std::move(val));
        ++last;
    }
#endif
    void pop_back()
    {
    if(isEmpty())
    {
        return;
    }
    verify(last-1,last);
    last--;
    _allocater.destroy(last);
    
}
    // myvector<T>& 这种实现连等，如果用void无法实现连等
    myvector<T>& operator =(myvector<T>& t)
    {
        if(this==&t)
            return *this;
        // delete [] start;
         // 先析构对象
        for(T* p=start;p!=last;++p)
        {
            _allocater.destroy(p);
        }

        // 再释放空间
        _allocater.deallocate(start);


        int size=t.end-t.start;
        int len=t.last-t.start;

        start=_allocater.allocate(size);

        //在开辟的空间上生成对象
        for(int i=0;i<len;++i)
        {
            // start[i]=t.start[i];
            _allocater.construct(start+i,t.start[i]);
        }


        last=start+len;
        end=start+size;
        return *this;

    }

    
    bool isFull()const{
    return last==end1;
}
    bool isEmpty()const{
    return last==start;
}
    void getAllElement()const
    {
        int len=last-start;
        cout<<"len"<<len<<endl;
        for(int i=0;i<len;++i)
        {
            cout<<start[i]<<"   ";
        }
    }
    ~myvector()
    {
        // delete [] start;  //不是所有的空间里都是对象，毕竟不一定装满


        // 先析构对象
        for(T* p=start;p!=last;++p)
        {
            _allocater.destroy(p);
        }

        // 再释放空间
        _allocater.deallocate(start);
        start=last=end1=nullptr;
    }
     // 新增的verify方法来检查迭代器失效
    void verify(T*fir,T* last)
    {
        // 这一个next可能有点大，不要混淆了
        Iterator_base *pre=&this->_head;
        Iterator_base* it=this->_head.next;
        while(it)
        {
            if(it->cur->ptr>fir&&it->cur->ptr<=last)
            {
                // 进行迭代器失效,直接让指向的容器为空了，直接就是失效了，直接让你都不是我们这个容器的了
                it->cur->_pvec=nullptr;
                // 删除当前迭代器节点，继续判断后面的迭代器节点是否失效
                pre->next=it->next;
                //迭代器只有指针，其他啥也没有，所以删了也就只是删了个指针罢了，对容器具体的数据什么的，其实没有影响
                delete it;
                it=pre->next;
            }
            else
            {
                pre=it;
                it=it->next;
            }
        }
    }
    //inset和erase的实现
    Iterator insert(Iterator it,const T& val)
    {
        //不考虑扩容和it的合法性
        verify(it.ptr-1,last);
        T* p=last;
        //插入点前面的元素都要往前移
        while(p>it.ptr)
        {
            _allocater.construct(p,*(p-1));
            _allocater.destroy(p-1);
            p--;
        }
        _allocater.construct(p,val);
        last++;
        return Iterator(this,p);
    }
     Iterator erase(Iterator it)
    {
        //不考虑扩容和it的合法性
        verify(it.ptr-1,last);
        T* p=it.ptr;
        while(p<last-1)
        {
            _allocater.destroy(p);
            _allocater.construct(p,*(p+1));
            ++p;
        }
        _allocater.destroy(p);
        last--;
        return Iterator(this, it.ptr);
    }
private:
    // 当空间不够时，自动进行扩容
    void resize()
    {
        int size=(end1-start)*2;
        int len=last-start;
        // T* newStart=new T[size];
        T* newStart=_allocater.allocate(size);

        // 要用for，防止T类型有指向外部空间的指针
        for(int i=0;i<len;++i)
        {
            // newStart[i]=start[i];
            _allocater.construct(newStart+i,start[i]);
        }   
        // delete [] start;
        for(T* p=start;p!=last;++p)
        {
            _allocater.destroy(p);
        }
        // 再释放空间
        _allocater.deallocate(start);


        start=newStart;
        last=start+len;
        end1=start+size;
    }


    // 增加类来实现确认迭代器位置
    struct Iterator_base
    {
        /* data */
        Iterator_base(Iterator *c =nullptr,Iterator_base *n=nullptr):cur(c),next(n)
        {}
        Iterator* cur;
        Iterator_base* next;
    };
    Iterator_base _head;


    T* start; //数组起始位置
    T* end1;   //数组空间的后继位置
    T* last;  //数组中有效元素的后继位置
    Alloc _allocater;  //内存分配器
    
};

#if 0
// template<typename T,typename E=int>
// class A
// {
// public:
//     void test();
// };

// template<typename T,typename E>
// void A<T,E>::test()
// {

// }
// template<typename T,typename Alloc=ALocator<T>>
// bool myvector<T>::isFull()const
// {
//     return last==end;
// }

// template<typename T,typename Alloc=ALocator<T>>
// bool myvector<T,ALocator<T>>::isEmpty()const
// {
//     return last==start;
// }
#endif
#if 0
template<typename T,typename Alloc> 
void myvector<T,Alloc>::push_back(const T& t)
{
    if(isFull())
    {
        resize();
    }
    _allocater.construct(last,t);
        ++last;

}
#endif
#if 0
// template<typename T,typename Alloc=ALocator<T>>
// void myvector<T,ALocator<T>>::pop_back()
// {
//     if(isEmpty())
//     {
//         return;
//     }
//     last--;
//     _alloctor.destroy(last);
    
// }
#endif
class Test
{
public:
    Test(int a,int b):_a(a),_b(b)
    {
        cout<<"Test(int a,int b):_a(a),_b(b)"<<endl;
    }
    Test(int a):_a(a),_b(0)
    {
         cout<<"Test(int a):_a(a),_b(0)"<<endl;
    }
private:
    int _a;
    int _b;
};

vector<int> ans;

void dfs(vector<vector<int>>& matrix,int a,int b,int c,int d)
{
    if(a>b||c<d)
    {
        return;
    }

    for(int i=c;i<=d;++i)
    {
        ans.push_back(matrix[a][i]);
    }

    for(int i=a+1;i<=b;++i)
    {
        ans.push_back(matrix[i][d]);
    }

    for(int i=d-1;i>=c;--i)
    {
        ans.push_back(matrix[b][i]);
    }

    for(int i=b-1;i>a;--i)
    {
        ans.push_back(matrix[i][c]);
    }
}
  vector<int> spiralOrder(vector<vector<int>>& matrix) {
    int m=matrix.size();
    if(m==0)
        return {};
    int n=matrix[0].size();
    
    dfs(matrix,0,m-1,0,n-1);

    return ans;
}


int main()
{
    unordered_map<int,int> map_testA;
    unordered_map<int,int> map_testB;
    map_testA[0] = 1;
    map_testA[1] = 2;

    map_testB[2] = 5;
    map_testB[3] = 6;

    unordered_map<int,int>& t = map_testA;
    t = map_testB;
    for(auto& x: map_testA)
    {
        cout<< x.first << ":" << x.second <<endl;
    }

#if 0
    map<int,int> set;
    set.begin()->first;

    myvector<int> b;
    b.emplace_back(10);
    myvector<Test> c;
    c.emplace_back(10,20);
    c.emplace_back(10);




    b.push_back(10);
    b.push_back(10);
    b.push_back(10);
    b.push_back(10);
    b.push_back(10);
    auto itr=b.begin();
    for(;itr!=b.end();++itr)
    {
        if(*itr==10)
        {
            itr=b.insert(itr,1);
            ++itr;
        }
        // cout<<*itr<<endl;
    }
      for(auto x:b)
    {
        cout<<x<<endl;
    }
#endif
    // myvector<CMyString> a;
    // CMyString str1="aaa";
    
    // cout<<"---------------------------------------------"<<endl;
    // a.push_back(str1);
    // a.push_back(CMyString("bbb"));
    // cout<<"---------------------------------------------"<<endl;
    return 0;


#if 0
    a.push_back(10);
    myvector<string> b;
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    b.push_back("aa");
    // b.pop_back();
    // b.getAllElement();
    auto it1=b.end();
    b.pop_back();
    auto it2=b.end();
    cout<<(it1!=it2)<<endl;

    // 利用迭代器实现
    // for(auto x:b)
    // {
    //     cout<<x<<endl;
    // }
    auto x=b.begin();
    for(;x!=b.end();++x)
    {
        cout<<*x<<endl;
    }
#endif
    cin.get();
}



//整理版本
template<typename T>
class Allcoator
{
public:
    T* allocatr(size_t size)
    {
        return (T*)malloc(sizeof(T)*size);
    }
    void deallocatr(T* ptr)
    {
        free(ptr);
    }

    template<typename Ty>
    void construct(T* ptr,Ty&& val)
    {
        new (ptr) T(std::forward<Ty>(val));
    }

    #if 0
    //const T&是能修饰左右值的
    void construct(T* ptr,const T& val)
    {
        new (ptr) T(val);
    }
    #endif
    void deconstruct(T* ptr)
    {
        ptr->~T();
    }
};

template<typename T,typename Alloc =Allcoator<T>>
class Vec
{
public:
    class Iterator
    {
        friend class Vec<T,Alloc>;
        public:
            // 把自己加入链表
            Iterator(Vec<T>*pvec=nullptr,T*ptr=nullptr):_ptr(ptr),_pvec(pvec)
            {
                Iterator_base* itb=new Iterator_base(this,_pvec->head._next);
                _pvec->head._next=itb;
            }

            T operator*()
            {
                return *_ptr;
            }

            bool operator!=(const Iterator& itr)
            {
                if(_pvec==nullptr||itr._pvec!=_pvec)
                {
                    return false;
                }
                return _ptr!=itr._ptr;
            }
            
            void operator ++()
            {
                ++_ptr;
            }

        private:
        T* _ptr;
        Vec<T,Alloc>* _pvec;
    };
    void verify(T* first,T*last)
    {
       Iterator_base* pre=&this->head;
       Iterator_base* node=this->head._next;
       while(node)
       {
           if(node->_cur->_ptr>first&&node->_cur->_ptr<last)
           {
               node->_cur->_pvec=nullptr;
               pre->_next=node->_next;
               delete node;
               node=pre->_next;
           }
           else
           {
               pre=node;
               node=node->_next;
           }
       }
    }
    Vec(int size=10)
    {
        _start= alloc.allocatr(size);
        _last=_start;
        _end=_start+size;       
    }
    Vec(const Vec<T>& v)
    {
        int size=v._end-v._start;
        int len=v._last-v._start;
        _start= alloc.allocatr(size);
        for(int i=0;i<len;++i)
        {
            alloc.construct(_start+i,v._start[i]);
        }
        _last=_start+len;
        _end=_start+size;
    }
    Vec<T>& operator=(const Vec<T>& v)
    {
        int size=_end-_start;
        int len=_last-_start;
        for(int i=0;i<len;++i)
        {
            alloc.deconstruct(_start);
        }
        alloc.deallocatr(_start);

        size=v._end-v._start;
        len=v._last-v._start;
        _start= alloc.allocatr(size);
        for(int i=0;i<len;++i)
        {
            alloc.construct(_start+i,v._start[i]);
        }
        _last=_start+len;
        _end=_start+size;
        return *this;
    }

    Iterator begin()
    {
        return Iterator(this,_start);
    }
    Iterator end()
    {
        return Iterator(this,_last);
    }


    bool isEmpty()
    {
        return _start==_last;
    }

    // 当push_back时，由于是最普通的值传递（用引用传递无法应对临时变量）
    // 那么怎么应对？
    // 折叠引用的概念

    template<typename Ty> 
    //这种引用是无敌的
    void push_back(Ty&& val)
    {
        if(_last==_end)
            resize();
        alloc.construct(_last,std::forward<Ty>(val));
        ++_last;
    }


    #if 0
    void push_back(T val)
    {
        if(_last==_end)
            resize();
        alloc.construct(_last,val);
        ++_last;
    }
    #endif

    void pop_back()
    {
        if(isEmpty())
        {
            cout<<"empty"<<endl;
            return ;
        }
        else
        {
            int lab=_last-_start-1;
            alloc.deconstruct(_start+lab);
            // alloc.deconstruct(_last-1);
            --_last;
        }
    }

    Iterator insert(Iterator itr,const T& val)
    {
        // 不考虑扩展问题
        // 全部失效
        verify(itr._ptr-1,_last);
        T* p=_last;
        while(p!=itr._ptr)
        {
            alloc.construct(p,*(p-1));
            alloc.deconstruct(p-1);
            --p;
        }
        alloc.construct(p,val);
        ++_last;
        return Iterator(this,p);
    }


    Iterator erase(Iterator itr)
    {
        verify(itr._ptr-1,_last);
        T* p=itr._ptr;
        while(p!=_last-1)
        {
            alloc.deconstruct(p);
            alloc.construct(p,*(p+1));
            ++p;
        }
        alloc.deconstruct(p);
        --_last;
        return Iterator(this,itr._ptr);
    }
    int size()
    {
        return _last-_start;
    }
    T& operator [](int n)
    {
        return _start[n];
    }
private:

    void resize()
    {
        int size=_end-_start;
        int len=_last-_start;
        T* ptr=alloc.allocatr(size*2);
        for(int i=0;i<len;++i)
        {
            alloc.construct(ptr+i,_start[i]);
            alloc.deconstruct(_start+i);
        }
        alloc.deallocatr(_start);
        _start=ptr;
        _last=_start+len;
        _end=_start+size*2;
    }
    class Iterator_base{
    public:
        Iterator_base(Iterator* cur =nullptr,Iterator_base* next=nullptr):_next(next),_cur(cur)
        {}
        Iterator_base* _next;
        Iterator* _cur;
    };
    T* _start;
    T* _last;
    T* _end;
    // 这里不能用指针了
    Iterator_base head;
    Alloc alloc;
};



// 写一个普通的类
class mString
{
public:
    // 普通的初始化
    mString(const char* ptr=nullptr)
    {
        cout<<"mString"<<endl;
        if(ptr==nullptr)
        {
            _mptr=new char('\0');
        }
        else
        {
            _mptr=new char[strlen(ptr)];
            strcpy(_mptr,ptr);
        }
    }
    // 拷贝初始化
     mString(const mString& s)
     {
        cout<<"mString(const mString& s)"<<endl;
        if(s._mptr==nullptr)
        {
            _mptr=new char('\0');
        }
        else
        {
            _mptr=new char[strlen(s._mptr)];
            strcpy(_mptr,s._mptr);
        }
     }
    //  赋值初始化
    mString& operator=(const mString& s)
    {
        cout<<" mString& operator=(const mString& s)"<<endl;
        if(this==&s)
        {
            return *this;
        }
        delete [] _mptr;

        if(s._mptr==nullptr)
        {
            _mptr=new char('\0');
        }
        else
        {
            _mptr=new char[strlen(s._mptr)];
            strcpy(_mptr,s._mptr);
        }
        return *this;
    }
    char* c_str()
    {
        return _mptr;
    }

    //右值引用时
    // 拷贝初始化
     mString(mString&& s)
     {
        cout<<" mString(mString&& s)"<<endl;
        if(s._mptr==nullptr)
        {
            _mptr=new char('\0');
        }
        else
        {
            _mptr=s._mptr;
            s._mptr=nullptr;
        }
     }
    mString& operator=(mString&& s)
    {
        cout<<"  mString& operator=(mString&& s)"<<endl;
        if(this==&s)
        {
            return *this;
        }
        delete [] _mptr;

        if(s._mptr==nullptr)
        {
            _mptr=new char('\0');
        }
        else
        {
            _mptr=s._mptr;
            s._mptr=nullptr;
        }
        return *this;
    }
private:
    char* _mptr;
};
#include<bits/stdc++.h>
using namespace std;





//可以用智能指针对这个进行升级处理
template<typename T>
class Queue{
public:
    Queue()
    {
        _front=_rear=new QueueItem();
    }
    void push(T& val)
    {
        QueueItem* tmp=new QueueItem(val);
        _rear->next=tmp;
        _rear=tmp;
    }
    void pop()
    {
        if(!_front->next)
            return;
        QueueItem* tmp=_front->next;
        _front->next=tmp->next;
        delete tmp;
        if(!_front->next)
            _rear=_front;
    }
    T front() const
    {
        return _front->next->data;
    }
    bool empty()const
    {
        return _front==_rear;
    }

      ~Queue()
    {
        auto tmp=_front;
        while(tmp)
        {
            _front=_front->next;
            delete tmp;
            tmp=_front;
        }
    }
private:
    // 对象池
    struct QueueItem
    {
        QueueItem(T data=T()):val(data),next(nullptr){}

        //给queenItem提供了自定义的内存管理
        void* operator new (size_t size)
        {
            if(_pool==nullptr)
            {
                //这里此时内存就开辟好了
                _pool=(QueueItem*)new char[POOL_SIZE*sizeof(QueueItem)];
                //进行初始化,保证是队列存储
                auto itr=_pool;
                //其实就是将链表连起来（这个和内存池就是一样的，只不过内存池没有固定到对象）
                for(;itr<_pool+POOL_SIZE;++itr)
                {
                    itr->next=itr+1;
                }
                itr->next=nullptr;
            }
            //然后也是一样的处理，如果对象池不为空，那就返回一个对象回去，并且将首节点往后移一个
            auto itr=_pool;
            _pool=_pool->next;
            return itr;
        }

        //这里其实就是一个对象的归还操作，重新插入到链表中
        void operator delete(void* ptr)
        {
            QueueItem* itr=(QueueItem*)ptr;
            itr->next=_pool;
            _pool=itr;

        }

        QueueItem* next;
        T val;
        static const int POOL_SIZE=100000;
        static QueueItem* _pool;
    };

  
    QueueItem* _front; //队首
    QueueItem* _rear;   //队尾
};
template<typename T>
typename Queue<T>::QueueItem* Queue<T>::QueueItem::_pool=nullptr;   //使用typename来保证后面是一个类，因为编译到这里进行代码检查时，由于还没有实例化，因此不确定queueItem是否是一共类





int main()
{
    Queue<int> q;
    for(int i=0;i<10000;++i)
    {
        q.push(i);
        q.pop();
    }

    cin.get();

}
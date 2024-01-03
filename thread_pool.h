#ifndef THREAD_POOL
#define THREAD_POOL
#include<iostream>
#include<memory>
#include<functional>
#include<condition_variable>
#include<thread>
#include<vector>
#include<queue>
#include<atomic>
#include<unordered_map>
#include<future>
enum  class PoolMode
{
	MODE_FIXED,
	MODE_CACHED
};







//不需要了


/***************************************
这4个类完全是可以缩减为一个future类以及packaged_task类
但还是得熟悉好Result和Any的运行原理
*********************************************/

class Sem
{
public:
	Sem(int num = 0) :num_(num), isValid_(true) {}
	~Sem()
	{
		isValid_ = false;
	}
	void wait()
	{
		if (!isValid_)
			return;
		std::unique_lock<std::mutex>lock(mtu_);
		cv_.wait(lock, [&]()->bool {return num_ > 0; });
		--num_;
	}
	void post()
	{
		if (!isValid_)
			return;
		std::unique_lock<std::mutex>lock(mtu_);
		++num_;
		cv_.notify_all();
	}
private:
	std::condition_variable cv_;
	std::mutex mtu_;
	int num_;
	std::atomic_bool isValid_;
};
//能接收所有的类型
class Any
{
public:
	Any() = default;
	~Any() = default;
	Any(const Any&) = delete;
	Any& operator=(const Any&) = delete;
	Any(Any&&) = default;
	Any& operator=(Any&&) = default;
	

	//接收所有的数据
	template<typename T>
	Any(T data):sp_(std::make_unique<Derive<T>>(data))
	{}
	template<typename T>
	T cast()
	{
		//这里取出真正的数据
		Derive<T>* p = dynamic_cast<Derive<T>*>(sp_.get());
		return p->data_;
	}
private:
	class Base
	{
	public:
		Base() = default;
		virtual ~Base() = default;
	};
	template<typename T>
	class Derive:public Base
	{
	public:
		Derive(T data) :data_(data) {}
		T data_;
	};

	std::unique_ptr<Base> sp_;
};

class Task;

class Result
{
public:
	Result(std::shared_ptr<Task> t, bool isValid=true);
	~Result();
	Any getVal();
	void setVal(Any any);

private:
	std::shared_ptr<Task> t_;
	Sem sem_;
	//只能右值传递的，因为他的成员变量也只是只能右值传递（unique_ptr）
	Any ret_;
	bool isValid_;

};







class Thread
{
public:
	using ThreadFunc = std::function<void(int)>;
	Thread(ThreadFunc func);
	~Thread();
	void start();
	int getId()const;

private:
	ThreadFunc func_;
	int threadId_;
	static int genneratorID_;

};

class Task
{
public:
	Task() :r_(nullptr) {}
	~Task() {}
	void exec();
	virtual Any run() = 0;
	void setResult(Result* r);
private:
	
	Result* r_;
	

};

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();
	ThreadPool(const ThreadPool& tp) = delete;
	ThreadPool& operator =(const ThreadPool& tp) = delete;

	void TestFunc(int id);
	
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	//使用C++17新特性优化代码
	//Result submmitTask(std::shared_ptr<Task> task);
	//可变参模板编程
	template<typename Func, typename ... Arg>
	auto submmitTask(Func&& func, Arg&&... args)->std::future<decltype(func(args ...))>   //后面的其实写不写也无所谓
	{
		using RType = decltype(func(args ...));
		//此时task就是一个f()函数了，因为参数都被绑定了
		auto task = std::make_shared<std::packaged_task<RType()>>(
			std::bind(std::forward<Func>(func), std::forward<Arg>(args)...));
		std::future<RType> result = task->get_future();
		// 获取锁
		std::unique_lock<std::mutex> lock(mtx);
		// 用户提交任务，最长不能阻塞超过1s，否则判断提交任务失败，返回
		if (!cv_notFull.wait_for(lock, std::chrono::seconds(1), [&]()->bool {
			return task_.size() < (size_t)TaskTh;
			}))
		{
			std::cout << "submmit error!" << std::endl;
			auto task = std::make_shared<std::packaged_task<RType()>>(
				[]()->RType { return RType(); });
			(*task)();
			return task->get_future();
		}
			//直接就不管返回值是什么，反正都是执行就行，返回值future会得到的（类内部的实现）
			task_.push([task]() {(*task)(); });
			task_size_++;


			//告诉所有人有任务了
			cv_notEmpty.notify_all();

			//动态创建线程
			if (poolMode_ == PoolMode::MODE_CACHED && task_size_ > thread_size_ && thread_size_ < ThreadSizeTh)
			{
				std::cout << ">>> create new thread..." << std::endl;
				std::unique_ptr<Thread> ptr = std::make_unique<Thread>(std::bind(&ThreadPool::TestFunc, this, std::placeholders::_1));
				int id = ptr->getId();
				threads_.insert({ id, move(ptr) });
				//一直没有开始线程，难怪出问题
				//ptr->id  这里为什么错？unique_ptr!
				threads_[id]->start();
				thread_size_++;
				idle_size_++;
			}
			return result;
	}

	void start(int init_size =std::thread::hardware_concurrency() );
	void  setThreadSizeTh(int size);
	void  setTaskSizeTh(int size);
	void  setPoolMode(PoolMode poolMode);

private:

	std::atomic_bool isPoolRuning;
	PoolMode poolMode_;

	using Task = std::function<void()>;

	std::unordered_map<int,std::unique_ptr<Thread>> threads_;
	int ThreadSizeTh;
	int initThreadSize;
	//当前的线程个数
	std::atomic_uint thread_size_;
	//空闲线程的个数
	std::atomic_uint idle_size_;
	



	
	std::queue<Task> task_;
	//最大的task阈值
	int TaskTh;
	std::atomic_uint task_size_;

	//进程通信用
	std::condition_variable cv_notFull;
	std::condition_variable cv_notEmpty;
	std::condition_variable cv_closePool;
	std::mutex mtx;

};




#endif // !THREAD_POOL

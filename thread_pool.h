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







//����Ҫ��


/***************************************
��4������ȫ�ǿ�������Ϊһ��future���Լ�packaged_task��
�����ǵ���Ϥ��Result��Any������ԭ��
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
//�ܽ������е�����
class Any
{
public:
	Any() = default;
	~Any() = default;
	Any(const Any&) = delete;
	Any& operator=(const Any&) = delete;
	Any(Any&&) = default;
	Any& operator=(Any&&) = default;
	

	//�������е�����
	template<typename T>
	Any(T data):sp_(std::make_unique<Derive<T>>(data))
	{}
	template<typename T>
	T cast()
	{
		//����ȡ������������
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
	//ֻ����ֵ���ݵģ���Ϊ���ĳ�Ա����Ҳֻ��ֻ����ֵ���ݣ�unique_ptr��
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
	
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	//ʹ��C++17�������Ż�����
	//Result submmitTask(std::shared_ptr<Task> task);
	//�ɱ��ģ����
	template<typename Func, typename ... Arg>
	auto submmitTask(Func&& func, Arg&&... args)->std::future<decltype(func(args ...))>   //�������ʵд��дҲ����ν
	{
		using RType = decltype(func(args ...));
		//��ʱtask����һ��f()�����ˣ���Ϊ������������
		auto task = std::make_shared<std::packaged_task<RType()>>(
			std::bind(std::forward<Func>(func), std::forward<Arg>(args)...));
		std::future<RType> result = task->get_future();
		// ��ȡ��
		std::unique_lock<std::mutex> lock(mtx);
		// �û��ύ�����������������1s�������ж��ύ����ʧ�ܣ�����
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
			//ֱ�ӾͲ��ܷ���ֵ��ʲô����������ִ�о��У�����ֵfuture��õ��ģ����ڲ���ʵ�֣�
			task_.push([task]() {(*task)(); });
			task_size_++;


			//������������������
			cv_notEmpty.notify_all();

			//��̬�����߳�
			if (poolMode_ == PoolMode::MODE_CACHED && task_size_ > thread_size_ && thread_size_ < ThreadSizeTh)
			{
				std::cout << ">>> create new thread..." << std::endl;
				std::unique_ptr<Thread> ptr = std::make_unique<Thread>(std::bind(&ThreadPool::TestFunc, this, std::placeholders::_1));
				int id = ptr->getId();
				threads_.insert({ id, move(ptr) });
				//һֱû�п�ʼ�̣߳��ѹֳ�����
				//ptr->id  ����Ϊʲô��unique_ptr!
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
	//��ǰ���̸߳���
	std::atomic_uint thread_size_;
	//�����̵߳ĸ���
	std::atomic_uint idle_size_;
	



	
	std::queue<Task> task_;
	//����task��ֵ
	int TaskTh;
	std::atomic_uint task_size_;

	//����ͨ����
	std::condition_variable cv_notFull;
	std::condition_variable cv_notEmpty;
	std::condition_variable cv_closePool;
	std::mutex mtx;

};




#endif // !THREAD_POOL

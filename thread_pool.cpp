#include"thread_pool.h"

const int maxTaskSize=1024;
const int maxTime = 5;


Thread::Thread(ThreadFunc func)
				:func_(func),
				 threadId_(genneratorID_++)
{}
Thread::~Thread()
{}
void Thread::start()
{
	std::thread t(func_,threadId_);
	t.detach();
}
int  Thread::getId()const
{
	return threadId_;
}

int Thread::genneratorID_ = 0;



ThreadPool::ThreadPool()
					   :initThreadSize(0),
						ThreadSizeTh(std::thread::hardware_concurrency()),
						TaskTh(maxTaskSize),
						thread_size_(0),
						idle_size_(0),
						task_size_(0),
						isPoolRuning(false),
						poolMode_(PoolMode::MODE_FIXED)
{

}

//实现任务关闭后再关闭线程
ThreadPool::~ThreadPool()
{
	isPoolRuning = false;
	std::unique_lock<std::mutex>lock(mtx);
	cv_notEmpty.notify_all();
	cv_closePool.wait(lock, [&]()->bool {return threads_.size() == 0; });
}
void ThreadPool::start(int init_size)
{
	//当前线程池一旦启动就不能调参(不能说启动后还能继续调参)
	isPoolRuning = true;
	initThreadSize = init_size;
	for (int i = 0; i < initThreadSize; ++i)
	{
		std::unique_ptr<Thread> ptr = std::make_unique<Thread>(std::bind(&ThreadPool::TestFunc, this,std::placeholders::_1));
		threads_.insert({ ptr->getId(), move(ptr) });
		thread_size_++;
		idle_size_++;
	}
	for (int i=0;i<initThreadSize;++i)
	{
		threads_[i]->start();
	}
}

#if 0
Result ThreadPool::submmitTask(std::shared_ptr<Task>task)
{
	std::unique_lock<std::mutex> lock(mtx);
	if (!cv_notFull.wait_for(lock, std::chrono::seconds(1), [&]()->bool {
		return task_.size() < (size_t)TaskTh;
		}))
	{
		std::cout << "submmit error!" << std::endl;
		return Result(task,false);
	}

	task_.push(task);
	task_size_++;
	

	//告诉所有人有任务了
	cv_notEmpty.notify_all();

	//动态创建线程
	if (poolMode_== PoolMode::MODE_CACHED && task_size_ > thread_size_ && thread_size_ < ThreadSizeTh)
	{
		std::cout << ">>> create new thread..." << std::endl;
		std::unique_ptr<Thread> ptr = std::make_unique<Thread>(std::bind(&ThreadPool::TestFunc, this, std::placeholders::_1));
		threads_.insert({ ptr->getId(), move(ptr) });
		//一直没有开始线程，难怪出问题
		ptr->start();
		thread_size_++;
		idle_size_++;
	}



	return Result(task);
}
#endif 
void ThreadPool::TestFunc(int id)
{
	//获取当前时间，确定线程空闲了多久
	auto lastTime = std::chrono::high_resolution_clock().now();
	//有了id就能删线程了
	//执行完所有任务再考虑删除 版本
	for (;;)
	{
		Task task;
		{
			std::unique_lock<std::mutex> lock(mtx);
			//只要任务为空，就得继续往下执行
			//cached模式时
			while (task_.empty())
			{
				if (!isPoolRuning)
				{
					//都已经结束线程了，无所谓了，直接返回吧
					threads_.erase(id);
					cv_closePool.notify_all();
					std::cout << "thread" << std::this_thread::get_id() << " exit" << std::endl;
					return;
				}
				if (poolMode_ == PoolMode::MODE_CACHED)
				{
					//每一秒钟查看一下状态
					if (std::cv_status::timeout == cv_notEmpty.wait_for(lock, std::chrono::seconds(1)))
					{
						auto curTime = std::chrono::high_resolution_clock().now();
						if (thread_size_ > initThreadSize && (curTime - lastTime).count() > maxTime)
						{
							//回收线程
							threads_.erase(id);
							thread_size_--;
							idle_size_--;
							std::cout << ">>> exit new thread..." << std::endl;
							return;
						}
					}
				}
				else
				{
					cv_notEmpty.wait(lock);
				}
				//if (!isPoolRuning)
				//{
				//	cv_closePool.notify_all();
				//	//都已经结束线程了，无所谓了，直接返回吧
				//	threads_.erase(id);
				//	std::cout << "thread"<< std::this_thread::get_id() <<" exit" << std::endl;
				//	return;
				//}
	
			}

			task = task_.front();
			task_.pop();
			task_size_--;
			idle_size_--;
			if (task_.size() > 0)
			{
				cv_notEmpty.notify_all();
			}
			cv_notFull.notify_all();
		}
	
		if(task!=nullptr)
			task();
		idle_size_++;
		//这个要记得，一旦执行完任务之后刷新一下线程的休息时间
		lastTime = std::chrono::high_resolution_clock().now(); // 更新线程执行完任务的时间
	}
	//当线程池不再运行时，就直接退出
		//都已经结束线程了，无所谓了，直接返回吧
	threads_.erase(id);
	std::cout << "thread" << std::this_thread::get_id() << " exit" << std::endl;
	cv_closePool.notify_all();
	return;

}
void  ThreadPool::setTaskSizeTh(int size)
{
	if (!isPoolRuning)
		TaskTh = size;
}
void  ThreadPool::setThreadSizeTh(int size)
{
	if(!isPoolRuning)
		ThreadSizeTh = size;
}
void  ThreadPool::setPoolMode(PoolMode poolMode)
{
	if (!isPoolRuning)
		poolMode_ = poolMode;
}





////////////////////////////////////////////task
void Task::setResult(Result* r)
{
	r_ = r;
}
 void Task::exec()
{
	 if(r_!=nullptr)
		r_->setVal(run());
}



/////////////////////////////////////////////Result
 Result::Result(std::shared_ptr<Task> t, bool isValid) :t_(t), isValid_(isValid) {
	 //将task的指针指向如今的
	 t_->setResult(this);
}

 Result::~Result()
 {}
void Result::setVal(Any any)
{
	ret_ = std::move(any);
	sem_.post();
}

Any Result::getVal()
{
	if (isValid_ == false)
	{
		return nullptr;
	}
	sem_.wait();
	return std::move(ret_);
}


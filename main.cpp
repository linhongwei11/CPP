#include"thread_pool.h"
#include<future>
using namespace std;
using uL = unsigned long long;



class MyTask{
public:
	MyTask(int begin=0,int end=0):begin_(begin),
								  end_(end)
	{}
	int run()
	{
		std::cout << "current thread id is" << std::this_thread::get_id() <<"��ȡ����"<< std::endl;

		uL res = 0;
		for (int i = begin_; i <=end_; ++i)
		{
			res += i;
		}
		std::cout << "tid:" << std::this_thread::get_id()
			<< "end!" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		return res;
	}
private:
	int begin_;
	int end_;

};


int sum1(int a, int b)
{
	this_thread::sleep_for(chrono::seconds(2));
	// �ȽϺ�ʱ
	return a + b;
}





int main()
{
	

	{ThreadPool pool;
	//pool.setPoolMode(PoolMode::MODE_CACHED);
	pool.start(4);
	MyTask m = MyTask(1, 100);
	future<int> res = pool.submmitTask(std::bind(&MyTask::run, &m));
	pool.submmitTask(std::bind(&MyTask::run, &m));
	pool.submmitTask(std::bind(&MyTask::run, &m));
	pool.submmitTask(std::bind(&MyTask::run, &m));
	pool.submmitTask(std::bind(&MyTask::run, &m));
	pool.submmitTask(std::bind(&MyTask::run, &m));
	pool.submmitTask(std::bind(&MyTask::run, &m));
	pool.submmitTask(std::bind(&MyTask::run, &m));
	pool.submmitTask(std::bind(&MyTask::run, &m));
	cout << res.get() << endl;
	std::this_thread::sleep_for(std::chrono::seconds(5));
	}
		cin.get();
//future<int> res=







#if 0
	//Ҳ��һ��unique�ı������õ�ʱ����ҪС�ĵģ�����þ��ã����ú������þ�û�ˣ���ֵ�ĳ�ʼ���Լ�ʹ��ȫû��
	packaged_task<int(int, int)> task(sum1);
	future<int> res = task.get_future();
	thread t1(move(task), 10, 20);
	t1.detach();
	//// future <=> Result
	cout << res.get() << endl;


	//{
	//	ThreadPool pool;
	//	//pool.setPoolMode(PoolMode::MODE_CACHED);
	//	pool.start(4);
	//	Result r1 = pool.submmitTask(std::make_shared<MyTask>(1, 1000000));
	//}
	std::cout << "main over" << std::endl;
	////�����uL���͸���������Ҫԭ����Ϊ��Derive������Ҫ
	//uL sum1=r1.getVal().cast<uL>()+r2.getVal().cast<uL>()+r3.getVal().cast<uL>() +r4.getVal().cast<uL>() +r5.getVal().cast<uL>() +r6.getVal().cast<uL>();
	//uL sum2=0;


	//for (int i = 0; i <=6000000; ++i)
	//{
	//	sum2 += i;
	//}
	//std::cout << sum1 << std::endl;
	//std::cout << sum2 << std::endl;
#endif

	//std::this_thread::sleep_for(std::chrono::seconds(55));
	std::cin.get();
}
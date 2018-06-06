#ifdef TESTING
#include "Tests.hh"
#include <Utils/queue>
#include <algorithm>
#include <vector>


template <class T, void(*Func)(typename std::vector<T>::iterator, typename std::vector<T>::iterator)>
void test_heap();


TEST(queue)
{
	using namespace Utils;
	//test_heap(std::make_heap<std::vector<int>::iterator>);
	test_heap<int, &std::make_heap>();
	test_heap<int, Utils::make_heap>();
	
	
	Utils::priority_queue<int> q;
	q.push(0);
	q.push(2);
	q.push(1);
	assert(q.top() == 2);
	q.pop();
	assert(q.top() == 1);
	q.pop();
	assert(q.top() == 0);
	q.pop();
	assert(q.empty());
}



template <class T, void(*fn)(typename std::vector<T>::iterator, typename std::vector<T>::iterator)>
void test_heap()
{
	const typename std::vector<T>::size_type n = 2000;
	std::vector<T> v;
	v.reserve(n);
	for (T i = 0; i < n; ++i)
	{
		v.push_back(i);
	}
	
	std::cout << "Starting" << std::endl;
	fn(v.begin(), v.end());
	std::cout << "Done." << std::endl;
	if (n < 30)
	{
		for (T i = 0; i < n; ++i)
		{
			std::cout << v[i] << "\n";
			if (i > 0)
			{
				assert(!(v[(i-1)/2] < v[i]));
			}
		}
		std::cout << std::endl;
	}
	else
	{
		for (T i = 0; i < n; ++i)
		{
			if (i > 0)
			{
				if (v[(i-1)/2] < v[i])
				{
					std::cout << v[i] << " - " << v[i-1] << std::endl;
				}
				assert(!(v[(i-1)/2] < v[i]));
			}
		}
	}
}
#endif
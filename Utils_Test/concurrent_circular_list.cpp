#include "Tests.hh"
#include <Support/collections/concurrent_circular_list.hh>
#include <vector>
#include <future>

template <class ...T>
using cclist = Support::Collections::concurrent_circular_list<T...>;

TEST(concurrent_circular_list)
{
	{
		cclist<int> l1;
		
		l1.push_back(4);
		assert(l1.size() == 1);
		
		int it_count = 0;
		
		for (auto& x : l1)
		{
			++it_count;
			if (it_count == 2)
			{
				break;
			}
		}
		assert(it_count == 2);
		
		
		l1.push_back(5);
		assert(l1.size() == 2);
		
		it_count = 0;
		
		for (auto& x : l1)
		{
			switch (it_count % 2)
			{
				case 0:
				assert(x == 4);
				break;
				
				case 1:
				assert(x == 5);
				break;
				
				default:
				assert(false);
			}
			assert((it_count % 2) + 4 == x);
			++it_count;
			if (it_count == 30)
			{
				break;
			}
		}
		assert(it_count == 30);
		
		
		{
			auto n1 = l1.get_n(l1.begin());
			assert(n1);
			auto n2 = n1->next;
			assert(n2);
			assert(n2 != n1);
			assert(n2->next == n1);
			assert(n2->prev == n1);
			assert(n1->prev);
		}
		
		assert(*l1.begin() == 4);
		l1.erase(l1.begin());
		assert(l1.size() == 1);
		assert(*l1.begin() == 5);
		it_count = 0;
		
		{
			auto n1 = l1.get_n(l1.begin());
			assert(n1);
			auto n2 = n1->next;
			assert(n2);
			assert(n2 == n1);
			assert(n1->next == n1);
			assert(n1->prev == n1);
			assert(n1->prev);
			assert(n1->next);
		}
		
		for (auto& x : l1)
		{
			ASSERTEQ(x, 5);
			++it_count;
			if (it_count == 2)
			{
				break;
			}
		}
		assert(it_count == 2);
		
		
		
		
		
		l1.push_back(6);
		assert(l1.size() == 2);
		
		it_count = 0;
		
		for (auto& x : l1)
		{
			assert((it_count % 2) + 5 == x);
			++it_count;
			if (it_count == 30)
			{
				break;
			}
		}
		assert(it_count == 30);
		
		l1.clear();
		
		l1.push_back(4);
		assert(l1.size() == 1);
		
		it_count = 0;
		
		for (auto& x : l1)
		{
			++it_count;
			if (it_count == 2)
			{
				break;
			}
		}
		assert(it_count == 2);
	}
	
	
	{
		cclist<int> li;
		
		li.push_back(-1);
		
		int it_count = 0;
		
		std::vector<std::future<void>> futs;
		for (int i = 0; i < 30; ++i)
		{
			futs.push_back(std::async(std::launch::async, [i, &li]()
			{
				li.push_back(i);
			}));
		}
		
		assert(futs.size() == 30);
		for (int i = 0; i < 30; ++i)
		{
			futs[i].get();
		}
		
		assert(li.size() == 31);
		
		for (auto& x : li)
		{
			if (x == -1 && it_count > 0)
			{
				break;
			}
			std::cout << x << std::endl;
			++it_count;
		}
		
		ASSERTEQ(it_count, 31);
	}
	
	
	{
		cclist<int> li;
		
		li.push_back(-1);
		
		int it_count = 0;
		
		std::vector<std::future<void>> futs;
		for (int i = 0; i < 30; ++i)
		{
			futs.push_back(std::async(std::launch::async, [i, &li]()
			{
				li.push_back(i);
				auto it = li.begin();
				while (*it != i)
				{
					++it;
				}
				ASSERTEQ(*it, i);
				li.erase(it);
				std::cout << "Erased " << i << std::endl;
			}));
		}
		
		assert(futs.size() == 30);
		for (int i = 0; i < 30; ++i)
		{
			futs[i].get();
		}
		
		assert(li.size() == 1);
		
		for (auto& x : li)
		{
			ASSERTEQ(x, -1);
			break;
		}
	}
}
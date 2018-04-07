#ifndef INCLUDED_CONDITION_VARIABLE_HH
#define INCLUDED_CONDITION_VARIABLE_HH

#include <Common.h>

namespace Utils
{
	namespace chrono
	{
		template <class, class>
		class time_point;
	}
	
	class mutex;
	template <class>
	class unique_lock;
	
	enum class cv_status
	{
		timeout,
		no_timeout
	};
	
	class condition_variable
	{
		//pthread_cond_t __cv_;
		
		public:
		constexpr condition_variable();
		~condition_variable();
		
		void notify_one() throw();
		void notify_all() throw();
		
		void wait(unique_lock<mutex>&) throw();
		
		template <class Clock, class Dur>
		cv_status wait_until(unique_lock<mutex>&, const chrono::time_point<Clock, Dur>&);
		
	};
}

#endif
#ifndef INCLUDED_CONDITION_VARIABLE_HH
#define INCLUDED_CONDITION_VARIABLE_HH

#include <Common.h>
#include <Support/threading/LowLock.hh>

namespace Utils
{
	#define NCOND_SHIFT 1
	
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
		public:
		typedef mutex mutex_type;
		
		private:
		int_lock_t __lock;
		unsigned int __futex;
		unsigned long long int __total_seq;
		unsigned long long int __wakeup_seq;
		unsigned long long int __woken_seq;
		mutex_type* __mutex;
		unsigned int __nwaiters;
		unsigned int __broadcast_seq;
		
		
		public:
		condition_variable();
		condition_variable(const condition_variable&) = delete;
		
		~condition_variable();
		
		void notify_one() throw();
		void notify_all() throw();
		
		void wait(unique_lock<mutex_type>&) throw();
		
		template <class Clock, class Dur>
		cv_status wait_until(unique_lock<mutex_type>&, const chrono::time_point<Clock, Dur>&);
		
	};
	
	
	template <class M>
	class generic_condition_variable
	{
		public:
		typedef M mutex_type;
		
		private:
		int_lock_t __lock;
		unsigned int __futex;
		unsigned long long int __total_seq;
		unsigned long long int __wakeup_seq;
		unsigned long long int __woken_seq;
		mutex_type* __mutex;
		unsigned int __nwaiters;
		unsigned int __broadcast_seq;
		
		
		public:
		generic_condition_variable();
		generic_condition_variable(const condition_variable&) = delete;
		
		~generic_condition_variable();
		
		void notify_one() throw();
		void notify_all() throw();
		
		void wait(unique_lock<mutex_type>&) throw();
		
		template <class Clock, class Dur>
		cv_status wait_until(unique_lock<mutex_type>&, const chrono::time_point<Clock, Dur>&);
		
	};
}

#endif
#ifndef INCLUDED_PTHREAD_PTHREAD_HH
#define INCLUDED_PTHREAD_PTHREAD_HH

#include <Common.h>
// #include <pthread_types.h>
// #include <Utils/id_queue>
// #include <Utils/mutex>
// #include <Utils/map>


// class pThread
// {
//     private:

//     struct Data
//     {
//         void* data;
//     };

//     public:
//     typedef pthread_key_t key_type;
//     typedef pthread_t thread_type;
//     typedef pthread_mutex_t mutex_type;
//     typedef pthread_cond_t cond_type;
//     typedef pthread_once_t once_type;
//     typedef Data data_type;


//     private:
//     typedef void(*key_destr_type)(void*);


//     Utils::id_queue<key_type> keys;
//     Utils::mutex keys_m;
//     Utils::map<key_type, key_destr_type> destructors;
//     Utils::map<key_type, data_type*> keyData;
//     Utils::mutex keyData_m;


//     static pThread* Instance() noexcept;

//     public:

//     key_type getKey(key_destr_type = nullptr) noexcept;
//     data_type* getData(const key_type&) noexcept;


//     static key_type GetKey(key_destr_type = nullptr) noexcept;
//     static data_type* GetData(const key_type&) noexcept;

// };

extern "C" {
	int sched_yield();
}


template <class T>
static inline void __attribute__((__always_inline__)) lll_lock(T& val)//, T expect, T desire)
{
	/*T tmp = expect;
	while (!__atomic_compare_exchange_n(&val, &tmp, desire, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
	{
		sched_yield();
		tmp = expect;
	}*/
	
	T tmp = 0;
	
	while (!__atomic_compare_exchange_n(&val, &tmp, 1, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
	{
		sched_yield();
		tmp = 0;
	}
}

template <class T>
static inline void __attribute__((__always_inline__)) lll_unlock(T& val)
{
	__atomic_exchange_n(&val, 0, __ATOMIC_RELEASE);
}


#endif
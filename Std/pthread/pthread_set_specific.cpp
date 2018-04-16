// #include <pthread.h>
// #include "pthread.hh"

// C_CODE

// int pthread_setspecific(pthread_key_t key, const void* nData)
// {
//     TRACE("Called");
// 	auto data = pThread::GetData(key);
//     assert(data == pThread::GetData(key));
//     data->data = const_cast<void*>(nData);
//     //assert(nData == pthread_getspecific(key));
//     return 0;
// }

// C_END
// #include <pthread.h>
// #include <Utils/mutex>

// C_CODE

// static Utils::mutex once_lock;

// int pthread_once(pthread_once_t* once_control, void (*init_routine)(void))
// {
//     if (*once_control == PTHREAD_ONCE_INIT)
//     {
//         Utils::unique_lock<Utils::mutex> lock(once_lock);
//         //lll_lock(once_lock, LLL_PRIVATE);

//         if (*once_control == PTHREAD_ONCE_INIT)
//         {
//             init_routine();

//             *once_control = !PTHREAD_ONCE_INIT;
//         }

//         //lll_unlock(once_lock, LLL_PRIVATE);
//     }
    
//     return 0;
// }

// C_END
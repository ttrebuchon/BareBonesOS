
#ifdef TESTING

#define pthread_cond_t osthread_cond_t
#define pthread_mutex_t osthread_mutex_t2
#define pthread_cond_wait osthread_cond_wait
#define pthread_mutex_unlock osthread_mutex_unlock
#define pthread_mutex_lock osthread_mutex_lock


#endif
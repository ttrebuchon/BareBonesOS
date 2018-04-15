#ifndef INCLUDED_PTHREAD_HH
#define INCLUDED_PTHREAD_HH

#include <Common.h>
#include <pthread_types.h>
#include <Utils/id_queue>
#include <Utils/mutex>
#include <Utils/map>


class pThread
{
    public:
    typedef pthread_key_t key_type;
    typedef pthread_t thread_type;
    typedef pthread_mutex_t mutex_type;
    typedef pthread_cond_t cond_type;
    typedef pthread_once_t once_type;


    private:
    typedef void(*key_destr_type)(void*);


    Utils::id_queue<key_type> keys;
    Utils::mutex keys_m;
    Utils::map<key_type, key_destr_type> destructors;



    static pThread* instance;

    public:

    key_type getKey(key_destr_type = nullptr) noexcept;


    static key_type GetKey(key_destr_type = nullptr) noexcept;

};


#endif
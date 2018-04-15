#include "pthread.hh"



pThread* pThread::instance = nullptr;





auto pThread::getKey(key_destr_type destr) noexcept -> key_type
{
    key_type key;
    {
        Utils::unique_lock<Utils::mutex> lock(keys_m);
        key = keys.next();
        if (destr)
        {
            destructors[key] = destr;
        }
    }
    return key;
}












auto pThread::GetKey(key_destr_type destr) noexcept -> key_type
{
    if (!instance)
    {
        instance = new pThread;
    }

    return instance->getKey(destr);
}
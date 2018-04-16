// #include "pthread.hh"
// #include <kernel/Task.hh>




// auto pThread::getKey(key_destr_type destr) noexcept -> key_type
// {
//     key_type key;
//     {
//         Utils::unique_lock<Utils::mutex> lock(keys_m);
//         key = keys.next();
//         if (destr)
//         {
//             destructors[key] = destr;
//         }
//     }
//     return key;
// }

// auto pThread::getData(const key_type& key) noexcept -> data_type*
// {
//     data_type* data = nullptr;
//     {
//         Utils::unique_lock<Utils::mutex> lock(keyData_m);
//         data = keyData[key];
//         if (!data)
//         {
//             data = keyData[key] = new Data;
//         }
//     }
//     return data;
// }







// pThread* pThread::Instance() noexcept
// {
//     if (Kernel::Task::current_task)
//     {
//         if (!Kernel::Task::current_task->pthread)
//         {
//             Kernel::Interrupts::cli();
//             if (Kernel::Task::current_task->pthread)
//             {
//                 Kernel::Interrupts::sti();
//                 return Instance();
//             }

//             TRACE("MARK.");
//             Kernel::Task::current_task->pthread = new pThread;

//             Kernel::Interrupts::sti();
//         }
//         return Kernel::Task::current_task->pthread;
//     }
//     assert(false);
//     return nullptr;
// }


// auto pThread::GetKey(key_destr_type destr) noexcept -> key_type
// {
//     auto inst = Instance();
//     assert(inst);
//     return inst->getKey(destr);
// }

// auto pThread::GetData(const key_type& key) noexcept -> data_type*
// {
//     auto inst = Instance();
//     assert(inst);
//     return inst->getData(key);
// }
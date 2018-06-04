#include <Common.h>
#include <signal.h>
#include <iostream>
#include "../TestUtils/QA.hh"

extern "C"
{
	
	static bool initialized = false;
	static sigset_t set;
	
	static sigset_t oldset;
	static sigset_t usr1set;
	
	static void init();
	
	static void(*init_fn)() = &init;
	
	static int counter = 0;
	
	static void(*sa_handler_fn)(int) = nullptr;
	
	static void dumb_fn() {}
	
	struct sigaction no_action;
	struct sigaction last_action;
	
	static void init()
	{
		if (__atomic_exchange_n(&initialized, true, __ATOMIC_ACQ_REL))
		{
			volatile auto fn_ptr = init_fn;
			while (fn_ptr)
			{
				__sync_synchronize();
				fn_ptr = init_fn;
			}
			return;
		}
		
		/*sigfillset(&set);
		sigset_t tmp;
		sigaddset(&tmp, SIGUSR1);
		sigprocmask(SIG_UNBLOCK, &tmp, nullptr);
		
		sigprocmask(SIG_SETMASK, nullptr, &oldset);
		
		assert(sigismember(&oldset, SIGUSR1) == 0);*/
		
		
		
		
		/*sigemptyset(&usr1set);
		sigaddset(&usr1set, SIGUSR1);
		
		sigprocmask(SIG_UNBLOCK, nullptr, &oldset);*/
		
		
		
		if (sigaction(SIGUSR1, nullptr, &no_action) != 0)
		{
			assert(false);
		}
		
		no_action.sa_handler = SIG_IGN;
		
		if (sigaction(SIGUSR1, nullptr, &last_action) != 0)
		{
			assert(false);
		}
		
		last_action.sa_handler = SIG_IGN;
		
		
		init_fn = nullptr;
	}
	
	
	
	
	
	void __testing_signal_cli()
	{
		if (init_fn)
		{
			init_fn();
		}
		
		if (last_action.sa_handler == SIG_IGN)
		{
			//std::clog << "CLI\n";
			if (sigaction(SIGUSR1, &no_action, &last_action) != 0)
			{
				assert(false);
			}
		}
		
		
		/*auto oldHndl = signal(SIGUSR1, SIG_IGN);
		
		assert(oldHndl != SIG_ERR);
		if (oldHndl != SIG_IGN)
		{
			sa_handler_fn = oldHndl;
		}*/
		
		//std::clog << "CLI\n";
	}
	
	
	
	void __testing_signal_sti()
	{
		if (init_fn)
		{
			init_fn();
		}
		
		//std::clog << "STI\n";
		
		
		
		if (last_action.sa_handler != SIG_IGN)
		{
			if (sigaction(SIGUSR1, &last_action, nullptr) != 0)
			{
				assert(false);
			}
			last_action.sa_handler = SIG_IGN;
		}
		
		
		
		/*if (handler_fn != nullptr)
		{
			auto ign_hndl = signal(SIGUSR1, handler_fn);
			assert(ign_hndl != SIG_ERR);
			handler_fn = nullptr;
		}
		
		return;*/
		
		
		
		/*auto oldAct = signal(SIGUSR1, SIG_IGN);
		
		sigprocmask(SIG_UNBLOCK, &usr1set, nullptr);
		signal(SIGUSR1, oldAct);*/
		//assert(sigismember(&oldset, SIGUSR1) == 0);
	}
	
	static sigset_t running_set;
	
	bool signals_running()
	{
		sigprocmask(SIG_UNBLOCK, nullptr, &running_set);
		return sigismember(&running_set, SIGUSR1) == 0;
	}
	
}
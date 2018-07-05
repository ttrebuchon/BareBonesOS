#include "Tests.hh"
#include <Std/pthread/osthread.h>


osthread_mutex_t* glob_mut = nullptr;
osthread_mutex_t glob_mut2 = OSTHREAD_MUTEX_INITIALIZER;

TEST(osthread)
{
	glob_mut = (osthread_mutex_t*)malloc(sizeof(osthread_mutex_t));
	memset(glob_mut, 0, sizeof(osthread_mutex_t));
	int err = osthread_mutex_init(glob_mut, nullptr);
	ASSERTEQ(err, 0);
	
	err = osthread_mutex_lock(glob_mut);
	ASSERTEQ(err, 0);
	err = osthread_mutex_unlock(glob_mut);
	ASSERTEQ(err, 0);
	err = osthread_mutex_destroy(glob_mut);
	ASSERTEQ(err, 0);
}
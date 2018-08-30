#include <Std/errno.h>
#include <Support/threading/LowLock.hh>


extern "C"
{
	
	const char* ___errs[EMAX_ERR];
	static int ___errs_locks[EMAX_ERR];
	
	const char* kstrerror(int err)
	{
		if (err <= 0 || err >= EMAX_ERR || !___errs[err])
		{
			return nullptr;
		}
		return ___errs[err];
	}
	
	int kstrerror_r(int errnum, char* buf, size_t buf_len)
	{
		using namespace Support::threading;
		
		if (errnum <= 0 || errnum >= EMAX_ERR || !buf_len || !___errs[errnum])
		{
			return -1;
		}
		
		low_lock(___errs_locks[errnum], false);
		
		const char* str = ___errs[errnum];
		
		auto len = strlen(str);
		if (len <= 0)
		{
			goto err;
		}
		
		--buf_len;
		
		if (len < buf_len)
		{
			buf_len = len;
		}
		
		memcpy(buf, str, buf_len);
		buf[buf_len] = '\0';
		
		low_unlock(___errs_locks[errnum]);
		return buf_len;
		
		err:
		
		low_unlock(___errs_locks[errnum]);
		return -1;
	}
	
	
	int set_error(int eno, const char* msg)
	{
		using namespace Support::threading;
		
		#ifdef DEBUG
		if (unlikely(eno >= EMAX_ERR || eno <= 0))
		{
			TRACE_VAL(eno);
		}
		#endif
		assert(eno < EMAX_ERR);
		assert(eno > 0);
		if (eno <= 0 || eno >= EMAX_ERR)
		{
			return -1;
		}
		
		low_lock(___errs_locks[eno], false);
		
		___errs[eno] = msg;
		kerrno = eno;
		#ifdef TESTING
		errno = eno;
		#endif
		
		low_unlock(___errs_locks[eno]);
		
		return 0;
		
		
		fail:
		
		low_unlock(___errs_locks[eno]);
		return -1;
	}
	
	
	
	
	
	#ifndef TESTING
	
	const char* strerror(int err)
	{
		return kstrerror(err);
	}
	
	int strerror_r(int errnum, char* buf, size_t buf_len)
	{
		return kstrerror_r(errnum, buf, buf_len);
	}
	
	#endif
	
	
}
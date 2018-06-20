#ifndef INCLUDED_BITS_FPOS_HH
#define INCLUDED_BITS_FPOS_HH

#include <Utils/Types.hh>

namespace Utils
{
	template <class StateT>
	class fpos
	{
		private:
		StateT _state;
		streamoff _off;
		
		
		public:
		fpos(streamoff off = streamoff()) : _state(), _off(off)
		{
			
		}
		
		StateT state() const
		{ return _state; }
		void state(StateT state)
		{ _state = state; }
		
		
		operator streamoff() const noexcept
		{
			return _off;
		}
		
		fpos& operator+=(streamoff rhs) noexcept
		{
			_off += rhs;
			return *this;
		}
		
		fpos operator+(streamoff rhs) const
		{
			fpos lhs(*this);
			lhs += rhs;
			return lhs;
		}
		
		fpos& operator-=(streamoff rhs) noexcept
		{
			_off -= rhs;
			return *this;
		}
		
		fpos operator-(streamoff rhs) const
		{
			fpos lhs(*this);
			lhs -= rhs;
			return lhs;
		}
		
	};
	
	
	
	template <class stateT>
	streamoff operator-(const fpos<stateT>& lhs, const fpos<stateT>& rhs) noexcept
	{
		return streamoff(lhs) - streamoff(rhs);
	}
	
	
	template <class stateT>
	bool operator==(const fpos<stateT>& lhs, const fpos<stateT>& rhs) noexcept
	{
		return streamoff(lhs) == streamoff(rhs);
	}
	
	template <class stateT>
	bool operator!=(const fpos<stateT>& lhs, const fpos<stateT>& rhs) noexcept
	{
		return streamoff(lhs) != streamoff(rhs);
	}
	
	
	typedef fpos<mbstate_t> streampos;
	typedef fpos<mbstate_t> wstreampos;
}

#endif
#ifndef INCLUDED_SUPPORT_SHARABLE_HH
#define INCLUDED_SUPPORT_SHARABLE_HH

#include <Common.h>

namespace Support
{
	template <class T>
	class sharable
	{
		public:
		typedef T* data_type;
		typedef const T* const_data_type;
		typedef long size_type;
		typedef data_type& reference;
		typedef const_data_type& const_reference;
		
		typedef struct
		{
			size_type users;
			data_type data;
		}
		ctrl_t;
		
		private:
		ctrl_t* control;
		
		void new_control()
		{
			if (control)
			{
				release_control();
			}
			
			control = new ctrl_t;
			control->users = 1;
		}
		
		void release_control()
		{
			if (!control)
			{
				return;
			}
			
			if (control->users > 1)
			{
				--control->users;
			}
			else
			{
				if (control->data)
				{
					delete control->data;
				}
				
				delete control;
			}
			
			control = nullptr;
		}
		
		bool modify()
		{
			if (control->users > 1)
			{
				auto ctrl2 = new ctrl_t;
				ctrl2->users = 1;
				auto next = copy(control->data);
				--control->users;
				ctrl2->data = next;
				control = ctrl2;
				return true;
			}
			else
			{
				return false;
			}
		}
		
		protected:
		
		const_data_type cdata() const noexcept
		{
			return control->data;
		}
		
		const_data_type data() const noexcept
		{
			return cdata();
		}
		
		data_type data() noexcept
		{
			modify();
			return control->data;
		}
		
		void data(data_type ndata) noexcept
		{
			if (!control)
			{
				control = new ctrl_t;
				control->users = 1;
				control->data = ndata;
			}
			else if (control->users > 1)
			{
				auto ctrl2 = new ctrl_t;
				ctrl2->users = 1;
				--control->users;
				ctrl2->data = ndata;
				control = ctrl2;
			}
			else
			{
				if (control->data)
				{
					delete control->data;
				}
				control->data = ndata;
			}
		}
		
		virtual data_type copy(data_type) noexcept = 0;
		
		public:
		sharable(data_type data) noexcept : control(nullptr)
		{
			new_control();
			control->data = data;
		}
		
		sharable(const sharable<T>& rhs) noexcept
		{
			control = rhs.control;
			if (control)
			{
				++control->users;
			}
			else
			{
				new_control();
			}
		}
		
		sharable(sharable<T>&& rhs) noexcept
		{
			control = rhs.control;
			rhs.control = nullptr;
		}
		
		
		~sharable()
		{
			release_control();
		}
		
		
		
		sharable& operator=(const sharable& rhs) noexcept
		{
			if (&rhs != this)
			{
				release_control();
				
				if (rhs.control)
				{
					++rhs.control->users;
					control = rhs.control;
				}
				else
				{
					new_control();
				}
			}
			return *this;
		}
		
		sharable& operator=(sharable&& rhs) noexcept
		{
			release_control();
			
			if (rhs.control)
			{
				control = rhs.control;
				rhs.control = nullptr;
			}
			else
			{
				new_control();
			}
			
			return *this;
		}
		
	};
}

#endif
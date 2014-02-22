#ifndef PBATTAG1_SPTR_HPP
#define PBATTAG1_SPTR_HPP

#include <stdlib.h>
#include <iostream>
#include <atomic>
#include <typeinfo>
#include <pthread.h>

namespace cs540 
{

	class RefBase
	{
		public:
			RefBase() { };
			virtual ~RefBase() { };
	};

	template <typename S>
	class RefCounter : public RefBase
	{
		public:
			S* original;
			RefCounter(S* ptr) : original(ptr) { }
			~RefCounter() 
			{
				delete original;
				original = NULL;
			}
	};

	template <typename T> 
	class Sptr
	{
		public:
			T *data_member;
			RefBase* ref_obj;
			std::atomic<u_int>* ref_counter;

			Sptr() : ref_obj(NULL), data_member(NULL), ref_counter(NULL) 
			{ 
			}

			template<typename U> 
			Sptr(U *item)
			{
				ref_obj = new RefCounter<U>(item);
				data_member = item;
				ref_counter = new std::atomic<u_int>();
				++(*ref_counter);
			}

			Sptr(const Sptr &item) 
			{
				ref_obj = item.ref_obj;
				data_member = static_cast<T*>(item.get());
				ref_counter = item.ref_counter;
				
				if(ref_counter != NULL)
					++(*ref_counter);
			}

			template <typename U> 
			Sptr(const Sptr<U> &item)
			{
				data_member = static_cast<T*>(item.get());
				ref_obj = (RefCounter<T>*)item.ref_obj;
				ref_counter = item.ref_counter;
				
				if(ref_counter != NULL)
					++(*ref_counter);
			}

			Sptr &operator=(const Sptr &item)
			{
				if(this == &item) return *this;
				
				if(ref_counter != NULL && --(*ref_counter) == 0)
				{
					delete ref_obj;
					delete ref_counter;
					ref_counter = NULL;
					ref_obj = NULL;
				}

				ref_counter = item.ref_counter;
				if(ref_counter != NULL)
					++(*ref_counter); // New reference, regardless.
				
				ref_obj = item.ref_obj;
				data_member = item.get();
				
				return *this;	
			}

			template <typename U> Sptr<T> &operator=(const Sptr<U> &item)
			{
				if(this == (Sptr<T>*)&item) return *this;
				
				if(ref_counter != NULL && (--(*ref_counter) == 0))
				{
					delete ref_obj;
					delete ref_counter;
					ref_counter = NULL;
					ref_obj = NULL;
					//data_member = NULL;
				}
				
				ref_counter = item.ref_counter;
				
				if(ref_counter != NULL)
					++(*ref_counter);
				
				ref_obj = (RefCounter<T>*)item.ref_obj;
				data_member = item.get();
				
				return *this;	
			}

			void reset()
			{
				if(ref_obj != NULL)
				{
					
					if(--(*ref_counter) == 0)
					{
						delete ref_counter;
						delete ref_obj;
					}
					
					ref_obj = NULL;
					data_member = NULL;
					ref_counter = NULL;
				} 
			}

			T &operator*() const
			{
				return (*data_member);
			}

			T *operator->() const
			{
				return data_member;
			}

			T *get() const
			{
				return data_member;
			}

			explicit operator bool() const
			{
				return (data_member != nullptr);
			}

			virtual ~Sptr()
			{
				if(ref_counter != NULL && --(*ref_counter) == 0)
				{
					delete ref_obj;
					delete ref_counter;
					 ref_obj = NULL;
					 data_member = NULL;
				}
			}
	};

	template <typename T1, typename T2>
	bool operator==(const Sptr<T1> &a, const Sptr<T2> &b)
	{
		return (a.get() == b.get()) || (a.get() == NULL && b.get() == NULL);
	}

	template <typename T, typename U> 
	Sptr<T> static_pointer_cast(const Sptr<U> &sp)
	{
		Sptr<T> n_sp(sp);
		n_sp.data_member = static_cast<T*>(n_sp.data_member);
		return n_sp;
	}

	template <typename T, typename U> 
	Sptr<T> dynamic_pointer_cast(const Sptr<U> &sp)
	{
		Sptr<T> n_sp(sp);
		n_sp.data_member = dynamic_cast<T*>(sp.data_member);
		return n_sp;
	}
}
#endif
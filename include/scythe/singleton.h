#ifndef __SCYTHE_SINGLETON_H__
#define __SCYTHE_SINGLETON_H__

#include "non_copyable.h"
#include "types.h"

namespace scythe {

	/**
	 * @brief      Front-end to get access to the unique  object.
	 *
	 * @tparam     T     A class name.
	 */
	template <class T>
	class Singleton
	: public NonCopyable
	{
	public:
		static T * GetInstance()
		{
			static T instance;
			return &instance;
		}
	};

	/**
	 * @brief      Front-end to get access to the unique managed object.
	 *
	 * @tparam     Base     A base class name.
	 */
	template <class Base>
	class ManagedSingleton
	: public NonCopyable
	{
	public:
		template <class Derived>
		static void CreateInstance()
		{
			static_assert(std::is_convertible<Derived*, Base*>::value, "Inheritance issue");
			instance_ = new Derived();
		}
		static void DestroyInstance()
		{
			delete instance_;
			instance_ = nullptr;
		}
		static Base * GetInstance()
		{
			return instance_;
		}

	private:
		static Base * instance_;
	};
	template <class Base>
	Base* ManagedSingleton<Base>::instance_ = nullptr;

	/**
	 * @brief      Front-end to get access to the unique object with counting references.
	 *
	 * @tparam     T     A class name.
	 */
	template <class T>
	class CountingSingleton
	: public NonCopyable
	{
	public:
		static T * GetInstance()
		{
			if (instance_counter_ == 0)
				instance_ = new T();
			++instance_counter_;
			return instance_;
		}
		static void ReleaseInstance()
		{
			--instance_counter_;
			if (instance_counter_ == 0) {
				delete instance_;
				instance_ = nullptr;
			}
		}

	private:
		static T * instance_;
		static U32 instance_counter_;
	};
	template <class T>
	T* CountingSingleton<T>::instance_ = nullptr;
	template <class T>
	U32 CountingSingleton<T>::instance_counter_ = 0;

} // namespace scythe

#endif
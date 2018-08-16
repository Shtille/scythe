#ifndef __SCYTHE_SINGLETON_H__
#define __SCYTHE_SINGLETON_H__

#include "non_copyable.h"

namespace scythe {

	//! Simple singleton class
	template <class T>
	class Singleton : public NonCopyable {
	public:
		static T * GetInstance() {
			static T instance;
			return &instance;
		}
	};

	//! Singleton class with instance management
	template <class T>
	class ManagedSingleton : public NonCopyable {
	public:
		static void CreateInstance() {
			instance_ = new T();
		}
		static void DestroyInstance()
		{
			delete instance_;
			instance_ = nullptr;
		}
		static T * GetInstance() {
			return instance_;
		}

	private:
		static T * instance_;
	};
	template <class T>
	T* ManagedSingleton<T>::instance_ = nullptr;

	//! Singleton which counting references
	template <class T>
	class CountingSingleton : public NonCopyable {
	public:
		static T * GetInstance() {
			if (instance_counter_ == 0)
				instance_ = new T();
			++instance_counter_;
			return instance_;
		}
		static void ReleaseInstance() {
			--instance_counter_;
			if (instance_counter_ == 0) {
				delete instance_;
				instance_ = nullptr;
			}
		}

	private:
		static T * instance_;
		static int instance_counter_;
	};
	template <class T>
	T* CountingSingleton<T>::instance_ = nullptr;
	template <class T>
	int CountingSingleton<T>::instance_counter_ = 0;

} // namespace scythe

#endif
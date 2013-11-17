#pragma once

#include "bricks/threading/mutex.h"
#include "bricks/threading/mutexlock.h"

namespace Bricks { namespace Threading {
	template<typename T> class MonitorLock;

	template<typename T> class Monitor
	{
	protected:
		AutoPointer<Mutex> mutex;
		T* value;

	public:
		Monitor(T* value) : mutex(autonew Mutex(MutexType::RecursiveLock)), value(value) { }
		Monitor(const Pointer<T>& value) : mutex(autonew Mutex(MutexType::RecursiveLock)), value(value) { }

		Mutex* GetMutex() const { return mutex; }

		T* GetValue() const { return value; }
		MonitorLock<T> operator->() const { return MonitorLock<T>(this); }
	};

	template<typename T> class AutoMonitor : public Monitor<T>
	{
	public:
		AutoMonitor(T* value) : Monitor<T>(value) { AutoPointer<T>::Retain(this->value); }
		AutoMonitor(const Pointer<T>& value) : Monitor<T>(value) { AutoPointer<T>::Retain(this->value); }
		~AutoMonitor() { AutoPointer<T>::Release(this->value); }
	};

	template<typename T> class MonitorLock
	{
	protected:
		MutexLock lock;
		T* pointer;

	public:
		MonitorLock(const Monitor<T>* monitor) : lock(monitor->GetMutex()), pointer(monitor->GetValue()) { }
		T* operator->() const { return pointer; }
	};
} }

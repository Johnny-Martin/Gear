#pragma once
#include "../stdafx.h"

//===========================================线程锁===========================================//
class SingleThreadLock
{
public:
	void Lock() {};
	void UnLock() {};
};

class MultiThreadLock
{
public:
	MultiThreadLock() {
		::InitializeCriticalSection(&m_cs);
	}
	~MultiThreadLock() {
		::DeleteCriticalSection(&m_cs);
	}
	void Lock() {
		::EnterCriticalSection(&m_cs);
	};
	void UnLock() {
		::LeaveCriticalSection(&m_cs);
	};
private:
	CRITICAL_SECTION m_cs;
};

template<typename Lock>
class ThreadAutoLock
{
public:
	ThreadAutoLock(Lock& lock) :m_lock(lock){
		m_lock.Lock();
	}
	~ThreadAutoLock() {
		m_lock.UnLock();
	}
private:
	Lock& m_lock;
};
//============================================================================================//
//==========================================原子操作==========================================//

class SingleThreadAtomicOption
{
public:
	static long Inc(volatile long* data) {
		return ++(*data);
	}
	static long Dec(volatile long* data) {
		return --(*data);
	}
};
class MultiThreadAtomicOption
{
public:
	static long Inc(volatile long* data) {
		return ::InterlockedIncrement(data);
	}
	static long Dec(volatile long* data) {
		return ::InterlockedDecrement(data);
	}
};
//============================================================================================//

class SingleThreadModel
{
public:
	typedef SingleThreadAtomicOption			AtomicOp;
	typedef SingleThreadLock					ThreadLock;
	typedef ThreadAutoLock<SingleThreadLock>	AutoLock;
};

class MultiThreadModel
{
public:
	typedef MultiThreadAtomicOption				AtomicOp;
	typedef MultiThreadLock						ThreadLock;
	typedef ThreadAutoLock<MultiThreadLock>		AutoLock;
};
#ifndef LOCK_HHH
#define LOCK_HHH

#include <pthread.h>
// To do: only composing without inheritation
// maintain virtual table hurts performance and space.
class ILock
{
public:
	virtual ~ILock() {}

	virtual void Lock() const = 0;
	virtual void Unlock() const = 0;
};

class CMutex : public ILock
{
public:
	CMutex();
	~CMutex();

	virtual void Lock() const;
	virtual void Unlock() const;

private:
	mutable pthread_mutex_t _mutex;
};

class CLock
{
public:
	CLock(const ILock&);
	~CLock();

private:
	const ILock& _lock;
};

#endif  //Lock_HHH

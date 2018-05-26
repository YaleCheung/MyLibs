#include "lock.h"


CMutex::CMutex()
{
	pthread_mutex_init(&_mutex, NULL);
}

CMutex::~CMutex()
{
	pthread_mutex_destroy(&_mutex);
}

void CMutex::Lock() const
{
	pthread_mutex_lock(&_mutex);
}

void CMutex::Unlock() const
{
	pthread_mutex_unlock(&_mutex);
}

CLock::CLock(const ILock& m) : _lock(m)
{
	_lock.Lock();
}

CLock::~CLock()
{
	_lock.Unlock();
}
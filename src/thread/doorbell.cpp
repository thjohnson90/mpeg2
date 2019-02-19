#include <stdint.h>
#include <pthread.h>

#include "doorbell.h"

Doorbell::Doorbell() : _isLocked(true)
{
    _doorbellMutex = PTHREAD_MUTEX_INITIALIZER;
    _doorbellCond  = PTHREAD_COND_INITIALIZER;

    LockBell();
}

Doorbell::~Doorbell()
{
    if (_isLocked) {
	UnlockBell();
    }
}

int32_t Doorbell::LockBell(void)
{
    int32_t status = 0;

    do {
	pthread_mutex_lock(&_doorbellMutex);
	_isLocked = true;
    } while (0);

    return status;
}

int32_t Doorbell::UnlockBell(void)
{
    int32_t status = 0;

    do {
	pthread_mutex_unlock(&_doorbellMutex);
	_isLocked = false;
    } while (0);

    return status;
}

int32_t Doorbell::Ring(void)
{
    int32_t status = 0;

    do {
	LockBell();
	pthread_cond_signal(&_doorbellCond);
	UnlockBell();
    } while (0);

    return status;
}

int32_t Doorbell::Listen(void)
{
    int32_t status = 0;

    do {
	pthread_cond_wait(&_doorbellCond, &_doorbellMutex);
    } while (0);

    return status;
}


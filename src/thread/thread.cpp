#include <iostream>
#include <stdint.h>
#include <pthread.h>

#include "stream.h"
#include "picdata.h"
#include "doorbell.h"
#include "thread.h"
#include "thrdcmds.h"
#include "videoproc.h"
#include "thrdcmds.h"

using namespace std;

Thread::Thread() :
    _thrdId(0),
    _cmd(common_cmd::null),
    _workFunc(nullptr)
{
}

Thread::~Thread()
{
    Destroy();
}

int32_t Thread::Initialize(void* (*func)(void*), void* arg)
{
    int32_t status = 0;

    do {
	status = pthread_create(&_thrdId, 0, func, arg);
    } while (0);

    return status;
}

int32_t Thread::Destroy(void)
{
    int32_t status = 0;

    do {
    } while (0);

    return status;
}

int32_t Thread::Ring(uint32_t cmd)
{
    int32_t status = 0;

    do {
	_cmd = cmd;
	status = _bell.Ring();
    } while (0);

    return status;
}

int32_t Thread::Listen(void)
{
    int32_t status = 0;

    do {
	status = _bell.Listen();
    } while (0);

    return status;
}

int32_t Thread::Join(void** retval)
{
    int32_t status = 0;

    do {
	status = pthread_join(_thrdId, retval);
    } while (0);

    return status;
}

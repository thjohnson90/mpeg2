#include <iostream>
#include <stdint.h>
#include <pthread.h>

#include "doorbell.h"
#include "thread.h"

using namespace std;

Thread::Thread() :
    _thrdId(0),
    _cmd(parse_cmd_null),
    _workFunc(nullptr)
{
}

Thread::~Thread()
{
    Destroy();
}

uint32_t Thread::Initialize(void* (*func)(void*), void* arg)
{
    uint32_t status = 0;

    do {
	status = pthread_create(&_thrdId, 0, func, arg);
    } while (0);

    return status;
}

uint32_t Thread::Destroy(void)
{
    uint32_t status = 0;

    do {
    } while (0);

    return status;
}

uint32_t Thread::Ring(uint32_t cmd)
{
    uint32_t status = 0;

    do {
	_cmd = cmd;
	status = _bell.Ring();
    } while (0);

    return status;
}

uint32_t Thread::Listen(void)
{
    uint32_t status = 0;

    do {
	status = _bell.Listen();
    } while (0);

    return status;
}

uint32_t Thread::Join(void** retval)
{
    uint32_t status = 0;

    do {
	status = pthread_join(_thrdId, retval);
    } while (0);

    return status;
}

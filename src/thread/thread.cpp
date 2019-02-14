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

void Thread::DumpCommand(void)
{
    switch (_cmd) {
    case parse_cmd_null:
	cout << "Got null cmd..." << endl;
	break;
	
    case parse_cmd_data_ready:
	cout << "Got data ready cmd..." << endl;
	break;
	
    case parse_cmd_data_consumed:
	cout << "Got data consumed cmd..." << endl;
	break;
	
    case parse_cmd_seq_end_received:
	cout << "Got sequence end cmd..." << endl;
	break;
	
    case parse_cmd_exit:
	cout << "Got exit cmd..." << endl;
	break;

    default:
	cout << "Got invalid cmd..." << endl;
	break;
    }
}


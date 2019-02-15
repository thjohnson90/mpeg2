#ifndef __THREAD_H__
#define __THREAD_H__

class Thread
{
public:
    Thread();
    ~Thread();

    uint32_t Initialize(void* (*func)(void*), void* arg);
    uint32_t Destroy(void);
    uint32_t Ring(uint32_t cmd);
    uint32_t Listen(void);
    uint32_t GetCmd(void) {return _cmd;}
    uint32_t Join(void** retval);
    
    enum {
	parse_cmd_null,
	parse_cmd_data_ready,
	parse_cmd_data_consumed,
	parse_cmd_seq_end_received,
	parse_cmd_exit
    };
    
private:
    Doorbell  _bell;
    pthread_t _thrdId;
    uint32_t  _cmd;
    void* (*_workFunc)(void* arg);
};

#endif
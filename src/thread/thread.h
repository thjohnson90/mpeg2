#ifndef __THREAD_H__
#define __THREAD_H__

class Thread
{
public:
    Thread();
    ~Thread();

    int32_t Initialize(void* (*func)(void*), void* arg);
    int32_t Destroy(void);
    int32_t Ring(uint32_t cmd);
    int32_t Listen(void);
    int32_t GetCmd(void) {return _cmd;}
    int32_t Join(void** retval);
    
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

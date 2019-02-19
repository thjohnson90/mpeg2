#ifndef __DOORBELL_H__
#define __DOORBELL_H__

class Doorbell
{
public:
    Doorbell();
    ~Doorbell();

    int32_t LockBell(void);
    int32_t UnlockBell(void);
    int32_t Ring(void);
    int32_t Listen(void);
    
private:
    bool              _isLocked;
    pthread_mutex_t   _doorbellMutex;
    pthread_cond_t    _doorbellCond;
};

#endif

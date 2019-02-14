#ifndef __DOORBELL_H__
#define __DOORBELL_H__

class Doorbell
{
public:
    Doorbell();
    ~Doorbell();

    uint32_t LockBell(void);
    uint32_t UnlockBell(void);
    uint32_t Ring(void);
    uint32_t Listen(void);
    
private:
    bool              _isLocked;
    pthread_mutex_t   _doorbellMutex;
    pthread_cond_t    _doorbellCond;
};

#endif

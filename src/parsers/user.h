#ifndef __USER_DATA_H__
#define __USER_DATA_H__

class UserDataParser
{
public:
    UserDataParser(BitBuffer& bb, StreamState& ss);
    int32_t ParseUserData(void);
    
private:
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#endif

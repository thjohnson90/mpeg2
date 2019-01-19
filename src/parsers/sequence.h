#ifndef __SEQUENCE_H__
#define __SEQUENCE_H__

class SeqHdrParser
{
public:
    SeqHdrParser(BitBuffer& bb, StreamState& ss);
    uint32_t ParseSequenceHdr(void);
    
protected:
    uint32_t LoadQuantMatrix(uint8_t (&q)[65]);
    
private:
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#endif

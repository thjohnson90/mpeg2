#ifndef __SEQUENCE_H__
#define __SEQUENCE_H__

class SeqHdrParser
{
public:
    SeqHdrParser(BitBuffer& bb, StreamState& ss);
    int32_t ParseSequenceHdr(void);
    
protected:
    int32_t LoadQuantMatrix(uint8_t (&q)[64]);
    
private:
    BitBuffer&   _bitBuffer;
    StreamState& _streamState;
};

#endif

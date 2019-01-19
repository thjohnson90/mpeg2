#include <fstream>
#include <stdint.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "syshdr.h"

SystemHdrParser::SystemHdrParser(BitBuffer& bb, StreamState& ss) : _bitBuffer(bb), _streamState(ss)
{
}

uint32_t SystemHdrParser::ParseSystemHdr(void)
{
    uint32_t status = 0;
    uint32_t marker = 0;
    
    do {
        _streamState.sysHdr.reset_system_header();

        _streamState.sysHdr.hdr_len                      = _bitBuffer.GetBits(16);
        marker                                           = _bitBuffer.GetBits(1);
        CHECK_VALUE_AND_BREAK(1, marker, status, -1);
        _streamState.sysHdr.rate_bound                   = _bitBuffer.GetBits(22);
        marker                                           = _bitBuffer.GetBits(1);
        CHECK_VALUE_AND_BREAK(1, marker, status, -1);
        _streamState.sysHdr.audio_bound                  = _bitBuffer.GetBits(6);
        _streamState.sysHdr.fixed_flag                   = _bitBuffer.GetBits(1);
        _streamState.sysHdr.CSPS_flag                    = _bitBuffer.GetBits(1);
        _streamState.sysHdr.system_audio_lock_flag       = _bitBuffer.GetBits(1);
        _streamState.sysHdr.system_video_lock_flag       = _bitBuffer.GetBits(1);
        marker                                           = _bitBuffer.GetBits(1);
        CHECK_VALUE_AND_BREAK(1, marker, status, -1);
        _streamState.sysHdr.video_bound                  = _bitBuffer.GetBits(5);
        _streamState.sysHdr.packet_rate_restriction_flag = _bitBuffer.GetBits(1);
        _streamState.sysHdr.reserved_byte                = _bitBuffer.GetBits(7);
        CHECK_VALUE_AND_BREAK(0x7F, _streamState.sysHdr.reserved_byte, status, -1);

        struct stream_id_syshdr** ppStrmId = &_streamState.sysHdr.pNextStrmId;

        while (1 == _bitBuffer.PeekBits(1)) {
            *ppStrmId = new struct stream_id_syshdr;
            if (0 != *ppStrmId) {
                (*ppStrmId)->stream_id                    = _bitBuffer.GetBits(8);
                marker                                    = _bitBuffer.GetBits(2);
                CHECK_VALUE_AND_BREAK(3, marker, status, -1);
                (*ppStrmId)->PSTD_buffer_bound_scale      = _bitBuffer.GetBits(1);
                (*ppStrmId)->PSTD_buffer_size_bound       = _bitBuffer.GetBits(13);
                (*ppStrmId)->pNext                        = nullptr;
                ppStrmId = &((*ppStrmId)->pNext);
            } else {
                status = -1;
                break;
            }
        }
     } while (0);
    
    return status;
}

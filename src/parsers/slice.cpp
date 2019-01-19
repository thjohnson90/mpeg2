#include <stdint.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "picdata.h"
#include "slice.h"

SliceParser::SliceParser(BitBuffer& bb, StreamState& ss) : _bitBuffer(bb), _streamState(ss)
{
}

uint32_t SliceParser::ParseSliceData(void)
{
    uint32_t     status  = 0;
    uint32_t     marker  = 0;
    PictureData* picData = 0;
    
    do {
        PictureDataMgr* picDataMgr = PictureDataMgr::GetPictureDataMgr(_streamState);
        if (0 == picDataMgr) {
            status = -1;
            break;
        }
        
	// do we really need another buffer - already got one when parsing the pic header
        picData = picDataMgr->GetNextBuffer();
        if (0 == picData) {
            status = -1;
            break;
        }
        
        if (2800 < _streamState.seqHdr.vertical_sz) {
            picData->sliceData.slice_vertical_position_ext = _bitBuffer.GetBits(3);
        }
    } while (0);
    
    return status;
}

#include <stdint.h>

using namespace std;

#include "stream.h"
#include "bitbuf.h"
#include "peshdr.h"

PesHeaderParser::PesHeaderParser(BitBuffer& bb, StreamState& ss) : _bitBuffer(bb), _streamState(ss)
{
}

int32_t PesHeaderParser::ParsePesHdr(void)
{
    int32_t status = 0;
    
    do {
        _streamState.pesHdr.stream_id  = _bitBuffer.GetLastStartCode();
        _streamState.pesHdr.packet_len = _bitBuffer.GetBits(16);
    } while (0);
    
    return status;
}

PesPacketParser::PesPacketParser(BitBuffer& bb, StreamState& ss) : _bitBuffer(bb), _streamState(ss)
{
}

int32_t PesPacketParser::ChoosePesPacketParser(void)
{
    int32_t status = 0;
    
    do {
        // check for other stream types
        if (_streamState.pesHdr.stream_id != StreamState::program_stream_map &&
            _streamState.pesHdr.stream_id != StreamState::pes_padding_stream &&
            _streamState.pesHdr.stream_id != StreamState::pes_private_stream2 &&
            _streamState.pesHdr.stream_id != StreamState::ECM_stream &&
            _streamState.pesHdr.stream_id != StreamState::EMM_stream &&
            _streamState.pesHdr.stream_id != StreamState::program_stream_dir &&
            _streamState.pesHdr.stream_id != StreamState::DSMCC_stream &&
            _streamState.pesHdr.stream_id != StreamState::H222_1_type_E_stream) {
            status = ParsePesPacket();
        } else if (_streamState.pesHdr.stream_id == StreamState::program_stream_map ||
                 _streamState.pesHdr.stream_id == StreamState::pes_private_stream2 ||
                 _streamState.pesHdr.stream_id == StreamState::ECM_stream ||
                 _streamState.pesHdr.stream_id == StreamState::EMM_stream ||
                 _streamState.pesHdr.stream_id == StreamState::program_stream_dir ||
                 _streamState.pesHdr.stream_id == StreamState::DSMCC_stream ||
                   _streamState.pesHdr.stream_id == StreamState::H222_1_type_E_stream) {
            status = ParsePesDataBytes();
        } else if (_streamState.pesHdr.stream_id == StreamState::pes_padding_stream) {
            status = ParsePesPadding();
        }
        
    } while (0);
    
    return status;
}

int32_t PesPacketParser::ParsePesPacket(void)
{
    int32_t  status = 0;
    uint32_t marker = 0;
    uint32_t bUsed  = 0;
    
    do {
        marker = _bitBuffer.GetBits(2);
        CHECK_VALUE_AND_BREAK(2, marker, status, -1);
        _streamState.pesHdr.ext.PES_scrambling_control   = _bitBuffer.GetBits(2);
        _streamState.pesHdr.ext.PES_priority             = _bitBuffer.GetBits(1);
        _streamState.pesHdr.ext.data_alignment_indicator = _bitBuffer.GetBits(1);
        _streamState.pesHdr.ext.copyright                = _bitBuffer.GetBits(1);
        _streamState.pesHdr.ext.original_copy            = _bitBuffer.GetBits(1);
        _streamState.pesHdr.ext.PTS_DTS_flags            = _bitBuffer.GetBits(2);
        _streamState.pesHdr.ext.ESCR_flag                = _bitBuffer.GetBits(1);
        _streamState.pesHdr.ext.ES_rate_flag             = _bitBuffer.GetBits(1);
        _streamState.pesHdr.ext.DSM_trick_mode_flag      = _bitBuffer.GetBits(1);
        _streamState.pesHdr.ext.addition_copy_info_flag  = _bitBuffer.GetBits(1);
        _streamState.pesHdr.ext.PES_CRC_flag             = _bitBuffer.GetBits(1);
        _streamState.pesHdr.ext.PES_extension_flag       = _bitBuffer.GetBits(1);
        _streamState.pesHdr.ext.PES_header_data_len      = _bitBuffer.GetBits(8);
        
        // check for PTS/DTS
        if (0x02 == _streamState.pesHdr.ext.PTS_DTS_flags) {
            marker                           = _bitBuffer.GetBits(4);
            if (0x02 == _streamState.pesHdr.ext.PTS_DTS_flags) {
                CHECK_VALUE_AND_BREAK(2, marker, status, -1);
            } else if (0x03 == _streamState.pesHdr.ext.PTS_DTS_flags) {
                CHECK_VALUE_AND_BREAK(3, marker, status, -1);
            }
            _streamState.pesHdr.ext.PTS32_30 = _bitBuffer.GetBits(3);
            marker                           = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.PTS29_15 = _bitBuffer.GetBits(15);
            marker                           = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.PTS14_00 = _bitBuffer.GetBits(15);
            marker                           = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);

	    bUsed += 5;
        }
        
        if (0x03 == _streamState.pesHdr.ext.PTS_DTS_flags) {
            marker                           = _bitBuffer.GetBits(4);
            CHECK_VALUE_AND_BREAK(3, marker, status, -1);

            _streamState.pesHdr.ext.PTS32_30 = _bitBuffer.GetBits(3);
            marker                           = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.PTS29_15 = _bitBuffer.GetBits(15);
            marker                           = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.PTS14_00 = _bitBuffer.GetBits(15);
            marker                           = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);

            marker                           = _bitBuffer.GetBits(4);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);

            _streamState.pesHdr.ext.DTS32_30 = _bitBuffer.GetBits(3);
            marker                           = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.DTS29_15 = _bitBuffer.GetBits(15);
            marker                           = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.DTS14_00 = _bitBuffer.GetBits(15);
            marker                           = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);

	    bUsed += 10;
        }
        
        // check for ESCR
        if (0x01 == _streamState.pesHdr.ext.ESCR_flag) {
            marker                            = _bitBuffer.GetBits(2);
            CHECK_VALUE_AND_BREAK(0, marker, status, -1);
            _streamState.pesHdr.ext.ESCR32_30 = _bitBuffer.GetBits(3);
            marker                            = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.ESCR29_15 = _bitBuffer.GetBits(15);
            marker                            = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.ESCR14_00 = _bitBuffer.GetBits(15);
            marker                            = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.ESCR_ext  = _bitBuffer.GetBits(9);
            marker                            = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);

	    bUsed += 6;
        }
        
        // check for ES rate
        if (1 == _streamState.pesHdr.ext.ES_rate_flag) {
            marker                          = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.ES_rate = _bitBuffer.GetBits(22);
            marker                          = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);

	    bUsed += 3;
        }
        
        // check for DSM trick mode - not used by DVD
        if (1 == _streamState.pesHdr.ext.DSM_trick_mode_flag) {
	    // TODO: fill in
        }
        
        // check for additional copy info
        if (1 == _streamState.pesHdr.ext.addition_copy_info_flag) {
            marker                                       = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.additional_copy_info = _bitBuffer.GetBits(7);

	    bUsed += 1;
        }
        
        // check for PES CRC
        if (_streamState.pesHdr.ext.PES_CRC_flag) {
            _streamState.pesHdr.ext.previous_PES_packet_CRC = _bitBuffer.GetBits(16);

	    bUsed += 2;
        }
        
        // check for PES extension
        if (1 == _streamState.pesHdr.ext.PES_extension_flag) {
            _streamState.pesHdr.ext.PES_private_data_flag    = _bitBuffer.GetBits(1);
            _streamState.pesHdr.ext.pack_header_field_flag   = _bitBuffer.GetBits(1);
            _streamState.pesHdr.ext.prog_packet_seq_ctr_flag = _bitBuffer.GetBits(1);
            _streamState.pesHdr.ext.PSTD_buff_flag           = _bitBuffer.GetBits(1);
            marker                                           = _bitBuffer.GetBits(3);
            _streamState.pesHdr.ext.PES_ext_flag2            = _bitBuffer.GetBits(1);

	    bUsed += 1;
        }
        
        // if PES_private_data_flag is set, an additional 16 bytes of user defined date is appended
        // TODO: add support for private data
        CHECK_VALUE_AND_BREAK(0, _streamState.pesHdr.ext.PES_private_data_flag, status, -1);
        
        // if pack_header_field_flag is set, an additional 8 bits of pack field length value is appended
        // TODO: add support for pack_header_flag
        CHECK_VALUE_AND_BREAK(0, _streamState.pesHdr.ext.pack_header_field_flag, status, -1);
        
        // check for program sequence counter
        if (1 == _streamState.pesHdr.ext.prog_packet_seq_ctr_flag) {
            marker                                         = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.packet_seq_counter     = _bitBuffer.GetBits(7);
            marker                                         = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.MPEG1_MPEG2_identifier = _bitBuffer.GetBits(1);
            _streamState.pesHdr.ext.original_stuffing_len  = _bitBuffer.GetBits(6);

	    bUsed += 2;
        }
        
        // check for PSTD buffer flag
        if (1 == _streamState.pesHdr.ext.PSTD_buff_flag) {
            marker                                    = _bitBuffer.GetBits(2);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.PSTD_buffer_scale = _bitBuffer.GetBits(1);
            _streamState.pesHdr.ext.PSTD_buffer_size  = _bitBuffer.GetBits(13);

	    bUsed += 2;
        }
        
        // check for PES extension flag 2
	// TODO: neds to be corrected
        if (1 == _streamState.pesHdr.ext.PES_ext_flag2) {
            marker                                    = _bitBuffer.GetBits(1);
            CHECK_VALUE_AND_BREAK(1, marker, status, -1);
            _streamState.pesHdr.ext.PES_ext_field_len = _bitBuffer.GetBits(7);
            _streamState.pesHdr.ext.reserved          = _bitBuffer.GetBits(8);

	    bUsed += 2;
        }

	_streamState.pesHdr.packet_len -= (_streamState.pesHdr.ext.PES_header_data_len + 3);
	if (0 != _streamState.pesHdr.ext.PES_header_data_len - bUsed) {
	    uint8_t buf[_streamState.pesHdr.ext.PES_header_data_len];

	    _bitBuffer.GetBytes(buf, _streamState.pesHdr.ext.PES_header_data_len - bUsed);
	}
    } while(0);
    
    return status;
}

int32_t PesPacketParser::ParsePesPadding(void)
{
    int32_t status = 0;
    
    do {
        if (0 != _streamState.pesHdr.packet_len) {
            // read (and dump) the padding bytes
            uint8_t* buf = new uint8_t[_streamState.pesHdr.packet_len + 1];
            if (nullptr == buf) {
                status = -1;
                break;
            }
            
	    // add one for null termination
            status = _bitBuffer.GetBytes(buf, _streamState.pesHdr.packet_len + 1);
            delete [] buf;
        }
    } while (0);
    
    return status;
}

int32_t PesPacketParser::ParsePesDataBytes(void)
{
    int32_t status = 0;
    
    do {
        if (0 != _streamState.pesHdr.packet_len) {
            // read (and dump) the data bytes - until I know what to do with them
            uint8_t* buf = new uint8_t[_streamState.pesHdr.packet_len + 1];
            if (nullptr == buf) {
                status = -1;
                break;
            }

	    // add one for null termination
            status = _bitBuffer.GetBytes(buf, _streamState.pesHdr.packet_len + 1);
            delete [] buf;
        }
    } while (0);
    
    return status;
}


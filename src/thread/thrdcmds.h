#ifndef __THRDCMDS_H__
#define __THRDCMDS_H__

#define NUM_HW_THREADS 4

namespace common_cmd {
    enum {
	null = 0,
	ack  = 50,
	exit = 51
    };
};
	
namespace parse_cmd {
    enum {
	data_ready       = 1,
	data_consumed    = 2,
	seq_end_received = 3
    };
};

namespace idct_cmd {
    enum {
	process_block_0     = 0,
	process_block_1     = 1,
	process_block_2     = 2,
	process_block_3     = 3,
	process_block_4     = 4,
	process_block_5     = 5,
	process_block_6     = 6,
	process_block_7     = 7,
	process_block_8     = 8,
	process_block_9     = 9,
	process_block_10    = 10,
	process_block_11    = 11,
	processing_complete = 12
    };
};

#endif

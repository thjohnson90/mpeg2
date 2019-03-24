#include <iostream>
#include <pthread.h>

#include "stream.h"
#include "bitbuf.h"
#include "pack.h"
#include "syshdr.h"
#include "peshdr.h"
#include "sequence.h"
#include "extension.h"
#include "user.h"
#include "gop.h"
#include "picdata.h"
#include "motvecs.h"
#include "videoproc.h"
#include "block.h"
#include "macroblk.h"
#include "slice.h"
#include "picture.h"
#include "doorbell.h"
#include "thread.h"
#include "base_parser.h"
#include "buf_bitbuf.h"
#include "picdata.h"
#include "block.h"

using namespace std;

extern int32_t quan_scale[2][32];

struct test_case {
    uint32_t qstype;
    uint32_t qscode;
    uint32_t exp;
};

struct test_case qscal_test_cases[] = {
    {0,  0,  0},
    {0,  1,  2},
    {0,  2,  4},
    {0,  3,  6},
    {0,  4,  8},
    {0,  5, 10},
    {0,  6, 12},
    {0,  7, 14},

    {0,  8, 16},
    {0,  9, 18},
    {0, 10, 20},
    {0, 11, 22},
    {0, 12, 24},
    {0, 13, 26},
    {0, 14, 28},
    {0, 15, 30},

    {0, 16, 32},
    {0, 17, 34},
    {0, 18, 36},
    {0, 19, 38},
    {0, 20, 40},
    {0, 21, 42},
    {0, 22, 44},
    {0, 23, 46},

    {0, 24, 48},
    {0, 25, 50},
    {0, 26, 52},
    {0, 27, 54},
    {0, 28, 56},
    {0, 29, 58},
    {0, 30, 60},
    {0, 31, 62},

    {1,  0,  0},
    {1,  1,  1},
    {1,  2,  2},
    {1,  3,  3},
    {1,  4,  4},
    {1,  5,  5},
    {1,  6,  6},
    {1,  7,  7},

    {1,  8,  8},
    {1,  9, 10},
    {1, 10, 12},
    {1, 11, 14},
    {1, 12, 16},
    {1, 13, 18},
    {1, 14, 20},
    {1, 15, 22},

    {1, 16, 24},
    {1, 17, 28},
    {1, 18, 32},
    {1, 19, 36},
    {1, 20, 40},
    {1, 21, 44},
    {1, 22, 48},
    {1, 23, 52},

    {1, 24, 56},
    {1, 25, 64},
    {1, 26, 72},
    {1, 27, 80},
    {1, 28, 88},
    {1, 29, 96},
    {1, 30, 104},
    {1, 31, 112}
};

int main(void)
{
    uint32_t i = 0;
    uint32_t v = 0;
    
    BufBitBuffer bbf;
    StreamState  ss;
    
    BlockParser bp(bbf, ss);
    struct test_case* tc = nullptr;

    for (i = 0; i < sizeof(qscal_test_cases) / sizeof(struct test_case); i++) {
	cout << "Inverse Scan Test Case " << dec << i+1 << ": ";
	tc = &qscal_test_cases[i];
	
	v = quan_scale[tc->qstype][tc->qscode];
	if (v != tc->exp) {
	    cout << "Run Failed (got " << v << ", exp " << tc->exp << ")" << endl;
	} else {
	    cout << "Passed" << endl;
	}
    }

    return 0;
}

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

extern uint32_t inv_scan[2][8][8];

struct test_case {
    uint32_t alt;
    uint32_t v;
    uint32_t u;
    uint32_t exp;
};

struct test_case iscan_test_cases[] = {
    {0, 0, 0,  0},
    {0, 0, 1,  1},
    {0, 0, 2,  5},
    {0, 0, 3,  6},
    {0, 0, 4, 14},
    {0, 0, 5, 15},
    {0, 0, 6, 27},
    {0, 0, 7, 28},

    {0, 1, 0,  2},
    {0, 1, 1,  4},
    {0, 1, 2,  7},
    {0, 1, 3, 13},
    {0, 1, 4, 16},
    {0, 1, 5, 26},
    {0, 1, 6, 29},
    {0, 1, 7, 42},

    {0, 2, 0,  3},
    {0, 2, 1,  8},
    {0, 2, 2, 12},
    {0, 2, 3, 17},
    {0, 2, 4, 25},
    {0, 2, 5, 30},
    {0, 2, 6, 41},
    {0, 2, 7, 43},

    {0, 3, 0,  9},
    {0, 3, 1, 11},
    {0, 3, 2, 18},
    {0, 3, 3, 24},
    {0, 3, 4, 31},
    {0, 3, 5, 40},
    {0, 3, 6, 44},
    {0, 3, 7, 53},

    {0, 4, 0, 10},
    {0, 4, 1, 19},
    {0, 4, 2, 23},
    {0, 4, 3, 32},
    {0, 4, 4, 39},
    {0, 4, 5, 45},
    {0, 4, 6, 52},
    {0, 4, 7, 54},

    {0, 5, 0, 20},
    {0, 5, 1, 22},
    {0, 5, 2, 33},
    {0, 5, 3, 38},
    {0, 5, 4, 46},
    {0, 5, 5, 51},
    {0, 5, 6, 55},
    {0, 5, 7, 60},

    {0, 6, 0, 21},
    {0, 6, 1, 34},
    {0, 6, 2, 37},
    {0, 6, 3, 47},
    {0, 6, 4, 50},
    {0, 6, 5, 56},
    {0, 6, 6, 59},
    {0, 6, 7, 61},

    {0, 7, 0, 35},
    {0, 7, 1, 36},
    {0, 7, 2, 48},
    {0, 7, 3, 49},
    {0, 7, 4, 57},
    {0, 7, 5, 58},
    {0, 7, 6, 62},
    {0, 7, 7, 63},

    {1, 0, 0, 0},
    {1, 0, 1, 4},
    {1, 0, 2, 6},
    {1, 0, 3, 20},
    {1, 0, 4, 22},
    {1, 0, 5, 36},
    {1, 0, 6, 38},
    {1, 0, 7, 52},

    {1, 1, 0, 1},
    {1, 1, 1, 5},
    {1, 1, 2, 7},
    {1, 1, 3, 21},
    {1, 1, 4, 23},
    {1, 1, 5, 37},
    {1, 1, 6, 39},
    {1, 1, 7, 53},

    {1, 2, 0, 2},
    {1, 2, 1, 8},
    {1, 2, 2, 19},
    {1, 2, 3, 24},
    {1, 2, 4, 34},
    {1, 2, 5, 40},
    {1, 2, 6, 50},
    {1, 2, 7, 54},

    {1, 3, 0, 3},
    {1, 3, 1, 9},
    {1, 3, 2, 18},
    {1, 3, 3, 25},
    {1, 3, 4, 35},
    {1, 3, 5, 41},
    {1, 3, 6, 51},
    {1, 3, 7, 55},

    {1, 4, 0, 10},
    {1, 4, 1, 17},
    {1, 4, 2, 26},
    {1, 4, 3, 30},
    {1, 4, 4, 42},
    {1, 4, 5, 46},
    {1, 4, 6, 56},
    {1, 4, 7, 60},

    {1, 5, 0, 11},
    {1, 5, 1, 16},
    {1, 5, 2, 27},
    {1, 5, 3, 31},
    {1, 5, 4, 43},
    {1, 5, 5, 47},
    {1, 5, 6, 57},
    {1, 5, 7, 61},

    {1, 6, 0, 12},
    {1, 6, 1, 15},
    {1, 6, 2, 28},
    {1, 6, 3, 32},
    {1, 6, 4, 44},
    {1, 6, 5, 48},
    {1, 6, 6, 58},
    {1, 6, 7, 62},

    {1, 7, 0, 13},
    {1, 7, 1, 14},
    {1, 7, 2, 29},
    {1, 7, 3, 33},
    {1, 7, 4, 45},
    {1, 7, 5, 49},
    {1, 7, 6, 59},
    {1, 7, 7, 63}
};

int main(void)
{
    uint32_t i = 0;
    uint32_t v = 0;
    
    BufBitBuffer bbf;
    StreamState  ss;
    
    BlockParser bp(bbf, ss);
    struct test_case* tc = nullptr;

    for (i = 0; i < sizeof(iscan_test_cases) / sizeof(struct test_case); i++) {
	cout << "Inverse Scan Test Case " << dec << i+1 << ": ";
	tc = &iscan_test_cases[i];
	
	v = inv_scan[tc->alt][tc->v][tc->u];
	if (v != tc->exp) {
	    cout << "Run Failed (got " << v << ", exp " << tc->exp << ")" << endl;
	} else {
	    cout << "Passed" << endl;
	}
    }

    return 0;
}

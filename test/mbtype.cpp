#include <iostream>
#include <pthread.h>
#include <string.h>

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

struct test_case {
    uint64_t bits;
    uint32_t peek;
    uint32_t macroblock_quant                  : 1;
    uint32_t macroblock_motion_forward         : 1;
    uint32_t macroblock_motion_backward        : 1;
    uint32_t macroblock_pattern                : 1;
    uint32_t macroblock_intra                  : 1;
    uint32_t spatial_temporal_weight_code_flag : 1;
};

struct test_case B2nsc_test_cases[] = {
    {0x2, 2, 0, 0, 0, 0, 1, 0},
    {0x1, 2, 1, 0, 0, 0, 1, 0}
};

struct test_case B3nsc_test_cases[] = {
    {0x20, 6, 0, 1, 0, 1, 0, 0},
    {0x10, 6, 0, 0, 0, 1, 0, 0},
    {0x8,  6, 0, 1, 0, 0, 0, 0},
    {0x6,  6, 0, 0, 0, 0, 1, 0},
    {0x4,  6, 1, 1, 0, 1, 0, 0},
    {0x2,  6, 1, 0, 0, 1, 0, 0},
    {0x1,  6, 1, 0, 0, 0, 1, 0}
};

struct test_case B4nsc_test_cases[] = {
    {0x20, 6, 0, 1, 1, 0, 0, 0},
    {0x30, 6, 0, 1, 1, 1, 0, 0},
    {0x10, 6, 0, 0, 1, 0, 0, 0},
    {0x18, 6, 0, 0, 1, 1, 0, 0},
    {0x8,  6, 0, 1, 0, 0, 0, 0},
    {0xC,  6, 0, 1, 0, 1, 0, 0},
    {0x6,  6, 0, 0, 0, 0, 1, 0},
    {0x4,  6, 1, 1, 1, 1, 0, 0},
    {0x3,  6, 1, 1, 0, 1, 0, 0},
    {0x2,  6, 1, 0, 1, 1, 0, 0},
    {0x1,  6, 1, 0, 0, 0, 1, 0}
};

struct test_case B5sc_test_cases[] = {
    {0x8, 4, 0, 0, 0, 1, 0, 0},
    {0x4, 4, 1, 0, 0, 1, 0, 0},
    {0x3, 4, 0, 0, 0, 0, 1, 0},
    {0x2, 4, 1, 0, 0, 0, 1, 0},
    {0x1, 4, 0, 0, 0, 0, 0, 0}
};

struct test_case B6sc_test_cases[] = {
    {0x40, 7, 0, 1, 0, 1, 0, 0},
    {0x30, 7, 0, 1, 0, 1, 0, 1},
    {0x4,  7, 0, 0, 0, 1, 0, 0},
    {0xE,  7, 0, 0, 0, 1, 0, 1},
    {0x10, 7, 0, 1, 0, 0, 0, 0},
    {0x7,  7, 0, 0, 0, 0, 1, 0},
    {0x18, 7, 0, 1, 0, 0, 0, 1},
    {0x20, 7, 1, 1, 0, 1, 0, 0},
    {0x8,  7, 1, 0, 0, 1, 0, 0},
    {0x6,  7, 1, 0, 0, 0, 1, 0},
    {0x60, 7, 1, 1, 0, 1, 0, 1},
    {0xA,  7, 1, 0, 0, 1, 0, 1},
    {0xC,  7, 0, 0, 0, 0, 0, 1},
    {0x5,  7, 0, 0, 0, 1, 0, 0},
    {0x2,  7, 1, 0, 0, 1, 0, 0},
    {0x3,  7, 0, 0, 0, 0, 0, 0}
};

struct test_case B7sc_test_cases[] = {
    {0x100, 9, 0, 1, 1, 0, 0, 0},
    {0x180, 9, 0, 1, 1, 1, 0, 0},
    {0x80,  9, 0, 0, 1, 0, 0, 0},
    {0xC0,  9, 0, 0, 1, 1, 0, 0},
    {0x40,  9, 0, 1, 0, 0, 0, 0},
    {0x60,  9, 0, 1, 0, 1, 0, 0},
    {0x30,  9, 0, 0, 1, 0, 0, 1},
    {0x38,  9, 0, 0, 1, 1, 0, 1},
    {0x20,  9, 0, 1, 0, 0, 0, 1},
    {0x28,  9, 0, 1, 0, 1, 0, 1},
    {0x18,  9, 0, 0, 0, 0, 1, 0},
    {0x1C,  9, 1, 1, 1, 1, 0, 0},
    {0x10,  9, 1, 1, 0, 1, 0, 0},
    {0x14,  9, 1, 0, 1, 1, 0, 0},
    {0x8,   9, 1, 0, 0, 0, 1, 0},
    {0xA,   9, 1, 1, 0, 1, 0, 1},
    {0xC,   9, 1, 0, 1, 1, 0, 1},
    {0xE,   9, 0, 0, 0, 0, 0, 0},
    {0xD,   9, 1, 0, 0, 1, 0, 0},
    {0xF,   9, 0, 0, 0, 1, 0, 0},
};

int main(void)
{
    int32_t  status = 0;
    uint32_t i      = 0;
    uint32_t inc    = 0;

    BufBitBuffer bbf;
    StreamState  ss;
    PictureData  pd;

    memset(&ss, 0, sizeof(ss));
    memset(&pd, 0, sizeof(pd));
    
    MacroblkParser mp(bbf, ss);
    struct test_case* tc = nullptr;

    cout << "Non-spatial Scaliability Cases" << endl;
    pd.picHdr.picture_coding_type = PictureHeader::PIC_CODING_TYPE_I;
    cout << "I-picture" << endl;
    for (i = 0; i < sizeof(B2nsc_test_cases) / sizeof(struct test_case); i++) {
	cout << "B2 NSC Test Case " << dec << i+1 << ": ";
	tc = &B2nsc_test_cases[i];
	
	bbf.PokeBits(tc->bits, tc->peek);
	status = mp.RunGetMacroblkModes(&pd);
	if (-1 == status) {
	    cout << "Function Error Detected" << endl;
	    continue;
	}

	if (tc->macroblock_quant != pd.macroblkData.macroblock_quant ||
	    tc->macroblock_motion_forward != pd.macroblkData.macroblock_motion_forw ||
	    tc->macroblock_motion_backward != pd.macroblkData.macroblock_motion_back ||
	    tc->macroblock_pattern != pd.macroblkData.macroblock_pattern ||
	    tc->macroblock_intra != pd.macroblkData.macroblock_intra ||
	    tc->spatial_temporal_weight_code_flag != pd.macroblkData.spatial_temporal_weight_code_flag) {
	    cout << "MB Type Error" << endl;
	} else {
	    cout << "Passed" << endl;
	}
    }

    pd.picHdr.picture_coding_type = PictureHeader::PIC_CODING_TYPE_P;
    cout << "P-picture" << endl;
    for (i = 0; i < sizeof(B3nsc_test_cases) / sizeof(struct test_case); i++) {
	cout << "B3 NSC Test Case " << dec << i+1 << ": ";
	tc = &B3nsc_test_cases[i];
	
	bbf.PokeBits(tc->bits, tc->peek);
	status = mp.RunGetMacroblkModes(&pd);
	if (-1 == status) {
	    cout << "Function Error Detected" << endl;
	    continue;
	}

	if (tc->macroblock_quant != pd.macroblkData.macroblock_quant ||
	    tc->macroblock_motion_forward != pd.macroblkData.macroblock_motion_forw ||
	    tc->macroblock_motion_backward != pd.macroblkData.macroblock_motion_back ||
	    tc->macroblock_pattern != pd.macroblkData.macroblock_pattern ||
	    tc->macroblock_intra != pd.macroblkData.macroblock_intra ||
	    tc->spatial_temporal_weight_code_flag != pd.macroblkData.spatial_temporal_weight_code_flag) {
	    cout << "MB Type Error" << endl;
	} else {
	    cout << "Passed" << endl;
	}
    }

    pd.picHdr.picture_coding_type = PictureHeader::PIC_CODING_TYPE_B;
    cout << "B-picture" << endl;
    for (i = 0; i < sizeof(B4nsc_test_cases) / sizeof(struct test_case); i++) {
	cout << "B4 NSC Test Case " << dec << i+1 << ": ";
	tc = &B4nsc_test_cases[i];
	
	bbf.PokeBits(tc->bits, tc->peek);
	status = mp.RunGetMacroblkModes(&pd);
	if (-1 == status) {
	    cout << "Function Error Detected" << endl;
	    continue;
	}

	if (tc->macroblock_quant != pd.macroblkData.macroblock_quant ||
	    tc->macroblock_motion_forward != pd.macroblkData.macroblock_motion_forw ||
	    tc->macroblock_motion_backward != pd.macroblkData.macroblock_motion_back ||
	    tc->macroblock_pattern != pd.macroblkData.macroblock_pattern ||
	    tc->macroblock_intra != pd.macroblkData.macroblock_intra ||
	    tc->spatial_temporal_weight_code_flag != pd.macroblkData.spatial_temporal_weight_code_flag) {
	    cout << "MB Type Error" << endl;
	} else {
	    cout << "Passed" << endl;
	}
    }

    ss.extData.seqScalExt.scalable_mode = sequence_scalable_extension::spatial_scalability;
    cout << endl << "Spatial Scalability Cases" << endl;
    
    pd.picHdr.picture_coding_type = PictureHeader::PIC_CODING_TYPE_I;
    cout << "I-picture" << endl;
    for (i = 0; i < sizeof(B5sc_test_cases) / sizeof(struct test_case); i++) {
	cout << "B5 SC Test Case " << dec << i+1 << ": ";
	tc = &B5sc_test_cases[i];
	
	bbf.PokeBits(tc->bits, tc->peek);
	status = mp.RunGetMacroblkModes(&pd);
	if (-1 == status) {
	    cout << "Function Error Detected" << endl;
	    continue;
	}

	if (tc->macroblock_quant != pd.macroblkData.macroblock_quant ||
	    tc->macroblock_motion_forward != pd.macroblkData.macroblock_motion_forw ||
	    tc->macroblock_motion_backward != pd.macroblkData.macroblock_motion_back ||
	    tc->macroblock_pattern != pd.macroblkData.macroblock_pattern ||
	    tc->macroblock_intra != pd.macroblkData.macroblock_intra ||
	    tc->spatial_temporal_weight_code_flag != pd.macroblkData.spatial_temporal_weight_code_flag) {
	    cout << "MB Type Error" << endl;
	} else {
	    cout << "Passed" << endl;
	}
    }

    pd.picHdr.picture_coding_type = PictureHeader::PIC_CODING_TYPE_P;
    cout << "P-picture" << endl;
    for (i = 0; i < sizeof(B6sc_test_cases) / sizeof(struct test_case); i++) {
	cout << "B6 SC Test Case " << dec << i+1 << ": ";
	tc = &B6sc_test_cases[i];
	
	bbf.PokeBits(tc->bits, tc->peek);
	status = mp.RunGetMacroblkModes(&pd);
	if (-1 == status) {
	    cout << "Function Error Detected" << endl;
	    continue;
	}

	if (tc->macroblock_quant != pd.macroblkData.macroblock_quant ||
	    tc->macroblock_motion_forward != pd.macroblkData.macroblock_motion_forw ||
	    tc->macroblock_motion_backward != pd.macroblkData.macroblock_motion_back ||
	    tc->macroblock_pattern != pd.macroblkData.macroblock_pattern ||
	    tc->macroblock_intra != pd.macroblkData.macroblock_intra ||
	    tc->spatial_temporal_weight_code_flag != pd.macroblkData.spatial_temporal_weight_code_flag) {
	    cout << "MB Type Error" << endl;
	} else {
	    cout << "Passed" << endl;
	}
    }

    pd.picHdr.picture_coding_type = PictureHeader::PIC_CODING_TYPE_B;
    cout << "B-picture" << endl;
    for (i = 0; i < sizeof(B7sc_test_cases) / sizeof(struct test_case); i++) {
	cout << "B7 SC Test Case " << dec << i+1 << ": ";
	tc = &B7sc_test_cases[i];
	
	bbf.PokeBits(tc->bits, tc->peek);
	status = mp.RunGetMacroblkModes(&pd);
	if (-1 == status) {
	    cout << "Function Error Detected" << endl;
	    continue;
	}

	if (tc->macroblock_quant != pd.macroblkData.macroblock_quant ||
	    tc->macroblock_motion_forward != pd.macroblkData.macroblock_motion_forw ||
	    tc->macroblock_motion_backward != pd.macroblkData.macroblock_motion_back ||
	    tc->macroblock_pattern != pd.macroblkData.macroblock_pattern ||
	    tc->macroblock_intra != pd.macroblkData.macroblock_intra ||
	    tc->spatial_temporal_weight_code_flag != pd.macroblkData.spatial_temporal_weight_code_flag) {
	    cout << "MB Type Error" << endl;
	} else {
	    cout << "Passed" << endl;
	}
    }
    
    return 0;
}

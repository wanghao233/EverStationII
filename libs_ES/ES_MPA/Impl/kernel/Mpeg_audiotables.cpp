#include "Mpeg_audiotables.h"
/*------------------------------------------------------------------------
 *
 * Copyright (c) 1997-1998 by Cornell University.
 * 
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 *------------------------------------------------------------------------
 */
/*
 * bit_rate_table[i][j][k] indicates the bitrate for
 * id i, layer j + 1, index k
 */
const static int MPA_bit_rate_table[2][3][16] =
{
   {
		{FREE_BITRATE, 32000, 48000, 56000,  64000,  80000,  96000, 112000, 128000, 144000, 160000, 176000, 192000, 224000, 256000,},
		{FREE_BITRATE,  8000, 16000, 24000,  32000,  40000,  48000,  56000,  64000,  80000,  96000, 112000, 128000, 144000, 160000,},
		{FREE_BITRATE,  8000, 16000, 24000,  32000,  40000,  48000,  56000,  64000,  80000,  96000, 112000, 128000, 144000, 160000,}
   },
   {
		{FREE_BITRATE, 32000, 64000, 96000, 128000, 160000, 192000, 224000, 256000, 288000, 320000, 352000, 384000, 416000, 448000,},
		{FREE_BITRATE, 32000, 48000, 56000,  64000,  80000,  96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, 384000,},
		{FREE_BITRATE, 32000, 40000, 48000,  56000,  64000,  80000,  96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000,}
   }
};

/*
 * sampling_rate_table[i][j] indicates the sampling rate for
 * id i, index j
 */
const static int MPA_sampling_rate_table[2][4] =
{
    {22050, 24000, 16000, RESERVE},
	{44100, 48000, 32000, RESERVE}
};

const static double MPA_scale_factor_table[64] =
{
    2.00000000000000, 1.58740105196820, 1.25992104989487, 1.00000000000000,
    0.79370052598410, 0.62996052494744, 0.50000000000000, 0.39685026299205,
    0.31498026247372, 0.25000000000000, 0.19842513149602, 0.15749013123686,
    0.12500000000000, 0.09921256574801, 0.07874506561843, 0.06250000000000,
    0.04960628287401, 0.03937253280921, 0.03125000000000, 0.02480314143700,
    0.01968626640461, 0.01562500000000, 0.01240157071850, 0.00984313320230,
    0.00781250000000, 0.00620078535925, 0.00492156660115, 0.00390625000000,
    0.00310039267963, 0.00246078330058, 0.00195312500000, 0.00155019633981,
    0.00123039165029, 0.00097656250000, 0.00077509816991, 0.00061519582514,
    0.00048828125000, 0.00038754908495, 0.00030759791257, 0.00024414062500,
    0.00019377454248, 0.00015379895629, 0.00012207031250, 0.00009688727124,
    0.00007689947814, 0.00006103515625, 0.00004844363562, 0.00003844973907,
    0.00003051757813, 0.00002422181781, 0.00001922486954, 0.00001525878906,
    0.00001211090890, 0.00000961243477, 0.00000762939453, 0.00000605545445,
    0.00000480621738, 0.00000381469727, 0.00000302772723, 0.00000240310869,
    0.00000190734863, 0.00000151386361, 0.00000120155435, 1E-20
};


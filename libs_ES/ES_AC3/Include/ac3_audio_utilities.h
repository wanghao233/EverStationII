#ifndef __API_AC3_AUDIO_UTILITIES_H__
#define __API_AC3_AUDIO_UTILITIES_H__

#include <stdint.h>
#include "../Compile.h"

#define		AC3_DEBUG_NAL_UNIT				0
#define		AC3_DEBUG_DECODE				0

/*AC3: Audio*/
_CDL_EXPORT int AC3_DecodeSamplingFrequency(uint8_t fscod, char* pszText, int strSize);
_CDL_EXPORT int	AC3_DecodeFrameSize(uint8_t frsizecod, uint8_t fscod, char* pszText, int strSize);
_CDL_EXPORT int AC3_DecodeBitStreamMode(uint8_t bsmod, uint8_t acmod, char* pszText, int strSize);
_CDL_EXPORT int AC3_DecodeAudioCodingMode(uint8_t acmod, char* pszText, int strSize);
_CDL_EXPORT	int	AC3_DecodeCmixlevCodetoText(uint8_t cmixlevel, char* pszText, int strSize);
_CDL_EXPORT	int	AC3_DecodeSurmixlevCodetoText(uint8_t surmixlevel, char* pszText, int strSize);
_CDL_EXPORT	int	AC3_DecodeDsurmodCodetoText(uint8_t dsurmod, char* pszText, int strSize);


#endif


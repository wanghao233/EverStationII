#ifndef __MPEG_DVB_COMMON_H__
#define __MPEG_DVB_COMMON_H__

#include <stdint.h>

#include "../compile.h"

#define MPEG2_PVT_SECTION_MIN_SIZE			12

typedef struct _reserved_descriptor_s
{
	uint8_t*		descriptor_buf;						//8
	uint8_t			descriptor_size;					//8

	uint16_t		descriptor_tag;						//16
	uint8_t			descriptor_length;					//8

} reserved_descriptor_t, *preserved_descriptor_t;

//_CDL_EXPORT int MPEG_DVB_decode_reserved_descriptor(uint8_t *buf, int length, reserved_descriptor_t* preserved_descriptor);

typedef struct private_section_s
{
	uint8_t			table_id;									//8

	uint8_t			section_syntax_indicator;					//1
	uint8_t			private_indicator;							//1
	uint8_t			reserved0;									//2
	uint16_t		section_length;								//12

	uint16_t		table_id_extension;							//16

	uint8_t			reserved1;									//2
	uint8_t			version_number;								//5
	uint8_t			current_next_indicator;						//1

	uint8_t			section_number;								//8
	uint8_t			last_section_number;						//8

	uint16_t		private_data_length;
	uint8_t			private_data_byte[4093];					//4096-3

	uint32_t		CRC_32;										//32
	uint32_t		CRC_32_verify;								//32

} private_section_t, *pprivate_section_t;

_CDL_EXPORT	int	MPEG_DVB_PVT_DecodeSection(uint8_t* buf, int length, private_section_t* pPVTSection);

_CDL_EXPORT	int GetSectionMinMaxLength(uint8_t table_id, int* pmin_length, int* pmax_length);

_CDL_EXPORT int MPEG_DVB_NumericCoding2Text_TableID(uint16_t PID, uint8_t table_id, char* pszText, int strSize);
_CDL_EXPORT int MPEG_DVB_NumericCoding2Text_CASystemID(uint16_t CA_system_ID, char* pszText, int strSize);

#endif
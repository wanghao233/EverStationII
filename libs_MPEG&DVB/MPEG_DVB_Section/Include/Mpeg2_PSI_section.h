#ifndef _MPEG2_PSI_SECTION_H_
#define _MPEG2_PSI_SECTION_H_

#include "../compile.h"
#include "MPEG_DVB_Common.h"
#include "MPEG_DVB_SysCapability.h"

#include "DVB_SI_Descriptor.h"

#include "HAL/HAL_Sys/Include/INTTYPES.H"

#define MIN(x, y)						(x < y? x : y)
/*------------------------------------------------------------
Section size definition
-------------------------------------------------------------*/
#define MPEG2_PSI_PAT_SECTION_MAX_SIZE			1024
#define MPEG2_PSI_PMT_SECTION_MAX_SIZE			1024
#define MPEG2_PSI_CAT_SECTION_MAX_SIZE			1024
#define MPEG2_PSI_CMT_SECTION_MAX_SIZE			1024
#define MPEG2_PSI_TSDT_SECTION_MAX_SIZE			1024

#define MPEG2_PSI_PAT_SECTION_MIN_SIZE			12
#define MPEG2_PSI_PMT_SECTION_MIN_SIZE			16
#define MPEG2_PSI_CAT_SECTION_MIN_SIZE			12
#define MPEG2_PSI_CMT_SECTION_MIN_SIZE			12
#define MPEG2_PSI_TSDT_SECTION_MIN_SIZE			12

/*------------------------------------------------------------
		PAT definition 
-------------------------------------------------------------*/
typedef struct _PROGRAM_MAP_DESCRIPTION_s
{
	U16			program_number;
	U16			reserved : 3;
	U16			program_map_PID : 13;

} PROGRAM_MAP_DESCRIPTION_t, *PPROGRAM_MAP_DESCRIPTION_t;

typedef struct program_association_section_s
{
	U8			table_id;									//8

	U8			section_syntax_indicator;					//1
	U8			reserved_future_use;						//1
	U8			reserved0;									//2
	U16			section_length;								//12

	U16			transport_stream_id;						//16

	U8			reserved1;									//2
	U8			version_number;								//5
	U8			current_next_indicator;						//1

	U8			section_number;								//8
	U8			last_section_number;						//8

	S32							 N;
	PROGRAM_MAP_DESCRIPTION_t	 astProgram[MAX_PROGRAMS_PER_PAT_SECTION];			//N x 32					每个section最多只能携带253个节目映射信息

	U32			CRC_32;										//32
	U32			CRC_32_verify;								//32

} program_association_section_t, *pprogram_association_section_t;

_CDL_EXPORT int MPEG2_PSI_PAT_DecodeSection(uint8_t *buf, int length, program_association_section_t* pPATSection);

/*------------------------------------------------------------
		PMT definition 
-------------------------------------------------------------*/
typedef struct _ES_DESCRIPTION_s
{
	U8		stream_type;
	U8		stream_subtype;				//added by chendelin, descriptor_tag

	U16		reserved0 : 3;
	U16		elementary_PID : 13;

	U16	    reserved1 : 4;
	U16		ES_info_length : 12;

	S32								reserved_count;
	reserved_descriptor_t			reserved_descriptor[MAX_RESERVED_DESCRIPTORS];

} ES_DESCRIPTION_t, *PES_DESCRIPTION_t;

typedef struct TS_program_map_section_s
{
	U8		table_id;									//8

	U8		section_syntax_indicator;					//1
	U8		reserved_future_use;						//1
	U8		reserved0;									//2
	U16		section_length;								//12

	U16		program_number;								//16

	U8		reserved1;									//2
	U8		version_number;								//5
	U8		current_next_indicator;						//1

	U8		section_number;								//8
	U8		last_section_number;						//8

	U8		reserved2;									// 3
	U16		PCR_PID;									//13

	U8		reserved3;									// 4
	U16		program_info_length;						//12

	S32							reserved_count;
	reserved_descriptor_t		reserved_descriptor[MAX_RESERVED_DESCRIPTORS];

	S32							N;
	ES_DESCRIPTION_t			astESMap[MAX_ESS_PER_PMT_SECTION];

	U32							CRC_32;										//32
	U32							CRC_32_verify;								//32

} TS_program_map_section_t, *pTS_program_map_section_t;

_CDL_EXPORT	int MPEG2_PSI_PMT_DecodeSection(uint8_t *section_buf, const int section_length, TS_program_map_section_t* pPMTSection);

/*------------------------------------------------------------
		CAT definition 
-------------------------------------------------------------*/
typedef struct CA_section_s
{
	U8		table_id;									//8

	U8		section_syntax_indicator;					//1
	U8		reserved_future_use;						//1
	U8		reserved0;									//2
	U16		section_length;								//12

	U32		reserved1;									//18
	U8		version_number;								//5
	U8		current_next_indicator;						//1

	U8		section_number;								//8
	U8		last_section_number;						//8

	S32							reserved_count;
	reserved_descriptor_t		reserved_descriptor[MAX_RESERVED_DESCRIPTORS];

	U32					CRC_32;							//32
	U32					CRC_32_verify;								//32

} CA_section_t, *pCA_section_t;

_CDL_EXPORT	int	MPEG2_PSI_CAT_DecodeSection(uint8_t *buf, int length, CA_section_t* pCATSection);


/*------------------------------------------------------------
	TSDT 
-------------------------------------------------------------*/
typedef struct TS_description_section_s
{
	U8		table_id;									//8

	U8		section_syntax_indicator;					//1
	U8		reserved_future_use;						//1
	U8		reserved0;									//2
	U16		section_length;								//12

	U32		reserved1;									//18
	U8		version_number;								//5
	U8		current_next_indicator;						//1

	U8		section_number;								//8
	U8		last_section_number;						//8

	S32							reserved_count;
	reserved_descriptor_t		reserved_descriptor[MAX_RESERVED_DESCRIPTORS];

	U32					CRC_32;							//32
	U32					CRC_32_verify;								//32

} TS_description_section_t, *pTS_description_section_t;

_CDL_EXPORT int MPEG2_PSI_TSDT_DecodeSection(uint8_t *buf, int length, TS_description_section_t* pTSDTSection);



#endif

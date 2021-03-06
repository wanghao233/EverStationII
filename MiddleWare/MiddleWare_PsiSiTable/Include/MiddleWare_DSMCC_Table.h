#ifndef _CDL_DSMCC_TABLE_H_
#define _CDL_DSMCC_TABLE_H_

#include "../compile.h"
#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\MPEG_DVB_Common.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_DSMCC_Descriptor.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_DSMCC_Section.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_DSMCC_Section_UNM.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/Include/Mpeg2_DSMCC_Section_DDM.h"

#include "MiddleWare_PSISI_Common.h"

#define MAX_DOWNLOAD_OBJS			256
/*------------------------------------------------------------
		AIT definition 
-------------------------------------------------------------*/
class _CDL_EXPORT CAIT : public CPVT
{
public:
	CAIT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CAIT(void);
public:

	//------------------semantic part------------------------
	U16						m_usApplicationType;

	//------------------syntax part---------------------------
public:

	void Init(void);
	void Reset(void);
	int	 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);
};

class _CDL_EXPORT CDSMCC_UNM : public CPVT
{
public:
	CDSMCC_UNM(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CDSMCC_UNM(void);

protected:
	uint16_t					m_usMessageId;

public:
	union
	{
		DSMCC_DSI_t					m_DSI;			//两者只能有其一，由messageId决定
		DSMCC_DII_t					m_DII;
	} u;

	int							m_nMemoryForModuleInfos;
	int							m_nMemoryForGroupInfos;

public:
	void Init(void);
	void Reset(void);
	int	 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);

	uint16_t GetMessageID(void);
};

class _CDL_EXPORT CDSMCC_DDM : public CPVT
{
public:
	CDSMCC_DDM(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CDSMCC_DDM(void);

protected:
	struct
	{
		uint8_t* buf;
		int		 length;
	} m_astBlockInfo[256];		//block_count 等同于section_count

	int						m_nMemAllocatedForModule;

	int						m_nModuleSize;
	uint16_t				m_usMessageId;

public:

	int							m_nDirMessageCount;
	DirectoryMessage_t*			m_pDirectoryMessage[MAX_DOWNLOAD_OBJS];					//OC
	int							m_nMemAllocatedForDirMessages;

	S32							m_nFileMessageCount;
	FileMessage_t*				m_pFileMessage[MAX_DOWNLOAD_OBJS];						//OC
	S32							m_nMemAllocatedForFileMessages;

public:
	void Init(void);
	void Reset(void);
	int	 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);
};

//MPE datagram
class _CDL_EXPORT CMPE : public CPVT
{
public:
	CMPE(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CMPE(void);
public:

/*------------------semantic part---------------------------*/
/*------------------syntax part---------------------------*/

public:
	void Init(void);
	void Reset(void);
	int	 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);
};

/*------------------------------------------------------------
		INT definition 
-------------------------------------------------------------*/
class _CDL_EXPORT CINT : public CPVT
{
public:
	CINT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CINT(void);
public:

/*------------------semantic part---------------------------*/
	uint32_t		m_uiPlatformID;

/*------------------syntax part---------------------------*/

public:
	void Init(void);
	void Reset(void);
	int	 AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section);
};

#endif

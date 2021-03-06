#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/Mpeg2_PSI_section.h"
#include "../Include/Mpeg2_PSI_Descriptor.h"
#include "../Include/DVB_SI_Descriptor.h"
//#include "../Include/Mpeg2_DSMCC_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"
#include "../Include/Mpeg2_StreamType.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

#include "libs_Math/Include/CRC_32.h"

////////////////////////////////////////////
int MPEG2_PSI_PMT_DecodeSection(uint8_t *section_buf, int section_size, TS_program_map_section_t* ppmt_section)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;
	U8*	 pl1temp;
	U8*	 pl2temp;
	S32	 N;
	S32  descriptor_loop_length;
	S32  es_loop_length;
	U16	 descriptor_tag;
	U8	 descriptor_length;
	U16	 move_length;
	S32	 CA_count = 0;
	S32  language_count = 0;
	S32	 descriptor_count = 0;

	BITS_t bs;

	ES_DESCRIPTION_t*	pESMap;

	if ((section_buf != NULL) && (section_size >= MPEG2_PSI_PMT_SECTION_MIN_SIZE) && (section_size <= MPEG2_PSI_PMT_SECTION_MAX_SIZE) && (ppmt_section != NULL))
	{
		memset(ppmt_section, 0x00, sizeof(TS_program_map_section_t));

		if (ppmt_section->CRC_32_verify == 0x00000000)
		{
			ppmt_section->CRC_32_verify = Encode_CRC_32(section_buf, section_size - 4);
		}
		ppmt_section->CRC_32 = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

		//if (ppmt_section->CRC_32_verify == ppmt_section->CRC_32)
		{
			BITS_map(&bs, section_buf, section_size);

			ppmt_section->table_id = BITS_get(&bs, 8);
			assert(ppmt_section->table_id == TABLE_ID_PMT);			//故意抓虫子

			ppmt_section->section_syntax_indicator = BITS_get(&bs, 1);
			ppmt_section->reserved_future_use = BITS_get(&bs, 1);
			assert(ppmt_section->reserved_future_use == 0);
			ppmt_section->reserved0 = BITS_get(&bs, 2);
			assert(ppmt_section->reserved0 == 0b11);

			ppmt_section->section_length = BITS_get(&bs, 12);

			if ((ppmt_section->section_length >= (MPEG2_PSI_PMT_SECTION_MIN_SIZE - 3)) && (ppmt_section->section_length <= (MPEG2_PSI_PMT_SECTION_MAX_SIZE - 3)))		//PMT
			{
				ppmt_section->program_number = BITS_get(&bs, 16);

				ppmt_section->reserved1 = BITS_get(&bs, 2);
				ppmt_section->version_number = BITS_get(&bs, 5);
				ppmt_section->current_next_indicator = BITS_get(&bs, 1);

				ppmt_section->section_number = BITS_get(&bs, 8);
				ppmt_section->last_section_number = BITS_get(&bs, 8);

				if (ppmt_section->section_number <= ppmt_section->last_section_number)			//
				{
					ppmt_section->reserved2 = BITS_get(&bs, 3);
					ppmt_section->PCR_PID = BITS_get(&bs, 13);

					ppmt_section->reserved3 = BITS_get(&bs, 4);
					ppmt_section->program_info_length = BITS_get(&bs, 12);

					descriptor_count = 0;
					if (ppmt_section->program_info_length > 0)  //PMT
					{
						pl1temp = bs.p_cur;
						BITS_byteSkip(&bs, ppmt_section->program_info_length);

						if (ppmt_section->program_info_length <= (ppmt_section->section_length - 13))
						{
							descriptor_loop_length = ppmt_section->program_info_length;
							while ((descriptor_loop_length >= 2) && (descriptor_count < MAX_RESERVED_DESCRIPTORS))
							{
								descriptor_tag = pl1temp[0];
								descriptor_length = pl1temp[1];
								move_length = descriptor_length + 2;

								assert(descriptor_count < MAX_RESERVED_DESCRIPTORS);

								ppmt_section->reserved_descriptor[descriptor_count].descriptor_tag = descriptor_tag;
								ppmt_section->reserved_descriptor[descriptor_count].descriptor_length = descriptor_length;
								ppmt_section->reserved_descriptor[descriptor_count].descriptor_buf = pl1temp;
								ppmt_section->reserved_descriptor[descriptor_count].descriptor_size = (uint8_t)move_length;

								descriptor_count++;

								pl1temp += move_length;
								descriptor_loop_length -= move_length;
							}
						}
					}
					ppmt_section->reserved_count = descriptor_count;

					es_loop_length = ppmt_section->section_length - ppmt_section->program_info_length - 13;
					N = 0;
					if (es_loop_length > 0)
					{
						uint8_t* es_loop_start_ptr = bs.p_cur;
						BITS_byteSkip(&bs, es_loop_length);

						BITS_t es_loop_bs;
						BITS_map(&es_loop_bs, es_loop_start_ptr, es_loop_length);

						while ((es_loop_length >= 5) && (N < MAX_ESS_PER_PMT_SECTION))			//为什么是16？ chendelin  2018/5/19
						{
							pESMap = ppmt_section->astESMap + N;

							pESMap->stream_subtype = STREAM_SUBTYPE_UNKNOWN;		//chendelin  20110423

							pESMap->stream_type = BITS_get(&es_loop_bs, 8);

							pESMap->reserved0 = BITS_get(&es_loop_bs, 3);
							pESMap->elementary_PID = BITS_get(&es_loop_bs, 13);

							pESMap->reserved1 = BITS_get(&es_loop_bs, 4);
							pESMap->ES_info_length = BITS_get(&es_loop_bs, 12);

							descriptor_count = 0;
							if (pESMap->ES_info_length > 0)
							{
								pl2temp = es_loop_bs.p_cur;
								BITS_byteSkip(&es_loop_bs, pESMap->ES_info_length);

								if (pESMap->ES_info_length <= (es_loop_length - 5))
								{
									descriptor_loop_length = pESMap->ES_info_length;
									while ((descriptor_loop_length >= 2) && (descriptor_count < MAX_RESERVED_DESCRIPTORS))
									{
										descriptor_tag = pl2temp[0];
										descriptor_length = pl2temp[1];
										move_length = descriptor_length + 2;

										pESMap->reserved_descriptor[descriptor_count].descriptor_tag = descriptor_tag;
										pESMap->reserved_descriptor[descriptor_count].descriptor_length = descriptor_length;
										pESMap->reserved_descriptor[descriptor_count].descriptor_buf = pl2temp;
										pESMap->reserved_descriptor[descriptor_count].descriptor_size = (uint8_t)move_length;

										registration_descriptor_t registration_descriptor;
										switch (descriptor_tag)
										{
										case MPEG2_PSI_REGISTRATION_DESCRIPTOR:
											MPEG2_PSI_decode_registration_descriptor(pl2temp, move_length, &registration_descriptor);
											break;
										}

										descriptor_count++;

										if (pESMap->stream_type == 0x05)
										{
											if (pESMap->stream_subtype == STREAM_SUBTYPE_UNKNOWN)
											{
												switch (descriptor_tag)
												{
												case DVB_SI_APPLICATION_SIGNALLING_DESCRIPTOR:
													pESMap->stream_subtype = STREAM_SUBTYPE_AIT;
													break;
												default:
													break;
												}
											}
										}
										else if (pESMap->stream_type == 0x06)
										{
											if (pESMap->stream_subtype == STREAM_SUBTYPE_UNKNOWN)
											{
												switch (descriptor_tag)
												{
												case MPEG2_PSI_REGISTRATION_DESCRIPTOR:
													if (strcmp(registration_descriptor.format_identifier_char, "DRA1") == 0)
													{
														pESMap->stream_subtype = STREAM_SUBTYPE_DRA;
													}
													break;
												case DVB_SI_TELETEXT_DESCRIPTOR:
													pESMap->stream_subtype = STREAM_SUBTYPE_TELETEXT;
													break;
												case DVB_SI_SUBTITLING_DESCRIPTOR:
													pESMap->stream_subtype = STREAM_SUBTYPE_SUBTITLE;
													break;
												case DVB_SI_AC3_DESCRIPTOR:
													pESMap->stream_subtype = STREAM_SUBTYPE_AC3;
													break;
												default:
													break;
												}
											}
										}
										else if (pESMap->stream_type == 0x81)				//added by chendelin		2018.4.28
										{
											if (pESMap->stream_subtype == STREAM_SUBTYPE_UNKNOWN)
											{
												switch (descriptor_tag)
												{
												case MPEG2_PSI_REGISTRATION_DESCRIPTOR:
													if (strcmp(registration_descriptor.format_identifier_char, "AC-3") == 0)
													{
														pESMap->stream_subtype = STREAM_SUBTYPE_AC3;
													}

													break;
												case DVB_SI_AC3_DESCRIPTOR:
												case DVB_SI_AC3_AUDIO_DESCRIPTOR:
													pESMap->stream_subtype = STREAM_SUBTYPE_AC3;
													break;
												default:
													break;
												}
											}
										}

										pl2temp += move_length;
										descriptor_loop_length -= move_length;
									}
								}
							}

							pESMap->reserved_count = descriptor_count;

							es_loop_length -= (pESMap->ES_info_length + 5);
							N++;
						}
					}
					ppmt_section->N = N;
				}
				else
				{
					//rtcode = SECTION_PARSE_SYNTAX_ERROR;
				}
			}
			else
			{
				//rtcode = SECTION_PARSE_SYNTAX_ERROR;
			}
		}

		//if (ppmt_section->CRC_32_verify != ppmt_section->CRC_32)
		//{
		//	rtcode = SECTION_PARSE_CRC_ERROR;
		//}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}



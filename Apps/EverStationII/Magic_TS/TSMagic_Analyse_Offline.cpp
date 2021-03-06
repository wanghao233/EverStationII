#include "stdafx.h"

#include "..\Common\define.h"

#include "TSMagic_Callbacks_From_Lib.h"
#include "TSMagic_Callbacks_To_Gui.h"

#include "TSMagic_GuiApi.h"
#include "TSMagic_GuiApi_MSG.h"
#include "TSMagic_ErrorCode.h"

#include "TSMagic_Trigger.h"
#include "TSMagic_Trigger_Section.h"

#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_Pcrs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiTables.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/Mpeg2_SectionSplicer.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/Mpeg2_PESSplicer.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/MiddleWare_SectionSplicer_ErrorCode.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities.h"
#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TS_ErrorCode.h"
#include "MiddleWare/MiddleWare_PsiSiTable\Include\MiddleWare_PSISI_ErrorCode.h"
#include "MiddleWare/MiddleWare_ESDecoder/Include/ESDecoder.h"

#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/MPEG_DVB_ErrorCode.h"
#include "libs_MPEG&DVB/MPEG_TSPacket\Include\Mpeg2_TS_Utilities.h"

//#include "MiddleWare_ESDecoder/Include/VideoDecoder_Mpeg2.h"
//#include "MiddleWare_ESDecoder/Include/VideoDecoder_H264.h"
//#include "MiddleWare_ESDecoder/Include/VideoDecoder_AVS.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_Mpeg2.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_AC3.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_AAC.h"

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <assert.h>

#define AUDIO_ES_READ_SIZE				4096						//16 * 1024
#define VIDEO_ES_READ_SIZE				4096						//32 * 1024

#define MAX_SECTION_FILTERS				16

void offline_ts_loop(pthread_params_t pThreadParams)
{
	U8*	  section_buf;
	S32	  section_length;
	
	U8	  packet_buf[204];
	S32	  packet_length;

//	S32	  old_pklength = -1;
	S8	  pszDebug[MAX_TXT_CHARS];
	S8	  pszTemp[128];
	//char  pszCheckFile[128];
	FILE* fpCheckFile = NULL;

	double file_size_div_100;
	S64	  read_byte_pos = 0;			//只能读小于2G的文件
	S64	  old_read_byte_pos = 0;

	S32	  old_ratio = 0;
	S32	  analyse_ratio = 0;
	U32	  old_tickcount;
	U32	  new_tickcount;
	S32	  diff_tickcount;

	S32	  stream_synced = 0;
//	S32	  do_next_splice;

	S32					rtcode;
//	S32					getdata_rtcode;
//	S32					nErrCode = FILE_ERROR_NO_ERROR;
	S32					filter_index;
	CSectionSplicer		SectionSplicer[MAX_SECTION_FILTERS];
	CSectionSplicer*	pSectionSplicer;
	CPESSplicer			PESSplicer;

	CTransportStream*	ptransport_stream = NULL;
	transport_packet_t	transport_packet;

	RECORD_TSPacket_t		TSPacketInfo;
	CDB_TSPackets*			pDB_TSPackets = NULL;
	CDB_Pcrs*				pDB_Pcrs = NULL;
	CDB_PsiSiTables*		pDB_PsiSiTables = NULL;

	CTrigger_PsiSiSection* pSectionTrigger = NULL;
	CTrigger_TSPacket*	   pTSPacketTrigger = NULL;
	CTrigger_PESPacket*	   pPESPacketTrigger = NULL;


	S32	hLastRecordHandler = -1;

	if (pThreadParams != NULL)
	{
		assert(pThreadParams->main_thread_stopped == 0);
		pThreadParams->main_thread_running = 1;

		old_tickcount = ::GetTickCount();

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_OFFLINE_THREAD, 1, NULL);

		sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: 开始(文件名：%s)", pThreadParams->pszFileName);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);

		ptransport_stream = pThreadParams->pTStream;
		ptransport_stream->Reset();

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_REPORT_FILESIZE, (WPARAM)(&(ptransport_stream->m_llTotalFileLength)), (LPARAM)((char*)pThreadParams->pszFileName));
		pTSPacketTrigger = pThreadParams->pTrigger_TSPacket;
		pTSPacketTrigger->Reset();

		pPESPacketTrigger = pThreadParams->pTrigger_PESPacket;
		pPESPacketTrigger->Reset();

		pSectionTrigger = pThreadParams->pTrigger_Section;
		pSectionTrigger->Reset();

		pDB_TSPackets = pThreadParams->pDB_TSPackets;
		pDB_TSPackets->Reset();
		pDB_Pcrs = pThreadParams->pDB_Pcrs;
		pDB_Pcrs->Reset();

		pDB_PsiSiTables = pThreadParams->pDB_PsiSiTables;
		pDB_PsiSiTables->Reset();

		ptransport_stream->SeekToBegin();
		ptransport_stream->StartGetData();			//启动接收任务


		file_size_div_100 = ptransport_stream->m_llTotalFileLength / 100.0;

//		sprintf_s(pszCheckFile, sizeof(pszCheckFile), "e:\\temp\\check.ts");
//		fpCheckFile = fopen(pszCheckFile, "wb");

		while (pThreadParams->main_thread_running == 1)
		{
			packet_length = sizeof(packet_buf);
			rtcode = ptransport_stream->PrefetchOnePacket(packet_buf, &packet_length);
			if (rtcode == MIDDLEWARE_TS_NO_ERROR)
			{
//				if (fpCheckFile != NULL)
//				{
//					fwrite(packet_buf, sizeof(char), packet_length, fpCheckFile);
//				}
//				old_read_byte_pos = read_byte_pos;
				read_byte_pos = ptransport_stream->Tell();			//因为是预读一个包，实际FIFO读指针并未发生移动

				//向界面汇报文件分析进度
				if (ptransport_stream->m_llTotalFileLength > 0)
				{
					analyse_ratio = (U32)(read_byte_pos / file_size_div_100);
					if (analyse_ratio > old_ratio)
					{
						old_ratio = analyse_ratio;
						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_REPORT_RATIO, (WPARAM)NULL, (LPARAM)analyse_ratio);

#if OPEN_PACKET_STATISTIC
						if (pDB_TSPackets->callback_gui_update != NULL)
						{
							pDB_TSPackets->callback_gui_update((int)ptransport_stream->GetBitrate(), NULL);
						}
#endif
					}
				}

				//汇报同步状态
				if (stream_synced == 0)
				{
					stream_synced = 1;

					sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: 找到同步（文件位置：0x%llx）\n", read_byte_pos);
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
				}

				//汇报TS包长度
				if (packet_length != pDB_TSPackets->m_packet_length)
				{
					pDB_TSPackets->m_packet_length = packet_length;
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_REPORT_PACKET_LENGTH, (WPARAM)packet_length, NULL);

					sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: TS包长 = %d\n", packet_length);
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
				}

				//对该TS包进行语法分析
				rtcode = MPEG_decode_TS_packet(packet_buf, packet_length, &transport_packet);
				if (rtcode == NO_ERROR)
				{
#if OPEN_TS_PACKET_TRIGGER					
					//TS packet trigger
					if (pTSPacketTrigger->IsOpened() && (!pTSPacketTrigger->IsFull()))
					{
						if (pTSPacketTrigger->IsMatched(packet_buf, packet_length))
						{
//							S32 nOldCatchedCount = pTSPacketTrigger->GetCatchedCount();
							pTSPacketTrigger->SaveTheWholePacket(packet_buf, packet_length);
//							if (nOldCatchedCount == 0)		//捕捉到第一个匹配TS包时报告状态
							{
								::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_TS_TRIGGER_STATE, 0, 0);
							}
						}
					}
#endif

#if OPEN_PES_PACKET_TRIGGER					
					//PES packet trigger
					if (pPESPacketTrigger->IsOpened() && (!pPESPacketTrigger->IsFull()))
					{
						if (transport_packet.PID == pPESPacketTrigger->GetPID())
						{
							//TS_PID与待捕捉的PES包一致
							rtcode = PESSplicer.WriteTSPacket(&transport_packet);
							if (rtcode == NO_ERROR)
							{
								S32 pes_length;
								U8* pes_buf = PESSplicer.GetPESPacket(&pes_length);
								pPESPacketTrigger->SaveTheWholePacket(pes_buf, pes_length);
								::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_PES_TRIGGER_STATE, 2, 0);
								::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_PES_TRIGGER_STATE, 0, 0);

								PESSplicer.Reset();
							}
						}
					}
#endif

#if OPEN_PACKET_STATISTIC
					rtcode = pDB_TSPackets->AddPacket(&transport_packet);
					if (rtcode != NO_ERROR)
					{
						//如果解析有错误，汇报错误状态
						if (rtcode == ETR290_CONTINUITY_COUNT_ERROR)
						{
							sprintf_s(pszDebug, sizeof(pszDebug), "离线分析:TS包统计——连续计数错误（文件位置：0x%llx, PID = 0x%04x）\n", read_byte_pos, transport_packet.PID);
							::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
						}
						else
						{
							TSMagic_ErrorCodeLookup(rtcode, pszTemp, sizeof(pszTemp));
							sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: TS包统计——%s（文件位置：0x%llx）\n", pszTemp, read_byte_pos);

							::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
						}
					}
#endif					

					//pTS_packet_info = pDB_TSPackets->GetRecord(transport_packet.PID);
					if (pDB_TSPackets->GetRecord(transport_packet.PID, &TSPacketInfo) == NO_ERROR)
					{
						if (TSPacketInfo.ucClass == TSPAYLOAD_CLASS_SECTION)
						{
#if OPEN_SECTION_SPLICER
							//首先寻找是否有可用的过滤器
							pSectionSplicer = NULL;
							for (filter_index = 0; filter_index < MAX_SECTION_FILTERS; filter_index++)
							{
								if (SectionSplicer[filter_index].m_usPID == transport_packet.PID)
								{
									pSectionSplicer = SectionSplicer + filter_index;
									break;
								}
							}

							if (filter_index == MAX_SECTION_FILTERS)
							{
								//说明没有找到

								//寻找第一个可用的FILTER
								for (filter_index = 0; filter_index < MAX_SECTION_FILTERS; filter_index++)
								{
									if (SectionSplicer[filter_index].m_usPID == 0xFFFF)
									{
										pSectionSplicer = SectionSplicer + filter_index;
										break;
									}
								}

								if (filter_index == MAX_SECTION_FILTERS)
								{
									sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: section拼接——超出section_filter的最大数量!");
									::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
								}
							}

							if (pSectionSplicer != NULL)
							{
								do
								{
									rtcode = pSectionSplicer->WriteTSPacket(&transport_packet);
									if ((rtcode == NO_ERROR) || (rtcode == SECTION_SPLICE_ONE_MORE_SECTIONS))
									{
										//如果SECTION拼接成功，则进行section分析

										section_buf = pSectionSplicer->m_pucSectionBuf;
										section_length = pSectionSplicer->m_nSectionLength;

										rtcode = REPORT_PSISI_section(pDB_PsiSiTables, pThreadParams->hMainWnd, pSectionSplicer->m_usPID, section_buf, section_length);
										if (rtcode != TSMAGIC_NO_ERROR)
										{
											if (rtcode == TSMAGIC_NOT_SUPPORTED_TABLE)
											{
												sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: section报告——在TS_PID=0x%04X上发现未识别的PSI/SI表(table_id=0x%02x)（文件位置：0x%llx）\n", pSectionSplicer->m_usPID, pSectionSplicer->m_ucTableID, read_byte_pos);

												::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_WARNING);
											}
											else if (rtcode == MIDDLEWARE_PSISI_VERSION_CHANGE)
											{
												sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: section报告——PSI/SI表(TS_PID=0x%04X, table_id=0x%02x)版本发生变更（文件位置：0x%llx）\n", pSectionSplicer->m_usPID, section_buf[0], read_byte_pos);

												::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_WARNING);
											}
											else if (rtcode == MIDDLEWARE_PSISI_DUPLICATED_SECTION)
											{
	//											sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: section报告——PSI/SI表重复的段(TS_PID=0x%04X, table_id=0x%02X)版本发生变更（文件位置：0x%llx）\n", pSectionSplicer->m_usPID, section_buf[0], read_byte_pos);
	//
	//											::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_WARNING);
											}
											else
											{
												TSMagic_ErrorCodeLookup(rtcode, pszTemp, sizeof(pszTemp));
												sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: section报告——PSI/SI表(TS_PID=0x%04X, table_id=0x%02X) %s（文件位置：0x%llx）\n",  pSectionSplicer->m_usPID, section_buf[0], pszTemp, read_byte_pos);

												::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
											}
										}
#if OPEN_SECTION_TRIGGER
										//PSISI section trigger
										if (pSectionTrigger->IsOpened() && (!pSectionTrigger->IsFull()))
										{
											if (pSectionTrigger->IsMatched(section_buf, section_length))
											{
												S32 nOldCatchedCount = pSectionTrigger->GetCatchedCount();
												pSectionTrigger->SaveTheWholePacket(section_buf, section_length);

												if (nOldCatchedCount == 0)		//捕捉到第一个匹配section时报告状态
												{
													::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_SECTION_TRIGGER_STATE, 0, 0);
												}
											}
										}
#endif
										//完成使命，释放当前section拼接器资源
										pSectionSplicer->Reset();
									}
									else if (rtcode == SECTION_SPLICE_NOT_COMPLETE)
									{
										//正常现象，不需要报错
									}
									else if (rtcode == SECTION_SPLICE_NOT_SYNC)
									{
										//正常现象，不需要报错
									}
									else if (rtcode == SECTION_SPLICE_PADDING_BYTES)
									{
										//正常现象，不需要报错
									}
									else if (rtcode == SECTION_SPLICE_TRANSPORT_ERROR)
									{
										sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: section拼接——TS包(TS_PID=0x%04X)包头指示传输错误（文件位置：0x%llx）\n", transport_packet.PID, read_byte_pos);
										::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_WARNING);
									}
									else if (rtcode == SECTION_SPLICE_CONTINUITY_ERROR)
									{
										sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: section拼接——TS包(TS_PID=0x%04X)序号不连续（文件位置：0x%llx）\n", transport_packet.PID, read_byte_pos);
										::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
									}
									else
									{
										TSMagic_ErrorCodeLookup(rtcode, pszTemp, sizeof(pszTemp));
										sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: section拼接——%s（TS_PID=0x%04X, 文件位置：0x%llx）\n", pszTemp, transport_packet.PID, read_byte_pos);
										::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
									}

								} while (rtcode == SECTION_SPLICE_ONE_MORE_SECTIONS);
							}
#endif
						}

					}

					if (fpCheckFile != NULL)
					{
						fwrite(packet_buf, sizeof(char), packet_length, fpCheckFile);
					}
#if OPEN_PCR_ANALYZER
					//离线分析情况下的PCR检测
					if (transport_packet.adaptation_field.PCR_flag)
					{
						PCR_code_t pcr_code;
						pcr_code.base_32_30 = transport_packet.adaptation_field.program_clock_reference_base_32_30;
						pcr_code.base_29_15 = transport_packet.adaptation_field.program_clock_reference_base_29_15;
						pcr_code.base_14_0 = transport_packet.adaptation_field.program_clock_reference_base_14_0;
						pcr_code.extension = transport_packet.adaptation_field.program_clock_reference_extension;

						BITRATE_ATTRIBUTE_t bitrate_attr;
						ptransport_stream->GetMeasuredBitrateAttribute(&bitrate_attr);

						rtcode = pDB_Pcrs->AddPCRSample(transport_packet.PID, read_byte_pos, &pcr_code, bitrate_attr.mean, bitrate_attr.rms);
						if (rtcode == NO_ERROR)
						{
							RECORD_PCR_t PCRRecord;
							//采用获取副本的方式，防止子程序对原始数据进行修改
							pDB_Pcrs->GetRecordByPID(transport_packet.PID, &PCRRecord);

							CALLBACK_REPORT_PCR_Record(&PCRRecord);

							int nID = PCRRecord.PCR_PID;

							if (PCRRecord.interval_available && PCRRecord.jitter_available)
							{
								PCR_INTERVAL_ATTRIBUTE_t interval_attr;
								PCR_JITTER_ATTRIBUTE_t jitter_attr;
								pDB_Pcrs->GetMeasuredIntervalAttribute(&interval_attr);
								pDB_Pcrs->GetMeasuredJitterAttribute(&jitter_attr);

								CALLBACK_REPORT_PCR_Attribute(nID, PCRRecord.interval_cur_value, PCRRecord.jitter_cur_value, &interval_attr, &jitter_attr);
							}

							//这里仅是一种码流速率的估算方法,实际上是不可以自己证明自己的，应该通过其他方式计算码率
							{
								S64 sum = 0;
								int count = 0;
								int pcr_count = pDB_Pcrs->GetTotalRecordCount();

								for (int i = 0; i < pcr_count; i++)
								{
									if (pDB_Pcrs->GetRecordByIndex(i, &PCRRecord) == NO_ERROR)
									{
										if (PCRRecord.encoder_bitrate_available)
										{
											sum += PCRRecord.encoder_bitrate_mean_value;
											count++;
										}
									}
								}
								if (count > 0)
								{
									int bitrate_cur = (S32)(sum / count);

									ptransport_stream->AddBitrateSample(bitrate_cur);

									ptransport_stream->GetMeasuredBitrateAttribute(&bitrate_attr);
									CALLBACK_REPORT_bitrates(bitrate_cur, &bitrate_attr);

									pDB_TSPackets->m_total_bitrate_from_software = bitrate_cur;
								}
							}
						}
					}
#endif
				}
				else
				{
					//解析TS包语法发现错误，为什么发生错误？？？？
					TSMagic_ErrorCodeLookup(rtcode, pszTemp, sizeof(pszTemp));
					sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: %s（文件位置：0x%llx）\n", pszTemp, read_byte_pos);

					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
				}

				ptransport_stream->SkipOnePacket();
			}
			else if (rtcode == ETR290_TS_SYNC_LOSS)
			{
				//读取TS包发生错误——同步丢失
				stream_synced = 0;

				sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: 丢失同步（文件位置：0x%llx）\n", read_byte_pos);
				::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
			}
			else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
			{
				break;
			}
			else if (rtcode == MIDDLEWARE_TS_FIFO_EMPTY_ERROR)
			{
				Sleep(1);
			}
			else
			{
				//读取TS包发生错误
				TSMagic_ErrorCodeLookup(rtcode, pszTemp, sizeof(pszTemp));
				sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: %s（文件位置：0x%llx）\n", pszTemp, read_byte_pos);

				::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
			}
		}

		ptransport_stream->StopGetData();			//停止接收任务

		if (fpCheckFile != NULL)
		{
			fclose(fpCheckFile);
			fpCheckFile = NULL;
		}

		//检查内存占用情况
		//RSISI_REPORT_check_memory(pThreadParams->hMainWnd);

		//释放sectionsplicer的资源
		for (filter_index = 0; filter_index < MAX_SECTION_FILTERS; filter_index++)
		{
			SectionSplicer[filter_index].Reset();
		}

#if OPEN_PACKET_STATISTIC
		S32		error_count = 0;
		S32		i;
		//RECORD_TSPacket_t* pRecord;

		for (i = 0; i < MAX_PID_COUNT; i++)
		{
			//pRecord = pDB_TSPackets->GetRecord(i);
			if (pDB_TSPackets->GetRecord(i, &TSPacketInfo) == NO_ERROR)
			{
				error_count = TSPacketInfo.continuity_counter_error;
				if (error_count > 0)
				{
					sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: 连续计数错误（PID = 0x%04x, error_count = %d）\n", i, error_count);
					//::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
				}
			}
		}

		if (pDB_TSPackets->callback_gui_update != NULL)
		{
			pDB_TSPackets->callback_gui_update((int)ptransport_stream->GetBitrate(), NULL);
		}
#endif	

		pDB_Pcrs->Reset();

		//重新文件定位
//		ptransport_stream->SeekToBegin();

		if (pThreadParams->main_thread_running == 0)
		{
			//说明是用户强制要求退出主线程，清除界面分析结果
			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_OFFLINE_THREAD, 0, NULL);
		}
		else
		{
			//说明是离线分析正常结束，退出主线程，保留界面分析结果
			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_OFFLINE_THREAD, 2, NULL);			//离线分析结束
		}

		new_tickcount = ::GetTickCount();
		diff_tickcount = new_tickcount - old_tickcount;

		analyse_ratio = 100;
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_REPORT_RATIO, (WPARAM)NULL, (LPARAM)analyse_ratio);

		sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: 结束! (耗时%d ms)", diff_tickcount);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);

		pThreadParams->main_thread_stopped = 1;			//通知应用程序，主线程顺利退出
	}
	else
	{
		sprintf_s(pszDebug, sizeof(pszDebug), "离线分析: 输入参数错误！");
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
	}
}

U32 TSMagic_offline_thread(LPVOID lpParam)
{
	pthread_params_t	pThreadParams = (pthread_params_t)lpParam;

	offline_ts_loop(pThreadParams);

	return 0;
}



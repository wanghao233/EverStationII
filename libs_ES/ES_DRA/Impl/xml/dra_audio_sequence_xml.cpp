#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../Include/dra_audio_sequence.h"
#include "../../Include/dra_audio_errorcode.h"
#include "../../Include/dra_audio_utilities.h"
#include "../../Include/xml/dra_audio_sequence_xml.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

int dra_decode_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
{
	int rtcode = DRA_NO_ERROR;
	char			pszTemp[64];
	int				i;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlUnknownNalNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "unaligned nal()");
		pxmlDoc->UpdateBufMark(pxmlUnknownNalNode, nal_buf, nal_buf + nal_length);
		sprintf_s(pszTemp, sizeof(pszTemp), "%d�ֽ�", nal_length);
		pxmlUnknownNalNode->SetAttribute("comment", pszTemp);

		for (i = 0; i < min(16, nal_length); i++)
		{
			sprintf_s(pszTemp + 3 * i, 4, "%02X ", nal_buf[i]);
		}
		if (nal_length > 16)
		{
			sprintf_s(pszTemp + 3 * i, 5, "...\n");
		}
		tinyxml2::XMLElement* pxmlTempNode = pxmlDoc->NewKeyValuePairElement(pxmlUnknownNalNode, pszTemp);
		pxmlDoc->UpdateBufMark(pxmlTempNode, nal_buf, nal_buf + nal_length);
	}
	else
	{
		rtcode = DRA_PARAMETER_ERROR;
	}

	return rtcode;
}

int dra_decode_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
{
	int rtcode = DRA_NO_ERROR;
	char			pszTemp[64];
	int				i;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlUnknownNalNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "unknown nal()");
		pxmlDoc->UpdateBufMark(pxmlUnknownNalNode, nal_buf, nal_buf + nal_length);
		sprintf_s(pszTemp, sizeof(pszTemp), "%d�ֽ�", nal_length);
		pxmlUnknownNalNode->SetAttribute("comment", pszTemp);

		for (i = 0; i < min(16, nal_length); i++)
		{
			sprintf_s(pszTemp + 3 * i, 4, "%02X ", nal_buf[i]);
		}
		if (nal_length > 16)
		{
			sprintf_s(pszTemp + 3 * i, 5, "...\n");
		}
		tinyxml2::XMLElement* pxmlTempNode = pxmlDoc->NewKeyValuePairElement(pxmlUnknownNalNode, pszTemp);
		pxmlDoc->UpdateBufMark(pxmlTempNode, nal_buf, nal_buf + nal_length);
	}
	else
	{
		rtcode = DRA_PARAMETER_ERROR;
	}

	return rtcode;
}

int	dra_decode_frame_to_xml(uint8_t* es_frame_buf, int es_frame_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, DRA_Frame_t* pDraFrame)
{
	BITS_t	bs;
	int		rtcode = DRA_UNKNOWN_ERROR;
	int     header_size = 4;
	char    pszTemp[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlFrameNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "Frame()");
		pxmlDoc->UpdateBufMark(pxmlFrameNode, es_frame_buf, es_frame_buf + es_frame_length);
		sprintf_s(pszTemp, sizeof(pszTemp), "%d�ֽ�", es_frame_length);
		pxmlFrameNode->SetAttribute("comment", pszTemp);

		if ((es_frame_buf != NULL) && (es_frame_length >= 4))
		{
			DRA_Frame_t* pdra_frame = (pDraFrame == NULL) ? new DRA_Frame_t : pDraFrame;
			memset(pdra_frame, 0x00, sizeof(DRA_Frame_t));
			BITS_map(&bs, es_frame_buf, es_frame_length);

			uint8_t* old_header_ptr = bs.p_cur;
			tinyxml2::XMLElement* pxmlHeaderNode = pxmlDoc->NewKeyValuePairElement(pxmlFrameNode, "FrameHeader()");

			DRA_FrameHeader_t* pFrameHeader = &(pdra_frame->FrameHeader);

			pFrameHeader->nSyncWord = BITS_get(&bs, 16);
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "nSyncWord", pFrameHeader->nSyncWord, 16, "bslbf", NULL, &bs);

			pFrameHeader->nFrmHeaderType = BITS_get(&bs, 1);
			DRA_DecodeFrmHeaderType(pFrameHeader->nFrmHeaderType, pszTemp, sizeof(pszTemp));
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "nFrmHeaderType", pFrameHeader->nFrmHeaderType, 1, "bslbf", pszTemp, &bs);

			if (pFrameHeader->nFrmHeaderType == 0)
			{
				pFrameHeader->nNumWord = BITS_get(&bs, 10);
				sprintf_s(pszTemp, sizeof(pszTemp), "%d�ֽ�/֡", pFrameHeader->nNumWord * 4);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "nNumWord", pFrameHeader->nNumWord, 10, "uimsbf", pszTemp, &bs);
			}
			else
			{
				pFrameHeader->nNumWord = BITS_get(&bs, 13);
				sprintf_s(pszTemp, sizeof(pszTemp), "%d�ֽ�/֡", pFrameHeader->nNumWord * 4);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "nNumWord", pFrameHeader->nNumWord, 13, "uimsbf", pszTemp, &bs);
			}

			pFrameHeader->num_blocks_per_frm_code = BITS_get(&bs, 2);
			pFrameHeader->nNumBlocksPerFrm = (1 << pFrameHeader->num_blocks_per_frm_code);
			sprintf_s(pszTemp, sizeof(pszTemp), "nNumBlockPerFrm=%d", pFrameHeader->nNumBlocksPerFrm);
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "nNumBlocksPerFrm code", pFrameHeader->num_blocks_per_frm_code, 2, "uimsbf", pszTemp, &bs);

			pFrameHeader->nSampleRateIndex = BITS_get(&bs, 4);
			DRA_DecodeSamplingFrequency(pFrameHeader->nSampleRateIndex, pszTemp, sizeof(pszTemp));
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "nSampleRateIndex", pFrameHeader->nSampleRateIndex, 4, "uimsbf", pszTemp, &bs);

			if (pFrameHeader->nFrmHeaderType == 0)
			{
				pFrameHeader->num_normal_ch_code = BITS_get(&bs, 3);
				pFrameHeader->nNumNormalCh = pFrameHeader->num_normal_ch_code + 1;
				sprintf_s(pszTemp, sizeof(pszTemp), "nNumNormalCh=%d", pFrameHeader->nNumNormalCh);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "nNumNormalCh code", pFrameHeader->num_normal_ch_code, 3, "uimsbf", pszTemp, &bs);

				pFrameHeader->nNumLfeCh = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "nNumLfeCh", pFrameHeader->nNumLfeCh, 1, "uimsbf", NULL, &bs);
			}
			else
			{
				pFrameHeader->num_normal_ch_code = BITS_get(&bs, 6);
				pFrameHeader->nNumNormalCh = pFrameHeader->num_normal_ch_code + 1;
				sprintf_s(pszTemp, sizeof(pszTemp), "nNumNormalCh=%d", pFrameHeader->nNumNormalCh);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "nNumNormalCh code", pFrameHeader->num_normal_ch_code, 6, "uimsbf", pszTemp, &bs);

				pFrameHeader->nNumLfeCh = BITS_get(&bs, 2);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "nNumLfeCh", pFrameHeader->nNumLfeCh, 2, "uimsbf", NULL, &bs);
			}

			pFrameHeader->bAuxData = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "bAuxData", pFrameHeader->bAuxData, 1, "bslbf", NULL, &bs);

			pxmlDoc->UpdateBufMark(pxmlHeaderNode, old_header_ptr, bs.p_cur);

			if (pDraFrame == NULL)
			{
				delete pdra_frame;
			}

			rtcode = DRA_NO_ERROR;
		}
		else
		{
			pxmlFrameNode->SetAttribute("error", "parameter error !");
			rtcode = DRA_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = DRA_PARAMETER_ERROR;
	}

	return rtcode;
}

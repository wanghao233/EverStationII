#ifndef __API_MPEG_AUDIO_SEQUENCE_XML_H__
#define __API_MPEG_AUDIO_SEQUENCE_XML_H__

#include <stdint.h>
#include "../../Compile.h"

#include "../mpeg_audio_sequence.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

//public functions
_CDL_EXPORT int mpga_decode_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode);
_CDL_EXPORT int mpga_decode_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode);
_CDL_EXPORT int	mpga_decode_frame_header_to_xml(uint8_t* es_header_buf, int es_header_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, MPA_header_t* pmpa_header = NULL);

_CDL_EXPORT int	mpga_decode_frame_to_xml(uint8_t* es_frame_buf, int es_frame_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, MPA_frame_t* pmpa_frame = NULL);

#endif


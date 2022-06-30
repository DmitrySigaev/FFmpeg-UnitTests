#ifndef _FF_STAT_H_
#define _FF_STAT_H_


#include <stdio.h>
#include <inttypes.h> // int64_t

#include <libavcodec/packet.h>

int ff_side_data_set_encoder_stats(AVPacket* pkt, int32_t quality, int64_t* error, uint8_t error_count, uint8_t pict_type);

int ff_side_data_get_encoder_stats(AVPacket* pkt, int32_t* quality, int64_t* error, uint8_t error_size_check, uint8_t* pict_type);

#endif //_FF_STAT_H_


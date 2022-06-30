#include <stdio.h>
#include <inttypes.h> // int64_t

#include <libavcodec/avcodec.h>
#include <libavcodec/packet.h>
#include <libavutil/intreadwrite.h>

// Temporary typedef to simplify porting all AVBufferRef users to size_t
#if FF_API_BUFFER_SIZE_T
typedef int buffer_size_t;
#else
typedef size_t buffer_size_t;
#endif

# if 0
typedef struct stat_type {
	/* Out: SSIM of the the frame luma (if x264_param_t.b_ssim is set) */
	double f_ssim;
	/* Out: Average PSNR of the frame (if x264_param_t.b_psnr is set) */
	double f_psnr_avg;
	/* Out: PSNR of Y, U, and V (if x264_param_t.b_psnr is set) */
	double f_psnr[3];

	/* Out: Average effective CRF of the encoded frame */
	double f_crf_avg;
} stat_type;

typedef union u_stat_t {
	stat_type stat_out;
	char data[sizeof(stat_type)];
}u_stat_t;

#endif 

int ff_side_data_set_encoder_stats(AVPacket* pkt, int32_t quality, int64_t* error, uint8_t error_count, uint8_t pict_type)
{
	uint8_t* side_data;
	buffer_size_t side_data_size;
	int i;

	side_data = av_packet_get_side_data(pkt, AV_PKT_DATA_QUALITY_STATS, &side_data_size);
	if (!side_data) {
		side_data_size = sizeof(int32_t); // quality (4 bytes)
		side_data_size += sizeof(int32_t);  // sizeof uint8_t error_count, uint8_t pict_type ... plus 2 bytes for future
		side_data_size += (sizeof(int64_t) * (error_count)); //  8 * error_count;
		side_data = av_packet_new_side_data(pkt, AV_PKT_DATA_QUALITY_STATS,
			side_data_size);
	}

	if (!side_data || side_data_size < sizeof(int32_t) + sizeof(int32_t) + (sizeof(int64_t) * (error_count)))
		return AVERROR(ENOMEM);

	AV_WL32(side_data, quality); // [0-3]
	side_data[4] = pict_type;
	side_data[5] = error_count;
	side_data[6] = 0; // for future
	side_data[7] = 0; // for future
	for (i = 0; i < error_count; i++)
		AV_WL64(side_data + sizeof(int32_t) + sizeof(int32_t) + (sizeof(int64_t) * (i)), error[i]);

	return 0;
}

int ff_side_data_get_encoder_stats(AVPacket* pkt, int32_t* quality, int64_t* error, uint8_t error_size_check, uint8_t* pict_type)
{
	uint8_t* side_data;
	buffer_size_t side_data_size;
	int i;

	side_data = av_packet_get_side_data(pkt, AV_PKT_DATA_QUALITY_STATS, &side_data_size);
	if (!side_data) {
		*quality = -1;
		*pict_type = AV_PICTURE_TYPE_NONE;
		return AVERROR_INVALIDDATA;
	}

	*quality = AV_RL32(side_data); // [0-3]
	*pict_type = side_data[4];
	if (NULL != error)
	{
		if (error_size_check != side_data[5])
			return AVERROR_INVALIDDATA;
		for (i = 0; i < side_data[5]; i++)
			error[i] = AV_RL64(side_data + sizeof(int32_t) + sizeof(int32_t) + (sizeof(int64_t) * (i)));
	}
	return 0;
}


#include <libavutil/avassert.h>
#include "internal.h"
#include <libavutil/intreadwrite.h>


int ff_alloc_packet2(AVCodecContext *avctx, AVPacket *avpkt, int64_t size, int64_t min_size)
{
    if (size < 0 || size > INT_MAX - AV_INPUT_BUFFER_PADDING_SIZE) {
        av_log(avctx, AV_LOG_ERROR, "Invalid minimum required packet size %"PRId64" (max allowed is %d)\n",
               size, INT_MAX - AV_INPUT_BUFFER_PADDING_SIZE);
        return AVERROR(EINVAL);
    }

    av_assert0(!avpkt->data);

    if (avctx && 2*min_size < size) { // FIXME The factor needs to be finetuned
        av_fast_padded_malloc(&avctx->internal->byte_buffer, &avctx->internal->byte_buffer_size, size);
        avpkt->data = avctx->internal->byte_buffer;
        avpkt->size = size;
    }

    if (!avpkt->data) {
        int ret = av_new_packet(avpkt, size);
        if (ret < 0)
            av_log(avctx, AV_LOG_ERROR, "Failed to allocate packet of size %"PRId64"\n", size);
        return ret;
    }

    return 0;
}

AVCPBProperties* ff_add_cpb_side_data(AVCodecContext* avctx)
{
    AVPacketSideData* tmp;
    AVCPBProperties* props;
    size_t size;
    int i;

    for (i = 0; i < avctx->nb_coded_side_data; i++)
        if (avctx->coded_side_data[i].type == AV_PKT_DATA_CPB_PROPERTIES)
            return (AVCPBProperties*)avctx->coded_side_data[i].data;

    props = av_cpb_properties_alloc(&size);
    if (!props)
        return NULL;

    tmp = av_realloc_array(avctx->coded_side_data, avctx->nb_coded_side_data + 1, sizeof(*tmp));
    if (!tmp) {
        av_freep(&props);
        return NULL;
    }

    avctx->coded_side_data = tmp;
    avctx->nb_coded_side_data++;

    avctx->coded_side_data[avctx->nb_coded_side_data - 1].type = AV_PKT_DATA_CPB_PROPERTIES;
    avctx->coded_side_data[avctx->nb_coded_side_data - 1].data = (uint8_t*)props;
    avctx->coded_side_data[avctx->nb_coded_side_data - 1].size = size;

    return props;
}

int ff_alloc_a53_sei(const AVFrame* frame, size_t prefix_len,
    void** data, size_t* sei_size)
{
    AVFrameSideData* side_data = NULL;
    uint8_t* sei_data;

    if (frame)
        side_data = av_frame_get_side_data(frame, AV_FRAME_DATA_A53_CC);

    if (!side_data) {
        *data = NULL;
        return 0;
    }

    *sei_size = side_data->size + 11;
    *data = av_mallocz(*sei_size + prefix_len);
    if (!*data)
        return AVERROR(ENOMEM);
    sei_data = (uint8_t*)*data + prefix_len;

    // country code
    sei_data[0] = 181;
    sei_data[1] = 0;
    sei_data[2] = 49;

    /**
     * 'GA94' is standard in North America for ATSC, but hard coding
     * this style may not be the right thing to do -- other formats
     * do exist. This information is not available in the side_data
     * so we are going with this right now.
     */
    AV_WL32(sei_data + 3, MKTAG('G', 'A', '9', '4'));
    sei_data[7] = 3;
    sei_data[8] = ((side_data->size / 3) & 0x1f) | 0x40;
    sei_data[9] = 0;

    memcpy(sei_data + 10, side_data->data, side_data->size);

    sei_data[side_data->size + 10] = 255;

    return 0;
}

int ff_side_data_set_prft(AVPacket* pkt, int64_t timestamp)
{
    AVProducerReferenceTime* prft;
    uint8_t* side_data;
    buffer_size_t side_data_size;

    side_data = av_packet_get_side_data(pkt, AV_PKT_DATA_PRFT, &side_data_size);
    if (!side_data) {
        side_data_size = sizeof(AVProducerReferenceTime);
        side_data = av_packet_new_side_data(pkt, AV_PKT_DATA_PRFT, side_data_size);
    }

    if (!side_data || side_data_size < sizeof(AVProducerReferenceTime))
        return AVERROR(ENOMEM);

    prft = (AVProducerReferenceTime*)side_data;
    prft->wallclock = timestamp;
    prft->flags = 0;

    return 0;
}

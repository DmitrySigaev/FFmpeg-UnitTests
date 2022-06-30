#include <utility>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/packet.h>
#include <libavutil/intreadwrite.h>
#include "ff_stat.h"
}

#include "gtest/gtest.h"
TEST(CloudTrancoder, ff_side_data_set_encoder_stats) {
	AVPacket* packet = av_packet_alloc();
	EXPECT_TRUE(packet != nullptr);
	u_stat_t encode = { 2.2, 3.3, {4.4, 5.5, 6.6}, 7.7 };
	int quality_in = (123) * FF_QP2LAMBDA;
	int type_in = AV_PICTURE_TYPE_B;
	int ret = ff_side_data_set_encoder_stats(packet, quality_in, (int64_t*)&encode, (sizeof(u_stat_t) / sizeof(int64_t)), type_in);
	EXPECT_EQ(ret, 0);
#ifdef WIN32
	EXPECT_EQ(sizeof(int), 4);

#else
	EXPECT_EQ(sizeof(int), 8);
#endif 
	EXPECT_EQ(sizeof(int64_t), 8);
	EXPECT_EQ(sizeof(u_stat_t), sizeof(encode));
	EXPECT_EQ(sizeof(u_stat_t), sizeof(stat_type));
	u_stat_t decode;
	int quality_out;
	uint8_t type_out;
	ret = ff_side_data_get_encoder_stats(packet, &quality_out, (int64_t*)&decode, (sizeof(u_stat_t) / sizeof(int64_t)), &type_out);
	EXPECT_EQ(ret, 0);
	EXPECT_EQ(quality_out, quality_in);
	EXPECT_EQ(type_out, type_in);
	for (auto i = 0; auto el : encode.data)
	{
		EXPECT_EQ(decode.data[i++], el);
	}

	av_packet_free(&packet);
}

TEST(CloudTrancoder, ff_side_data_set_encoder_stats_oun_class_vector) {
	AVPacket* packet = av_packet_alloc();
	EXPECT_TRUE(packet != nullptr);
	std::vector encode_of_stat = { 2.2, 3.3, 4.4, 5.5, 6.6, 7.7 };
	int quality_in = (123) * FF_QP2LAMBDA;
	int type_in = AV_PICTURE_TYPE_B;
	int ret = ff_side_data_set_encoder_stats(packet, quality_in, (int64_t*)&encode_of_stat[0], (encode_of_stat.size()*sizeof(decltype(encode_of_stat)::value_type)) / sizeof(int64_t), type_in);
	EXPECT_EQ(ret, 0);
#ifdef WIN32
	EXPECT_EQ(sizeof(int), 4);

#else
	EXPECT_EQ(sizeof(int), 8);
#endif 
	EXPECT_EQ(sizeof(int64_t), 8);
	double dåcode_out[6];
	EXPECT_EQ(8*6, encode_of_stat.size() * sizeof(encode_of_stat[0]));
	EXPECT_EQ(8*6, (encode_of_stat.size() * sizeof(decltype(encode_of_stat)::value_type)));

	EXPECT_EQ(sizeof(dåcode_out), (encode_of_stat.size() * sizeof(decltype(encode_of_stat)::value_type)));
	int quality_out;
	uint8_t type_out;
	ret = ff_side_data_get_encoder_stats(packet, &quality_out, (int64_t*)dåcode_out, (sizeof(dåcode_out) / sizeof(int64_t)), & type_out);
	EXPECT_EQ(ret, 0);
	EXPECT_EQ(quality_out, quality_in);
	EXPECT_EQ(type_out, type_in);
	for (auto i = 0; auto el : encode_of_stat)
	{
		EXPECT_EQ(dåcode_out[i++], el);
	}
	av_packet_free(&packet);
}

TEST(CloudTrancoder, ff_side_data_set_encoder_stats_oun_clase) {
	AVPacket* packet = av_packet_alloc();
	EXPECT_TRUE(packet != nullptr);
	std::vector encode_of_stat = { 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 9.9 };
	int quality_in = (123) * FF_QP2LAMBDA;
	int type_in = AV_PICTURE_TYPE_B;
	int ret = ff_side_data_set_encoder_stats(packet, quality_in, (int64_t*)&encode_of_stat[0], (encode_of_stat.size() * sizeof(decltype(encode_of_stat)::value_type)) / sizeof(int64_t), type_in);
	EXPECT_EQ(ret, 0);
#ifdef WIN32
	EXPECT_EQ(sizeof(int), 4);

#else
	EXPECT_EQ(sizeof(int), 8);
#endif 
	EXPECT_EQ(sizeof(int64_t), 8);
    auto dåcode_out = std::make_shared<double[]>(encode_of_stat.size()); // new double[encode_of_stat.size()] or we allocate array by new double[Size] and make shared pointer 
	EXPECT_EQ(8 * encode_of_stat.size(), encode_of_stat.size() * sizeof(encode_of_stat[0]));
	EXPECT_EQ(8 * encode_of_stat.size(), (encode_of_stat.size() * sizeof(decltype(encode_of_stat)::value_type)));

//	EXPECT_EQ(sizeof(decltype(dåcode_out.get())), (encode_of_stat.size() * sizeof(decltype(encode_of_stat)::value_type)));
	int quality_out;
	uint8_t type_out;
	ret = ff_side_data_get_encoder_stats(packet, &quality_out, (int64_t*)dåcode_out.get(), (encode_of_stat.size() * sizeof(double)) / sizeof(int64_t), &type_out);
	EXPECT_EQ(ret, 0);
	EXPECT_EQ(quality_out, quality_in);
	EXPECT_EQ(type_out, type_in);
	for (auto i = 0; auto el : encode_of_stat)
	{
		EXPECT_EQ(dåcode_out[i++], el);
	}
	av_packet_free(&packet);
}
#include "mythFFmpegEncoder.hh"
#include <stdio.h>
mythFFmpegEncoder::mythFFmpegEncoder(void* phwnd, int width, int height)
{
	mwidth = width;
	mheight = height;
	video_codec = NULL;
	this->c = NULL;
	this->frame = NULL;
	hwnd = phwnd;
	img_convert_ctx = NULL;
	Init();
	FullYUVBuffer = NULL;
	InitFullYUVBuffer();
}

void mythFFmpegEncoder::yuv2RGB(int width, int height,
	const void* ysrc, const void* usrc, const void* vsrc,
	int ysize, int usize, int vsize, void** dst){
	const void* src [] = { ysrc, vsrc, usrc };
	int src_linesize [] = { ysize, vsize, usize };
	yuv2RGB(width, height, (const void**) &src, src_linesize, dst);
	return;
}

void mythFFmpegEncoder::SuperFastRGB2yuv(int width, int height, int stride, const void* src, void** dst)
{
	unsigned int* tmp = (unsigned int*)src;
	unsigned char* yplanar = (unsigned char*) dst[0];
	unsigned char* uplanar = (unsigned char*) dst[1];
	unsigned char* vplanar = (unsigned char*) dst[2];
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			YUVSingle* single = &FullYUVBuffer[*tmp & 0x00ffffff]; tmp++;
			*yplanar = single->YY; yplanar++;
			if (j % 2 == 0){
				if (i % 2 == 0){
					*uplanar = single->UU; uplanar++;
				}
				else{
					*vplanar = single->VV; vplanar++;
				}
			}
		}
	}
}

void mythFFmpegEncoder::InnerRGB2yuv(int width, int height, int stride, const void* src, void** dst){
	if (!img_convert_ctx){
		img_convert_ctx = sws_getContext(
			width, height, PIX_FMT_BGRA,
			width, height, PIX_FMT_YUV420P,
			SWS_FAST_BILINEAR, NULL, NULL, NULL);

	}
	uint8_t *rgb_src[3] = { (uint8_t *) src, NULL, NULL };
	int srcwidth [] = { stride, 0, 0 };
	int dstwidth [] = { width, width / 2, width / 2 };
	if (img_convert_ctx){
		sws_scale(img_convert_ctx, (const uint8_t *const*) rgb_src, srcwidth, 0, height,
			(uint8_t *const*) dst, dstwidth);
	}
	//sws_freeContext(img_convert_ctx);
	return;
}
void mythFFmpegEncoder::RGB2yuv(int width, int height, int stride,const void* src, void** dst){
	struct SwsContext *img_convert_ctx = sws_getContext(
		width , height, PIX_FMT_BGRA,
		width, height, PIX_FMT_YUV420P,
		SWS_FAST_BILINEAR, NULL, NULL, NULL);
	uint8_t *rgb_src[3] = { (uint8_t *) src, NULL, NULL };

	int srcwidth [] = { stride, 0, 0 };
	int dstwidth [] = { width, width / 2, width / 2 };
	//if (img_convert_ctx){
	//	sws_scale(img_convert_ctx, (const uint8_t *const*) rgb_src, srcwidth, 0, height,
	//		(uint8_t *const*) dst, dstwidth);
	//}
	sws_freeContext(img_convert_ctx);
	return;
}
void mythFFmpegEncoder::yuv2RGB(int width, int height, const void** src, int* src_linesize, void** dst){
	struct SwsContext *img_convert_ctx = sws_getContext(
		width, height, PIX_FMT_YUV420P,
		width, height, PIX_FMT_RGBA,
		SWS_FAST_BILINEAR, NULL, NULL, NULL);
	int dstwidth [] = { width * 4, width * 4, width * 4 };
	if (img_convert_ctx){
		sws_scale(img_convert_ctx, (const uint8_t *const*) src, src_linesize, 0, height,
			(uint8_t *const*) dst, dstwidth);
	}
	sws_freeContext(img_convert_ctx);
	return;
}
mythFFmpegEncoder* mythFFmpegEncoder::CreateNew(void* phwnd, int width, int height)
{
	return new mythFFmpegEncoder(phwnd, width, height);
}

void mythFFmpegEncoder::InitFullYUVBuffer()
{
	FullYUVBuffer = new YUVSingle[256 * 256 * 256];
	YUVSingle* p = FullYUVBuffer;
	for (int r = 0; r < 256; r++){
		for (int g = 0; g < 256; g++){
			for (int b = 0; b < 256; b++){
				int y = (int) ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
				int u = (int) ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
				int v = (int) ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
				if (y > 255)y = 255; if (y < 0)y = 0;
				if (u > 255)u = 255; if (u < 0)u = 0;
				if (v > 255)v = 255; if (v < 0)v = 0;
				p->YY = y;
				p->UU = u;
				p->VV = v;
				p++;
			}
		}
	}
	printf("Init YUV Buffer Success!\n");
}

bool mythFFmpegEncoder::Init(){
	//if it has been initialized before, we should do cleanup first
	Cleanup();
	/* Initialize libavcodec, and register all codecs and formats. */
	av_register_all();
	avcodec_register_all();

	video_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!video_codec){
		Cleanup();
		return false;
	}
	c = avcodec_alloc_context3(video_codec);
	AVDictionary *opts = NULL;
	//av_dict_set(&opts, "b", "0.5M", 0);
	c->width = mwidth;
	c->height = mheight;
	//c->bit_rate = 400000;
	c->bit_rate = 2000;
	c->gop_size = 25;
	c->time_base.den = 1;
	c->time_base.num = 25;
	c->pix_fmt = AV_PIX_FMT_YUV420P;
	c->max_b_frames = 0;
	av_opt_set(c->priv_data, "preset", "ultrafast", 0);   //ultrafast,superfast, veryfast, faster, fast, medium, slow, slower, veryslow,placebo.
	av_opt_set(c->priv_data, "profile", "baseline", 0);        //baseline main high
	//av_opt_set(c->priv_data, "level", "4.0", 0);
	av_opt_set(c->priv_data, "tune", "zerolatency", 0); //  tune

	/* open the codec */
	int ret = avcodec_open2(c, video_codec, &opts);
	if (ret < 0) {
		Cleanup();
		return false;
	}

	/* allocate and init a re-usable frame */
	frame = av_frame_alloc();
	if (!frame) {
		Cleanup();
		return false;
	}
	frame->format = c->pix_fmt;
	frame->width = c->width;
	frame->height = c->height;
	av_init_packet(&avpkt);
	return true;
}
mythFFmpegEncoder::~mythFFmpegEncoder(void)
{
	Cleanup();
	if (FullYUVBuffer)
		delete [] FullYUVBuffer;
}
void mythFFmpegEncoder::Cleanup(){
	//if (avpkt != NULL){
	//		av_free_packet(&avpkt);
	//	}//avpkt = NULL;
	if (this->frame != NULL){
		av_free(frame);
		frame = NULL;
	}
	if (this->c != NULL){
		avcodec_close(c);
		av_free(c);
		c = NULL;
	}
}

void mythFFmpegEncoder::ProcessFrame(unsigned char** src, int* srclinesize, responseHandler* response){

	avpkt.data = NULL;
	avpkt.size = 0;
	//frame->pts += 40;
	for (int i = 0; i < 3; i++){
		frame->data[i] = src[i];
		frame->linesize[i] = srclinesize[i];
	}
	if (c == NULL && frame == NULL)return;
	//while (avpkt.size > 0) {
	int got_frame = 0;
	int len = avcodec_encode_video2(c, &avpkt, frame, &got_frame);
	if (len < 0) {
		return;
	}
	if (got_frame){
		//callback
		response(this->hwnd, (char*) avpkt.data, avpkt.size);
	}
	else{
		printf("encode failed\n");
	}
	//}
}

void mythFFmpegEncoder::ProcessFrame(unsigned char** src, int* srclinesize, PacketHandler* response)
{

	avpkt.data = NULL;
	avpkt.size = 0;
	//frame->pts += 40;
	for (int i = 0; i < 3; i++){
		frame->data[i] = src[i];
		frame->linesize[i] = srclinesize[i];
	}
	if (c == NULL && frame == NULL)return;
	//while (avpkt.size > 0) {
	int got_frame = 0;
	int len = avcodec_encode_video2(c, &avpkt, frame, &got_frame);
	if (len < 0) {
		return;
	}
	if (got_frame){
		//callback
		response(this->hwnd, &avpkt);
	}
	else{
		printf("encode failed\n");
	}
	//}
}

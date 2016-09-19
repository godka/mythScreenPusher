#include "mythScreenDecoder.hh"
#include <iostream>
using namespace std;

mythScreenDecoder::mythScreenDecoder()
	:mythVirtualDecoder()
{
	flag = 0;
	encoder = NULL;
	//ptr = RTMPInit("rtmp://localhost/live/stream");
	//Init();
	pts = 0; dts = 0;

}

mythScreenDecoder::~mythScreenDecoder()
{
	if (encoder != NULL){
		encoder->Cleanup();
		delete encoder;
		encoder = NULL;
	}
}

int mythScreenDecoder::InitSrsRTMP(const char* rtmpurl) {
	do {
		rtmp = srs_rtmp_create(rtmpurl);

		if (srs_rtmp_handshake(rtmp) != 0) {
			break;
		}

		if (srs_rtmp_connect_app(rtmp) != 0) {
			break;
		}
		int ret = srs_rtmp_publish_stream(rtmp);
		if (ret != 0) {
			break;
		}
		return 0;
	} while (0);

	return 1;
}
void mythScreenDecoder::start()
{
	startthread = SDL_CreateThread(pushthreadstatic, "decode", this);
	decodethreadstatic(this);
}

int mythScreenDecoder::decodethread()
{
	char* yy = NULL;
	char* uu = NULL;
	char* vv = NULL;
	int got_picture = 0;
	char* tmpsrc [] = { 0, 0, 0 };
	int width = 0, height = 0;
	mythScreenCapture* cap = mythScreenCapture::CreateNew(NULL);
	width = cap->Width();
	height = cap->Height();
	encoder = mythFFmpegEncoder::CreateNew(this,
		width, height);
	yy = new char[width * height];
	uu = new char[width * height / 4];
	vv = new char[width * height / 4];
	tmpsrc[0] = yy;
	tmpsrc[1] = uu;
	tmpsrc[2] = vv;
	int tmplinesize [] = { width, width / 2, width / 2 };
	
	while (flag == 0){
		unsigned int t1 = SDL_GetTicks();
		char* t = cap->capture();
		encoder->SuperFastRGB2yuv(width, height, width * 4, t, (void**) tmpsrc);
		encoder->ProcessFrame((unsigned char**) tmpsrc, tmplinesize,staticresponse);
		unsigned int t2 = SDL_GetTicks();
		unsigned int delay = 40 - (t2 - t1);
		if (delay > 0 && delay <= 40)
			SDL_Delay(delay);
	}
	delete [] yy;
	delete [] uu;
	delete [] vv;
	encoder->Cleanup();
	delete encoder;
	encoder = NULL;
	return 0;
}

int mythScreenDecoder::decodethreadstatic(void* data)
{
	mythScreenDecoder* decoder = (mythScreenDecoder*) data;
	decoder->decodethread();
	return 0;
}

int mythScreenDecoder::pushthreadstatic(void* data)
{

	mythScreenDecoder* decoder = (mythScreenDecoder*) data;
	decoder->pushthread();
	return 0;
}

int mythScreenDecoder::pushthread()
{
	InitSrsRTMP("rtmp://localhost/live/stream");
	int time = SDL_GetTicks();
	for (;;){
		PacketQueue* pkt = get();
		if (pkt){
			int time2 = SDL_GetTicks();
			pts = dts += (time2 - time);
			//printf("Push length:%6d£¬timespan=%6dms\n", pkt->h264PacketLength,(time2 - time));
			time = time2;
			int ret = srs_h264_write_raw_frames(rtmp, (char*) pkt->h264Packet, pkt->h264PacketLength, dts, pts);
			if (ret != 0) {
				if (srs_h264_is_dvbsp_error(ret)) {
					printf("ignore drop video error, code=%d\n", ret);
				}
				else if (srs_h264_is_duplicated_sps_error(ret)) {
					printf("ignore duplicated sps, code=%d\n", ret);
				}
				else if (srs_h264_is_duplicated_pps_error(ret)) {
					printf("ignore duplicated pps, code=%d\n", ret);
				}
				else {
					printf("send h264 raw data failed. code=%d\n", ret);
					break;
				}
			}
		}
		SDL_Delay(1);
	}
	//RTMPStart(ptr);
	return 0;
}

void mythScreenDecoder::stop()
{
	flag = 1;
}

void mythScreenDecoder::staticresponse(void *myth, char* pdata, int plength)
{
	//cout << "process frame callback" << endl;
	mythScreenDecoder* decoder = (mythScreenDecoder*) myth;
	decoder->response(pdata, plength);
}

void mythScreenDecoder::response(char* pdata, int plength)
{
	//printf("Push one frame,size=plength:%d\n",plength);
	//pts = dts += 40;
	//int ret = srs_h264_write_raw_frames(rtmp, (char*) pdata, plength, dts, pts);
	//RTMPPutH264Data(ptr,pdata, plength);
	put((unsigned char*) pdata, (unsigned int) plength);

}

#pragma once
extern "C"{
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#include "libavdevice/avdevice.h"
#include "libavutil/opt.h"       // for av_opt_set  
#include "libavutil/time.h"
#include "libavutil/mathematics.h"
}
/*
#pragma comment( lib, "libgcc.a")  
#pragma comment( lib, "libmingwex.a")  
#pragma comment( lib, "libcoldname.a")  
#pragma comment( lib, "libavcodec.a")  
#pragma comment( lib, "libavformat.a")  
#pragma comment( lib, "libavutil.a")  
#pragma comment( lib, "libswscale.a")  
#pragma comment( lib, "libz.a")  
#pragma comment( lib, "libfaac.a")  
#pragma comment( lib, "libgsm.a")  
#pragma comment( lib, "libmp3lame.a")  
#pragma comment( lib, "libogg.a")  
#pragma comment( lib, "libspeex.a")  
#pragma comment( lib, "libtheora.a")  
#pragma comment( lib, "libvorbis.a")  
#pragma comment( lib, "libvorbisenc.a")  
#pragma comment( lib, "libx264.a")  
#pragma comment( lib, "xvidcore.a")  
#pragma comment( lib, "wsock32.lib")  
#pragma comment( lib, "vfw32.lib")  
*/
//#define SUPERFASTYUV 1
typedef struct YUVSingle
{
	unsigned char YY;
	unsigned char UU;
	unsigned char VV;
	//unsigned char None;
};
class mythFFmpegEncoder
{
public:
	static void RGB2yuv(int width, int height, int stride, const void* src, void** dst);
	typedef void (responseHandler)(void *myth, char* pdata, int plength);
	typedef void (PacketHandler)(void *myth, AVPacket* packet);
	mythFFmpegEncoder(void* phwnd, int width, int height);
	~mythFFmpegEncoder(void);
	bool Init();
	void Cleanup();
	void ProcessFrame(unsigned char** src, int* srclinesize, responseHandler* response);
	void ProcessFrame(unsigned char** src, int* srclinesize, PacketHandler* response);
	void yuv2RGB(int width, int height, const void** src, int* src_linesize, void** dst);
	void yuv2RGB(int width, int height,
		const void* ysrc, const void* usrc, const void* vsrc,
		int ysize, int usize, int vsize, void** dst);

	void SuperFastRGB2yuv(int width, int height, int stride, const void* src, void** dst);
	void InnerRGB2yuv(int width, int height, int stride, const void* src, void** dst);
	AVCodecContext* getC(){
		return c;
	}
	static mythFFmpegEncoder* CreateNew(void* hwnd, int width, int height);//�������
public:
	void* hwnd;
protected:
	//AVCodec* avCodec;
	//AVCodecContext* avCodecContext;
	//AVFrame *avFrame;
	AVCodec *video_codec;
	AVCodecContext *c;
	AVFrame *frame;
	AVPacket avpkt;
	int mwidth;
	int mheight;
private:
	struct SwsContext *img_convert_ctx;
	YUVSingle* FullYUVBuffer;
	void InitFullYUVBuffer();
};


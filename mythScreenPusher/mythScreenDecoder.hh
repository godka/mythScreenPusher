#pragma once
#include "mythconfig.hh"
#include "mythVirtualDecoder.hh"
#include "mythFFmpegEncoder.hh"
class mythScreenDecoder :
	public mythVirtualDecoder
{
public:
	static void staticresponse(void *myth, char* pdata, int plength);
	static mythScreenDecoder* CreateNew(){
		return new mythScreenDecoder();
	}
	void response(char* pdata, int plength);
	void start();
	void stop();
	~mythScreenDecoder();
protected:
	int decodethread();
	mythScreenDecoder();
	static int decodethreadstatic(void* data);
	mythFFmpegEncoder* encoder;
	SDL_Thread* startthread;
	SDL_mutex* startmutex;
private:
	void show_dshow_device();
	void show_avfoundation_device();
	int Init();
	int SetupFormat(const char* short_name, const char* filename);
	AVFormatContext	*pFormatCtx;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame;
	AVPacket packet;
	int videoindex;
};


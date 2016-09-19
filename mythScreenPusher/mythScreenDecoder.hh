#pragma once
#include "mythconfig.hh"
#include "mythVirtualDecoder.hh"
#include "mythFFmpegEncoder.hh"
#include "mythScreenCapture.hh"
#include "srs_librtmp.h"
//#include "dll.h"
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
	int InitSrsRTMP(const char* rtmpurl);
protected:
	int decodethread();
	mythScreenDecoder();
	static int decodethreadstatic(void* data);
	static int pushthreadstatic(void* data);
	int pushthread();
	mythFFmpegEncoder* encoder;
	SDL_Thread* startthread;
private:
	//void* ptr;
	int videoindex;
	FILE* file;
	int64_t start_time;
	int frame_index; 

	srs_rtmp_t rtmp;
	int fTimestamp;
	int pts, dts;
};


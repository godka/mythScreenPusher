#pragma once
#include "mythconfig.hh"
#include "mythVirtualDecoder.hh"
#include "mythFFmpegEncoder.hh"
#include "mythScreenCapture.hh"
#include "srs_librtmp.h"
#include <string>
using namespace std;
//#include "dll.h"
class mythScreenDecoder :
	public mythVirtualDecoder
{
public:
	static void staticresponse(void *myth, char* pdata, int plength);
	static mythScreenDecoder* CreateNew(const char* rtmpurl){
		return new mythScreenDecoder(rtmpurl);
	}
	void response(char* pdata, int plength);
	void start();
	void stop();
	~mythScreenDecoder();
protected:
	int InitSrsRTMP(const char* rtmpurl);
	int decodethread();
	mythScreenDecoder(const char* rtmpurl);
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
	string _rtmpurl;
};


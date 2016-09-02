#include "mythScreenDecoder.hh"
#include <iostream>
using namespace std;

mythScreenDecoder::mythScreenDecoder()
	:mythVirtualDecoder()
{
	flag = 0;
	startthread = NULL;
	encoder = NULL;
	startmutex = SDL_CreateMutex();
	ptr = RTMPInit("rtmp://localhost/live/stream");
	Init();
	file = fopen("test.h264", "wb");

}

//Show Dshow Device
void mythScreenDecoder::show_dshow_device(){
	AVFormatContext *pFormatCtx = avformat_alloc_context();
	AVDictionary* options = NULL;
	av_dict_set(&options, "list_devices", "true", 0);
	AVInputFormat *iformat = av_find_input_format("dshow");
	printf("========Device Info=============\n");
	avformat_open_input(&pFormatCtx, "video=dummy", iformat, &options);
	printf("================================\n");
}

//Show AVFoundation Device
void mythScreenDecoder::show_avfoundation_device(){
	AVFormatContext *pFormatCtx = avformat_alloc_context();
	AVDictionary* options = NULL;
	av_dict_set(&options, "list_devices", "true", 0);
	AVInputFormat *iformat = av_find_input_format("avfoundation");
	printf("==AVFoundation Device Info===\n");
	avformat_open_input(&pFormatCtx, "", iformat, &options);
	printf("=============================\n");
}

int mythScreenDecoder::SetupFormat(const char* short_name,const char* filename){
	AVInputFormat *ifmt = av_find_input_format(short_name);
	if (avformat_open_input(&pFormatCtx, filename, ifmt, NULL) != 0){
		printf("Couldn't open input stream.\n");
		return -1;
	}
	return 0;
}
mythScreenDecoder::~mythScreenDecoder()
{
	if (encoder != NULL){
		encoder->Cleanup();
		delete encoder;
		encoder = NULL;
	}
	if (startmutex){
		SDL_DestroyMutex(startmutex);
	}
}
#define USE_DSHOW 0
int mythScreenDecoder::Init(){

	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
	avdevice_register_all();
#ifdef _WIN32
#if USE_DSHOW
	show_dshow_device();
	if (SetupFormat("dshow", "video=screen-capture-recorder") == -1){
		SetupFormat("gdigrab", "desktop");
	}
#else
	SetupFormat("gdigrab", "desktop");
#endif
#elif defined linux
	SetupFormat("x11grab", ":0.0+10,20");
#else
	show_avfoundation_device();
	SetupFormat("avfoundation", "1");
#endif

	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}
	videoindex = -1;
	for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++){
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			videoindex = i;
			break;
		}
	}
	if (videoindex == -1){
		printf("Didn't find a video stream.\n");
		return -1;
	}
	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL){
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return -1;
	}
	pFrame = av_frame_alloc();
	av_init_packet(&packet);
	return 0;
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
	while (flag == 0){
		SDL_PollEvent(NULL);
		if (av_read_frame(pFormatCtx, &packet) >= 0){
			if (packet.stream_index == videoindex){
				int ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
				if (ret < 0){
					printf("Decode Error.\n");
					return -1;
				}
				if (got_picture){
					if (encoder == NULL){
						width = pFrame->width;
						height = pFrame->height;
						encoder = mythFFmpegEncoder::CreateNew(this,
							width,height);
						yy = new char[width * height];
						uu = new char[width * height / 4];
						vv = new char[width * height / 4];
						tmpsrc[0] = yy;
						tmpsrc[1] = uu;
						tmpsrc[2] = vv;
					}
					int tmplinesize [] = { width, width / 2, width / 2 };
					mythFFmpegEncoder::RGB2yuv(width, height, pFrame->linesize[0], pFrame->data[0], (void**) tmpsrc);
					encoder->ProcessFrame((unsigned char**) tmpsrc, tmplinesize, mythScreenDecoder::staticresponse);
				}
			}
			av_free_packet(&packet);
		}
		SDL_Delay(1);
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
	RTMPStart(ptr);
	return 0;
}

void mythScreenDecoder::stop()
{
	flag = 1;
	if (startthread)
		SDL_WaitThread(startthread, NULL);
}

void mythScreenDecoder::staticresponse(void *myth, char* pdata, int plength)
{
	//cout << "process frame callback" << endl;
	mythScreenDecoder* decoder = (mythScreenDecoder*) myth;
	decoder->response(pdata, plength);
}

void mythScreenDecoder::response(char* pdata, int plength)
{
	RTMPPutH264Data(ptr,pdata, plength);
	fwrite(pdata, plength, 1, file);
	//put((unsigned char*) pdata, (unsigned int) plength);

}

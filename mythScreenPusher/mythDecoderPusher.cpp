#include "mythDecoderPusher.hh"

mythDecoderPusher::mythDecoderPusher(mythVirtualDecoder* decoder, const char* ip, int port, int cameraid)
{
	mip = ip;
	mport = port;
	mcameraid = cameraid;
	mdecoder = decoder;
}

mythDecoderPusher::~mythDecoderPusher()
{
}

void mythDecoderPusher::start()
{
	for (;;){
		people = PEOPLE::CreateNew(mip, mport);
		baseclient = mythBaseClient::CreateNew(people);
		if (people){
			char tmpsendstr[100] = { 0 };
			SDL_snprintf(tmpsendstr, 100, "PUT /CameraID=%d&Type=zyh264 HTTP/1.0\r\n\r\n", mcameraid);
			people->socket_SendStr(tmpsendstr);
			SDL_Delay(100);
			for (;;){
				if (!people)
					break;
				if (mdecoder){
					PacketQueue* pkt = mdecoder->get();
					if (pkt){
						if (baseclient->DataCallBack((void*) pkt->h264Packet, pkt->h264PacketLength) == 1){
							printf("fuck,fuck!\n");
							//people->socket_CloseSocket();
							//delete people;
							break;
						}
					}
					SDL_Delay(1);
					SDL_PollEvent(NULL);
				}
			}
		}
		delete baseclient;
		delete people;
	}
}

void mythDecoderPusher::stop()
{

}

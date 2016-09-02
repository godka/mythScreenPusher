#pragma once
#include "MythConfig.hh"
#include "mythVirtualDecoder.hh"
#include "PEOPLE.hh"
#include "mythBaseClient.hh"
class mythDecoderPusher
{
public:
	static mythDecoderPusher* CreateNew(mythVirtualDecoder* decoder, const char* ip, int port, int cameraid){
		return new mythDecoderPusher(decoder,ip, port, cameraid);
	}
	mythDecoderPusher(mythVirtualDecoder* decoder, const char* ip, int port, int cameraid);
	~mythDecoderPusher();
	void start();
	void stop();
private:
	mythVirtualDecoder* mdecoder;
	const char* mip;
	int mport;
	int mcameraid;
	PEOPLE* people;
	mythBaseClient* baseclient;
};


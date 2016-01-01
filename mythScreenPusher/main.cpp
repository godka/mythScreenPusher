#include <stdio.h>
#include "MythConfig.hh"
#include "mythScreenDecoder.hh"
#include "mythDecoderPusher.hh"
int main(int argc, char* argv[])
{
	mythScreenDecoder* decoder = mythScreenDecoder::CreateNew();
	if (decoder)
		decoder->start();

	mythDecoderPusher* pusher = mythDecoderPusher::CreateNew(decoder, argv[1], 5834, atoi(argv[2]));
	//mythDecoderPusher* pusher = mythDecoderPusher::CreateNew(decoder, 
	//	"127.0.0.1", 5834, 
	//	10024);
	if (pusher)
		pusher->start();
	return 0;
}


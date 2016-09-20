#include <stdio.h>
#include "MythConfig.hh"
#include "mythScreenDecoder.hh"
#include <iostream>
using namespace std;


int main(int argc, char* argv [])
{

	mythScreenDecoder* decoder = mythScreenDecoder::CreateNew("rtmp://localhost/live/stream");
	if (decoder)
		decoder->start();
	return 0;
}


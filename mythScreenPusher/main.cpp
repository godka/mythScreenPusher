#include <stdio.h>
#include "MythConfig.hh"
#include "mythScreenDecoder.hh"

void MyMethod(int time)
{
	HDC hdcStill = GetDC(NULL);
	char FileName[256] = { 0 };
	int BitPerPixel = GetDeviceCaps(hdcStill, BITSPIXEL);
	int lStillHeight = GetSystemMetrics(SM_CYSCREEN);
	int lStillWidth = GetSystemMetrics(SM_CXSCREEN);
	HDC hMemDC = CreateCompatibleDC(hdcStill);
	HBITMAP hBit, hBitOld;
	hBit = CreateCompatibleBitmap(hdcStill, lStillWidth, lStillHeight);
	hBitOld = (HBITMAP) SelectObject(hMemDC, hBit);
	BitBlt(hMemDC, 0, 0, lStillWidth, lStillHeight, hdcStill, 0, 0, SRCCOPY);//������Ļͼ���ڴ�DC
	BITMAP bmp;
	GetObject(hBit, sizeof(BITMAP), &bmp);
	sprintf(FileName, "%d.bmp", time);
	FILE *fp = fopen(FileName, "w+b");
	BITMAPINFOHEADER bih = { 0 };//λͼ��Ϣͷ
	bih.biBitCount = bmp.bmBitsPixel;//ÿ�������ֽڴ�С
	bih.biCompression = BI_RGB;
	bih.biHeight = bmp.bmHeight;//�߶�
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//ͼ�����ݴ�С
	bih.biWidth = bmp.bmWidth;//���

	BITMAPFILEHEADER bfh = { 0 };
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//��λͼ���ݵ�ƫ����
	bfh.bfSize = bfh.bfOffBits + bmp.bmWidthBytes * bmp.bmHeight;//�ļ��ܵĴ�С
	bfh.bfType = (WORD) 0x4d42;
	fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);//д��λͼ�ļ�ͷ
	fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);//д��λͼ��Ϣͷ
	char* p = new char[bmp.bmWidthBytes * bmp.bmHeight];//�����ڴ汣��λͼ����
	GetDIBits(hMemDC, (HBITMAP) hBit, 0, lStillHeight, p,
		(LPBITMAPINFO) &bih, DIB_RGB_COLORS);//��ȡλͼ����
	fwrite(p, 1, bmp.bmWidthBytes * bmp.bmHeight, fp);//д��λͼ����
	fclose(fp);
	SelectObject(hMemDC, hBitOld);
	ReleaseDC(NULL, hdcStill);
	ReleaseDC(NULL, hMemDC);
	delete [] p;
}

int main(int argc, char* argv [])
{
	//for (int i = 0;;i++){
	//	MyMethod(i);
	//	SDL_Delay(1);
	//}
	mythScreenDecoder* decoder = mythScreenDecoder::CreateNew();
	if (decoder)
		decoder->start();

	//mythDecoderPusher* pusher = mythDecoderPusher::CreateNew(decoder, argv[1], 5834, atoi(argv[2]));
	////mythDecoderPusher* pusher = mythDecoderPusher::CreateNew(decoder, 
	////	"127.0.0.1", 5834, 
	////	10024);
	//if (pusher)
	//	pusher->start();
	return 0;
}


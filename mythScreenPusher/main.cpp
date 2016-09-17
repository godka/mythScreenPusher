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
	BitBlt(hMemDC, 0, 0, lStillWidth, lStillHeight, hdcStill, 0, 0, SRCCOPY);//复制屏幕图像到内存DC
	BITMAP bmp;
	GetObject(hBit, sizeof(BITMAP), &bmp);
	sprintf(FileName, "%d.bmp", time);
	FILE *fp = fopen(FileName, "w+b");
	BITMAPINFOHEADER bih = { 0 };//位图信息头
	bih.biBitCount = bmp.bmBitsPixel;//每个像素字节大小
	bih.biCompression = BI_RGB;
	bih.biHeight = bmp.bmHeight;//高度
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//图像数据大小
	bih.biWidth = bmp.bmWidth;//宽度

	BITMAPFILEHEADER bfh = { 0 };
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//到位图数据的偏移量
	bfh.bfSize = bfh.bfOffBits + bmp.bmWidthBytes * bmp.bmHeight;//文件总的大小
	bfh.bfType = (WORD) 0x4d42;
	fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);//写入位图文件头
	fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);//写入位图信息头
	char* p = new char[bmp.bmWidthBytes * bmp.bmHeight];//申请内存保存位图数据
	GetDIBits(hMemDC, (HBITMAP) hBit, 0, lStillHeight, p,
		(LPBITMAPINFO) &bih, DIB_RGB_COLORS);//获取位图数据
	fwrite(p, 1, bmp.bmWidthBytes * bmp.bmHeight, fp);//写入位图数据
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


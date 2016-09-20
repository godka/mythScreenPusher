#include "mythScreenCapture.hh"


mythScreenCapture::mythScreenCapture(HWND hwnd)
{
	_hwnd = hwnd;
	init();
}


void mythScreenCapture::init()
{
	hdcStill = GetDC(_hwnd);
	BitPerPixel = GetDeviceCaps(hdcStill, BITSPIXEL);
	lStillHeight = GetSystemMetrics(SM_CYSCREEN);
	lStillWidth = GetSystemMetrics(SM_CXSCREEN);

	hMemDC = CreateCompatibleDC(hdcStill);
	hBit = CreateCompatibleBitmap(hdcStill, lStillWidth, lStillHeight);
	//hMemDC = CreateCompatibleDC(hdcStill);
	//hBit = CreateCompatibleBitmap(hdcStill, lStillWidth, lStillHeight);
	GetObject(hBit, sizeof(BITMAP), &bmp);
	memset(&bih, 0, sizeof(BITMAPINFOHEADER));
	bih.biBitCount = bmp.bmBitsPixel;//每个像素字节大小
	bih.biCompression = BI_RGB;
	bih.biHeight = bmp.bmHeight;//高度
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//图像数据大小
	bih.biWidth = bmp.bmWidth;//宽度

	memset(&bfh, 0, sizeof(BITMAPFILEHEADER));
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//到位图数据的偏移量
	bfh.bfSize = bfh.bfOffBits + bmp.bmWidthBytes * bmp.bmHeight;//文件总的大小
	bfh.bfType = (WORD) 0x4d42;
	//p = NULL;
	memset(&CursorPar, 0, sizeof(CursorPar));
	GetCursorInfo(&CursorPar);
	iCursor = CopyIcon(LoadCursor(NULL, IDC_ARROW));

	p = new char[bmp.bmWidthBytes * bmp.bmHeight];//申请内存保存位图数据

}

mythScreenCapture::~mythScreenCapture()
{
	ReleaseDC(_hwnd, hdcStill);
	ReleaseDC(_hwnd, hMemDC);
	delete [] p;
}

char* mythScreenCapture::capture()
{
	GetCursorPos(&pt);
	hBitOld = (HBITMAP) SelectObject(hMemDC, hBit);
	BitBlt(hMemDC, 0, 0, lStillWidth, lStillHeight, hdcStill, 0, 0, SRCCOPY);
	DrawIcon(hMemDC, pt.x, pt.y, iCursor);
	GetBitmapBits(hBit, bmp.bmWidthBytes*bmp.bmHeight, p);
	//GetDIBits(hMemDC, (HBITMAP) hBit, 0, lStillHeight, p, (LPBITMAPINFO) &bih, DIB_RGB_COLORS);//获取位图数据
	SelectObject(hMemDC, hBitOld);
	return p;
}

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
	bih.biBitCount = bmp.bmBitsPixel;//ÿ�������ֽڴ�С
	bih.biCompression = BI_RGB;
	bih.biHeight = bmp.bmHeight;//�߶�
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//ͼ�����ݴ�С
	bih.biWidth = bmp.bmWidth;//���

	memset(&bfh, 0, sizeof(BITMAPFILEHEADER));
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//��λͼ���ݵ�ƫ����
	bfh.bfSize = bfh.bfOffBits + bmp.bmWidthBytes * bmp.bmHeight;//�ļ��ܵĴ�С
	bfh.bfType = (WORD) 0x4d42;
	//p = NULL;
	memset(&CursorPar, 0, sizeof(CursorPar));
	GetCursorInfo(&CursorPar);
	iCursor = CopyIcon(LoadCursor(NULL, IDC_ARROW));

	p = new char[bmp.bmWidthBytes * bmp.bmHeight];//�����ڴ汣��λͼ����

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
	//GetDIBits(hMemDC, (HBITMAP) hBit, 0, lStillHeight, p, (LPBITMAPINFO) &bih, DIB_RGB_COLORS);//��ȡλͼ����
	SelectObject(hMemDC, hBitOld);
	return p;
}

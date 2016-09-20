#pragma once
#include "MythConfig.hh"
#include <Windows.h>
class mythScreenCapture
{
public:
	static mythScreenCapture* CreateNew(HWND hwnd){
		return new mythScreenCapture(hwnd);
	}
	int Width(){
		return lStillWidth;
	}
	int Height(){
		return lStillHeight;
	}
	~mythScreenCapture();
	char* capture();
protected:
	mythScreenCapture(HWND hwnd);
private:
	HWND _hwnd;
	void init();
	HDC hdcStill;// = GetDC(NULL);
	int BitPerPixel;
	int lStillHeight;
	int lStillWidth;
	HDC hMemDC;
	HBITMAP hBit, hBitOld;
	BITMAP bmp;
	BITMAPINFOHEADER bih;
	BITMAPFILEHEADER bfh;
	char* p;
	CURSORINFO CursorPar;
	HICON iCursor;
	POINT pt;
};


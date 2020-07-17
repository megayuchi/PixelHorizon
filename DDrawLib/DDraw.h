// DDraw.h: interface for the CDDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDRAW_H__627A510B_7641_4053_B222_0C0951603602__INCLUDED_)
#define AFX_DDRAW_H__627A510B_7641_4053_B222_0C0951603602__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ddraw.h>
#include "../Common/typedef.h"

class CImageData;
class CDDraw
{
	LPDIRECTDRAW			m_pDD = nullptr;
	LPDIRECTDRAW7			m_pDD7 = nullptr;
	LPDIRECTDRAWSURFACE7	m_pDDPrimary = nullptr;
	LPDIRECTDRAWSURFACE7	m_pDDBack = nullptr;
	LPDIRECTDRAWCLIPPER		m_pClipper = nullptr;

	char*	m_pLockedBackBuffer = nullptr;
	DWORD	m_dwLockedBackBufferPitch = 0;
	DWORD	m_dwWidth = 0;
	DWORD	m_dwHeight = 0;
	RECT	m_rcWindow = {};
	HWND	m_hWnd = nullptr;
	DWORD	m_dwFrameCount = 0;
	DWORD	m_dwFPS = 0;
	ULONGLONG m_LastDrawTick = 0;
	WCHAR	m_wchInfoTxt[64] = {};
	DWORD	m_dwInfoTxtLen = 0;
	


	BOOL	CreateBackBuffer(DWORD dwWidth, DWORD dwHeight);
	void	CleanupBackBuffer();
	BOOL	GetFontSize(int* piOutWidth, int* piOutHeight, const WCHAR* wchString, DWORD dwStrLen, HDC hDC);
	
	
	void	UpdateInfoTxt();
public:
	BOOL	InitializeDDraw(HWND hWnd);
	BOOL	LockBackBuffer(char** ppBits, DWORD* pdwWidth, DWORD* pdwHeight, DWORD* pdwPitch);
	void	UnlockBackBuffer();

	BOOL	CalcClipArea(INT_VECTOR2* pivOutSrcStart, INT_VECTOR2* pivOutDestStart, INT_VECTOR2* pivOutDestSize, const INT_VECTOR2* pivPos, const INT_VECTOR2* pivImageSize);
	void	OnUpdateWindowSize();
	void	OnUpdateWindowPos();
	void	SetPixelRandom();
	void	DrawRect(int x, int y, int iWidth, int iHeight, DWORD dwColor);
	BOOL	DrawBitmap(int sx, int sy, int iBitmapWidth, int iBitmapHeight, char* pBits);
	BOOL	DrawBitmapWithColorKey(int sx, int sy, int iBitmapWidth, int iBitmapHeight, char* pBits, DWORD dwColorKey);
	BOOL	DrawImageData(int sx, int sy, const CImageData* pImgData);

	BOOL	BeginDraw();
	void	EndDraw();

	void	DrawInfo(HDC hDC);
	BOOL	BeginGDI(HDC* pOutDC);
	void	EndGDI(HDC hDC);
	void	ProcessGDI(HDC hDC);
	void	WriteText(const WCHAR* wchTxt, DWORD dwLen, int x, int y, DWORD dwColor, HDC hDC);
	void	Clear();
	void	OnDraw();
	BOOL	CheckFPS();

	void	Cleanup();
	DWORD	GetWidth() { return m_dwWidth; }
	DWORD	GetHeight() { return m_dwHeight; }

	DWORD	CaptureBackBuffer(FILE* fp);
	
	CDDraw();
	~CDDraw();

};

#endif // !defined(AFX_DDRAW_H__627A510B_7641_4053_B222_0C0951603602__INCLUDED_)

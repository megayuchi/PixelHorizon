// DDraw.cpp: implementation of the CDDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../Common/typedef.h"
#include <stdio.h>
#include "DDraw.h"
#include "../Util/Util.h"
#include "../ImageData/ImageData.h"
#include "../Util/TGAImage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDDraw::CDDraw()
{
	memset(this, 0, sizeof(CDDraw));

}

#include <ddraw.h>

BOOL CDDraw::InitializeDDraw(HWND hWnd)
{
	BOOL	bResult = FALSE;

	m_hWnd = hWnd;

	DDSURFACEDESC2 ddsd = {};
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;


	if (DD_OK != DirectDrawCreate(nullptr, &m_pDD, nullptr))
	{
		MessageBox(hWnd, L"Fail to Create DirectDraw", L"Error", MB_OK);
		goto lb_return;
	}

	if (DD_OK != m_pDD->QueryInterface(IID_IDirectDraw7, (LPVOID*)&m_pDD7))
	{
		MessageBox(hWnd, L"Fail to Create DirectDraw 4", L"Error", MB_OK);
		goto lb_return;
	}

	HRESULT hr = m_pDD7->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
	if (FAILED(hr))
	{
		MessageBox(m_hWnd, L"Failed to Set CooperativeLevel", L"ERROR", MB_OK);
		goto lb_return;
	}



	// Create the primary surface.
	hr = m_pDD7->CreateSurface(&ddsd, &m_pDDPrimary, nullptr);
	if (FAILED(hr))
	{
		MessageBox(m_hWnd, L"Failed to CreateSurface", L"ERROR", MB_OK);
		goto lb_return;
	}
	// Create a clipper object which handles all our clipping for cases when
	// our window is partially obscured by other windows. This is not needed
	// for apps running in fullscreen mode.

	hr = m_pDD->CreateClipper(0, &m_pClipper, nullptr);
	if (FAILED(hr))
	{
		MessageBox(m_hWnd, L"Failed to Create Clipper", L"ERROR", MB_OK);
		goto lb_return;
	}
	// Associate the clipper with our window. Note that, afterwards, the
	// clipper is internally referenced by the primary surface, so it is safe
	// to release our local reference to it.
	m_pClipper->SetHWnd(0, hWnd);
	m_pDDPrimary->SetClipper(m_pClipper);

	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

	OnUpdateWindowPos();

	DWORD dwWidth = m_rcWindow.right - m_rcWindow.left;
	DWORD dwHeight = m_rcWindow.bottom - m_rcWindow.top;

	if (!CreateBackBuffer(dwWidth, dwHeight))
	{
	#ifdef _DEBUG
		__debugbreak();
	#endif
		goto lb_return;
	}

	bResult = TRUE;

lb_return:
	return bResult;

}


BOOL CDDraw::CreateBackBuffer(DWORD dwWidth, DWORD dwHeight)
{
	BOOL	bResult = FALSE;

	DDSURFACEDESC2 ddsd = {};
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = dwWidth;
	ddsd.dwHeight = dwHeight;

	HRESULT hr = m_pDD7->CreateSurface(&ddsd, &m_pDDBack, nullptr);
	if (FAILED(hr))
	{
		MessageBox(m_hWnd, L"Failed to Create Surface", L"ERROR", MB_OK);
		goto lb_return;
	}
	m_pDDBack->GetSurfaceDesc(&ddsd);
	m_dwWidth = ddsd.dwWidth;
	m_dwHeight = ddsd.dwHeight;
	bResult = TRUE;
lb_return:
	return bResult;
}

void CDDraw::Clear()
{
	if (!m_pLockedBackBuffer)
	{
	#ifdef _DEBUG
		__debugbreak();
	#endif
		return;
	}
	for (DWORD y = 0; y < m_dwHeight; y++)
	{
		memset(m_pLockedBackBuffer + y * m_dwLockedBackBufferPitch, 0, 4 * m_dwWidth);
	}
}

BOOL CDDraw::LockBackBuffer(char** ppBits, DWORD* pdwWidth, DWORD* pdwHeight, DWORD* pdwPitch)
{
	BOOL	bResult = FALSE;

	if (m_pDDBack)
	{

		DDSURFACEDESC2 ddsc;
		memset(&ddsc, 0, sizeof(DDSURFACEDESC2));
		ddsc.dwSize = sizeof(DDSURFACEDESC2);

		m_pDDBack->Lock(nullptr, &ddsc, DDLOCK_WAIT | DDLOCK_WRITEONLY, nullptr);

		*ppBits = (char*)ddsc.lpSurface;
		*pdwPitch = ddsc.lPitch;
		*pdwWidth = ddsc.dwWidth;
		*pdwHeight = ddsc.dwHeight;
	}

	bResult = TRUE;
lb_return:
	return bResult;
}
void CDDraw::UnlockBackBuffer()
{
	if (m_pDDBack)
		m_pDDBack->Unlock(nullptr);
}

DWORD CDDraw::CaptureBackBuffer(FILE* fp)
{
	DWORD	dwResult = 0;

	if (!m_pDDBack)
		goto lb_return;

	DDSURFACEDESC2 ddsc;
	memset(&ddsc, 0, sizeof(DDSURFACEDESC2));
	ddsc.dwSize = sizeof(DDSURFACEDESC2);

	m_pDDBack->Lock(nullptr, &ddsc, DDLOCK_WAIT | DDLOCK_READONLY, nullptr);

	dwResult = WriteTGAImage(fp, (char*)ddsc.lpSurface, ddsc.dwWidth, ddsc.dwHeight, ddsc.lPitch, 32);
	m_pDDBack->Unlock(nullptr);

lb_return:
	return dwResult;
}


void CDDraw::SetPixelRandom()
{
	DWORD	dwColor = 0xffffffff;

	char*	pBits = nullptr;
	DWORD	dwWidth = 0;
	DWORD	dwHeight = 0;
	DWORD	dwPitch = 0;
	if (!LockBackBuffer(&pBits, &dwWidth, &dwHeight, &dwPitch))
	{
		__debugbreak();
	}
	DWORD x = rand() % dwWidth;
	DWORD y = rand() % dwHeight;
	DWORD*	pDest = (DWORD*)(pBits + x * 4 + y * dwPitch);
	*pDest = dwColor;

	UnlockBackBuffer();

}
void CDDraw::OnUpdateWindowSize()
{
	CleanupBackBuffer();

	OnUpdateWindowPos();

	DWORD dwWidth = m_rcWindow.right - m_rcWindow.left;
	DWORD dwHeight = m_rcWindow.bottom - m_rcWindow.top;

	CreateBackBuffer(dwWidth, dwHeight);

	if (BeginDraw())
	{
		Clear();
		EndDraw();
	}

}
void CDDraw::OnUpdateWindowPos()
{
	GetClientRect(m_hWnd, &m_rcWindow);
	::ClientToScreen(m_hWnd, (POINT*)&m_rcWindow.left);
	::ClientToScreen(m_hWnd, (POINT*)&m_rcWindow.right);
}
void CDDraw::DrawRect(int sx, int sy, int iWidth, int iHeight, DWORD dwColor)
{

#ifdef _DEBUG
	if (!m_pLockedBackBuffer)
		__debugbreak();

#endif
	int start_x = max(0, sx);
	int start_y = max(0, sy);
	int end_x = min(sx + iWidth, (int)m_dwWidth);
	int end_y = min(sy + iHeight, (int)m_dwHeight);

	for (int y = start_y; y < end_y; y++)
	{
		for (int x = start_x; x < end_x; x++)
		{
			DWORD*	pDest = (DWORD*)(m_pLockedBackBuffer + x * 4 + y * m_dwLockedBackBufferPitch);
			*pDest = dwColor;
		}
	}
}
BOOL CDDraw::CalcClipArea(INT_VECTOR2* pivOutSrcStart, INT_VECTOR2* pivOutDestStart, INT_VECTOR2* pivOutDestSize, const INT_VECTOR2* pivPos, const INT_VECTOR2* pivImageSize)
{
	INT_VECTOR2	ivBufferSize = { (int)m_dwWidth, (int)m_dwHeight };
	BOOL bResult = ::CalcClipArea(pivOutSrcStart, pivOutDestStart, pivOutDestSize, pivPos, pivImageSize, &ivBufferSize);
	return bResult;
}
BOOL CDDraw::BeginDraw()
{
	BOOL	bResult = FALSE;

	char*	pBuffer = nullptr;
	DWORD	dwBufferWidth = 0;
	DWORD	dwBufferHeight = 0;
	DWORD	dwPitch = 0;
	if (!LockBackBuffer(&pBuffer, &dwBufferWidth, &dwBufferHeight, &dwPitch))
	{
	#ifdef _DEBUG
		__debugbreak();
	#endif
		goto lb_return;
	}
#ifdef _DEBUG
	if (dwBufferWidth != m_dwWidth)
		__debugbreak();

	if (dwBufferHeight != m_dwHeight)
		__debugbreak();
#endif
	m_pLockedBackBuffer = pBuffer;
	m_dwLockedBackBufferPitch = dwPitch;
	bResult = TRUE;
lb_return:
	return bResult;
}
void CDDraw::EndDraw()
{
	UnlockBackBuffer();
	m_pLockedBackBuffer = nullptr;
	m_dwLockedBackBufferPitch = 0;
}
BOOL CDDraw::DrawBitmap(int sx, int sy, int iBitmapWidth, int iBitmapHeight, char* pBits)
{
	BOOL	bResult = FALSE;

#ifdef _DEBUG
	if (!m_pLockedBackBuffer)
		__debugbreak();

#endif


	INT_VECTOR2	ivSrcStart = {};
	INT_VECTOR2	ivDestStart = {};

	INT_VECTOR2	ivPos = { sx, sy };
	INT_VECTOR2	ivImageSize = { iBitmapWidth, iBitmapHeight };
	INT_VECTOR2 ivDestSize = {};

	if (!CalcClipArea(&ivSrcStart, &ivDestStart, &ivDestSize, &ivPos, &ivImageSize))
		goto lb_return;


	char* pSrc = pBits + (ivSrcStart.x + ivSrcStart.y * iBitmapWidth) * 4;
	char* pDest = m_pLockedBackBuffer + (ivDestStart.x * 4) + ivDestStart.y * m_dwLockedBackBufferPitch;

	for (int y = 0; y < ivDestSize.y; y++)
	{
		for (int x = 0; x < ivDestSize.x; x++)
		{
			*(DWORD*)pDest = *(DWORD*)pSrc;
			pSrc += 4;
			pDest += 4;
		}
		pSrc -= (ivDestSize.x * 4);
		pSrc += (iBitmapWidth * 4);
		pDest -= (ivDestSize.x * 4);
		pDest += m_dwLockedBackBufferPitch;
	}
	//
	bResult = TRUE;
lb_return:
	return bResult;

}

BOOL CDDraw::DrawBitmapWithColorKey(int sx, int sy, int iBitmapWidth, int iBitmapHeight, char* pBits, DWORD dwColorKey)
{
	BOOL	bResult = FALSE;

#ifdef _DEBUG
	if (!m_pLockedBackBuffer)
		__debugbreak();

#endif


	INT_VECTOR2	ivSrcStart = {};
	INT_VECTOR2	ivDestStart = {};

	INT_VECTOR2	ivPos = { sx, sy };
	INT_VECTOR2	ivImageSize = { iBitmapWidth, iBitmapHeight };
	INT_VECTOR2 ivDestSize = {};

	if (!CalcClipArea(&ivSrcStart, &ivDestStart, &ivDestSize, &ivPos, &ivImageSize))
		goto lb_return;


	char* pSrc = pBits + (ivSrcStart.x + ivSrcStart.y * iBitmapWidth) * 4;
	char* pDest = m_pLockedBackBuffer + (ivDestStart.x * 4) + ivDestStart.y * m_dwLockedBackBufferPitch;

	for (int y = 0; y < ivDestSize.y; y++)
	{
		for (int x = 0; x < ivDestSize.x; x++)
		{
			if (*(DWORD*)pSrc != dwColorKey)
			{
				*(DWORD*)pDest = *(DWORD*)pSrc;
			}
			pSrc += 4;
			pDest += 4;
		}
		pSrc -= (ivDestSize.x * 4);
		pSrc += (iBitmapWidth * 4);
		pDest -= (ivDestSize.x * 4);
		pDest += m_dwLockedBackBufferPitch;
	}
	//
	bResult = TRUE;
lb_return:
	return bResult;

}

BOOL CDDraw::DrawImageData(int sx, int sy, const CImageData* pImgData)
{
	BOOL	bResult = FALSE;

#ifdef _DEBUG
	if (!m_pLockedBackBuffer)
		__debugbreak();

#endif

	int iScreenWidth = (int)m_dwWidth;

	int iBitmapWidth = (int)pImgData->GetWidth();
	int iBitmapHeight = (int)pImgData->GetHeight();

	INT_VECTOR2	ivSrcStart = {};
	INT_VECTOR2	ivDestStart = {};

	INT_VECTOR2	ivPos = { sx, sy };
	INT_VECTOR2	ivImageSize = { iBitmapWidth, iBitmapHeight };
	INT_VECTOR2 ivDestSize = {};

	if (!CalcClipArea(&ivSrcStart, &ivDestStart, &ivDestSize, &ivPos, &ivImageSize))
		goto lb_return;

	const COMPRESSED_LINE* pLineDesc = pImgData->GetCompressedImage(ivSrcStart.y);
	char* pDestPerLine = m_pLockedBackBuffer + (ivDestStart.y) * m_dwLockedBackBufferPitch;

	for (int y = 0; y < ivDestSize.y; y++)
	{
		for (DWORD i = 0; i < pLineDesc->dwStreamNum; i++)
		{
			PIXEL_STREAM*	pStream = pLineDesc->pPixelStream + i;
			DWORD	dwPixelColor = pStream->dwPixel;
			int		iPixelNum = (int)pStream->wPixelNum;

			int dest_x = sx + (int)pStream->wPosX;
			if (dest_x < 0)
			{
				iPixelNum += dest_x;
				dest_x = 0;
			}
			if (dest_x + iPixelNum > iScreenWidth)
			{
				iPixelNum = iScreenWidth - dest_x;
			}
			char* pDest = pDestPerLine + (DWORD)(dest_x * 4);
			for (int x = 0; x < iPixelNum; x++)
			{
				*(DWORD*)pDest = dwPixelColor;
				pDest += 4;
			}
		}
		pLineDesc++;
		pDestPerLine += m_dwLockedBackBufferPitch;
	}
	//
	bResult = TRUE;
lb_return:
	return bResult;

}


void CDDraw::UpdateInfoTxt()
{
	m_dwInfoTxtLen = swprintf_s(m_wchInfoTxt, L"FPS : %u", m_dwFPS);
}
BOOL CDDraw::CheckFPS()
{
	BOOL	bUpdated = FALSE;

	ULONGLONG CurTick = GetTickCount64();
	if (CurTick - m_LastDrawTick > 1000)
	{
		DWORD	dwOldPFS = m_dwFPS;

		m_dwFPS = m_dwFrameCount;
		m_LastDrawTick = CurTick;
		m_dwFrameCount = 0;
		if (m_dwFPS != dwOldPFS)
		{
			UpdateInfoTxt();
			bUpdated = TRUE;
		}
		else
		{
			int a = 0;
		}
	}
	m_dwFrameCount++;
	return bUpdated;
}
#if defined(ARM64) && !defined(_DEBUG)
//#pragma optimize( "gpsy",off)
//#pragma optimize( "g",off)
void CDDraw::OnDraw()
{
	
	//WCHAR	wchTxt[128];
	//swprintf_s(wchTxt, L"%p, %p, %d,%d, %d,%d\n", m_pDDPrimary, m_pDDBack, m_rcWindow.left, m_rcWindow.top, m_rcWindow.right, m_rcWindow.bottom);
	//OutputDebugString(wchTxt);
	m_pDDPrimary->Blt(&m_rcWindow, m_pDDBack, nullptr, DDBLT_WAIT, nullptr);
}
//#pragma optimize( "",on)
#else
void CDDraw::OnDraw()
{
	m_pDDPrimary->Blt(&m_rcWindow, m_pDDBack, nullptr, DDBLT_WAIT, nullptr);
}
#endif

void CDDraw::ProcessGDI(HDC hDC)
{


}
BOOL CDDraw::BeginGDI(HDC* pOutDC)
{
	BOOL	bResult = FALSE;
	HDC	hDC = nullptr;

	HRESULT hr = m_pDDBack->GetDC(&hDC);
	if (FAILED(hr))
	{
	#ifdef _DEBUG
		__debugbreak();
	#endif
		goto lb_return;
	}
	bResult = TRUE;
	*pOutDC = hDC;

lb_return:
	return bResult;
}
void CDDraw::DrawInfo(HDC hDC)
{
	const WCHAR* wchTxt = m_wchInfoTxt;
	DWORD dwLen = m_dwInfoTxtLen;

	WriteText(wchTxt, dwLen, 0, 0, 0xffff0000, hDC);
}
void CDDraw::EndGDI(HDC hDC)
{
	ProcessGDI(hDC);
	m_pDDBack->ReleaseDC(hDC);
}
void CDDraw::WriteText(const WCHAR* wchTxt, DWORD dwLen, int x, int y, DWORD dwColor, HDC hDC)
{
	SetBkMode(hDC, TRANSPARENT);

	RECT	textRect, texRectSide[4];
	int		iWidth = 0;
	int		iHeight = 0;
	GetFontSize(&iWidth, &iHeight, wchTxt, dwLen, hDC);

	textRect.left = x;
	textRect.top = y;
	textRect.right = textRect.left + iWidth;
	textRect.bottom = textRect.top + iHeight;

	texRectSide[0].left = textRect.left - 1;
	texRectSide[0].top = textRect.top - 1;
	texRectSide[0].right = textRect.right - 1;
	texRectSide[0].bottom = textRect.bottom - 1;


	texRectSide[1].left = textRect.left + 1;
	texRectSide[1].top = textRect.top - 1;
	texRectSide[1].right = textRect.right + 1;
	texRectSide[1].bottom = textRect.bottom - 1;

	texRectSide[2].left = textRect.left + 1;
	texRectSide[2].top = textRect.top + 1;
	texRectSide[2].right = textRect.right + 1;
	texRectSide[2].bottom = textRect.bottom + 1;

	texRectSide[3].left = textRect.left - 1;
	texRectSide[3].top = textRect.top + 1;
	texRectSide[3].right = textRect.right - 1;
	texRectSide[3].bottom = textRect.bottom + 1;

	SetTextColor(hDC, 0x00000000);
	for (DWORD i = 0; i < 4; i++)
	{
		DrawText(hDC, wchTxt, -1, &texRectSide[i], DT_LEFT | DT_WORDBREAK);
	}

	DWORD r = (dwColor & 0x00ff0000) >> 16;
	DWORD g = (dwColor & 0x0000ff00) >> 8;
	DWORD b = (dwColor & 0x000000ff);

	COLORREF color = RGB(r, g, b);
	SetTextColor(hDC, color);
	DrawText(hDC, wchTxt, -1, &textRect, DT_LEFT | DT_WORDBREAK);


}

BOOL CDDraw::GetFontSize(int* piOutWidth, int* piOutHeight, const WCHAR* wchString, DWORD dwStrLen, HDC hDC)
{
	BOOL	bResult = FALSE;
	*piOutWidth = 1;
	*piOutHeight = 1;

	SIZE	strSize;
	BOOL bGetSize = GetTextExtentPoint32W(hDC, wchString, dwStrLen, &strSize);


	if (bGetSize)
	{
		*piOutWidth = strSize.cx;
		*piOutHeight = strSize.cy;
		bResult = TRUE;
	}
	return bResult;
}
void CDDraw::CleanupBackBuffer()
{
	if (m_pDDBack)
	{
		m_pDDBack->Release();
		m_pDDBack = nullptr;
	}
}
void CDDraw::Cleanup()
{
	CleanupBackBuffer();

	if (m_pDDPrimary)
	{
		m_pDDPrimary->SetClipper(nullptr);
		m_pDDPrimary->Release();
		m_pDDPrimary = nullptr;
	}
	if (m_pClipper)
	{
		m_pClipper->Release();
		m_pClipper = nullptr;
	}
	if (m_pDD7)
	{
		ULONG ref_count = m_pDD7->Release();
		if (ref_count)
			__debugbreak();
		m_pDD7 = nullptr;
	}
	if (m_pDD)
	{
		ULONG ref_count = m_pDD->Release();
		if (ref_count)
			__debugbreak();
		m_pDD = nullptr;
	}
}

CDDraw::~CDDraw()
{
	Cleanup();
}

#include "stdafx.h"
#include <crtdbg.h>
#include <Windows.h>
#include "../Common/typedef.h"
#include "ImageData.h"

CImageData::CImageData()
{
}
BOOL CImageData::Create(const char* pSrcBits, DWORD dwWidth, DWORD dwHeight, DWORD dwColorKey)
{

	DWORD	dwWorkingMemSize = sizeof(PIXEL_STREAM) * dwWidth * dwHeight;
	char*	pWorkingMemory = (char*)malloc(dwWorkingMemSize);
	memset(pWorkingMemory, 0, dwWorkingMemSize);

	COMPRESSED_LINE*	pLineList = (COMPRESSED_LINE*)malloc(sizeof(COMPRESSED_LINE) * dwHeight);
	const char*	pSrcEntry = pSrcBits;
	const char* pDestEntry = pWorkingMemory;
	DWORD	dwUsedMemSize = 0;
	for (DWORD i = 0; i < dwHeight; i++)
	{
		DWORD	dwStreamNumPerLine = CreatePerLine(pDestEntry, dwWorkingMemSize, (DWORD*)pSrcEntry, dwWidth, dwColorKey);

		DWORD	dwLineMemSize = sizeof(PIXEL_STREAM) * dwStreamNumPerLine;
		pSrcEntry += (dwWidth * 4);

		DWORD dwOffset = (DWORD)((DWORD_PTR)pDestEntry - (DWORD_PTR)pWorkingMemory);
		if ((int)dwWorkingMemSize - (int)dwLineMemSize < 0)
			__debugbreak();

		dwWorkingMemSize -= dwLineMemSize;
		dwUsedMemSize += dwLineMemSize;
		pDestEntry += dwLineMemSize;
		pLineList[i].dwLineDataSize = dwLineMemSize;
		pLineList[i].dwStreamNum = dwStreamNumPerLine;
		pLineList[i].dwOffset = dwOffset;
	}

	// ������ �ȼ� ��Ʈ���� �� ������ ��ũ���Ͱ� �Ǵ� COMPRESSED_LINE�޸𸮸� �ѹ��� �Ҵ��Ѵ�.
	dwUsedMemSize += sizeof(COMPRESSED_LINE) * dwHeight;
	m_pCompressedImage = (COMPRESSED_LINE*)malloc(dwUsedMemSize);
	char*	pPixelStreamDataEntry = (char*)m_pCompressedImage + sizeof(COMPRESSED_LINE) * dwHeight;

	for (DWORD i = 0; i < dwHeight; i++)
	{
		char*	pSrcPixelStreamData = pWorkingMemory + pLineList[i].dwOffset;	// �ռ� ������ �ȼ� ��Ʈ���� ����� �޸�
		pLineList[i].pPixelStream = (PIXEL_STREAM*)pPixelStreamDataEntry;					// ����� �޸�
		memcpy(pPixelStreamDataEntry, pSrcPixelStreamData, pLineList[i].dwLineDataSize);	// ������ �ȼ� ��Ʈ���� Ȯ���� �޸𸮷� ī��.
		pPixelStreamDataEntry += pLineList[i].dwLineDataSize;
	}
	memcpy(m_pCompressedImage, pLineList, sizeof(COMPRESSED_LINE) * dwHeight);

	free(pWorkingMemory);
	free(pLineList);

	m_dwWidth = dwWidth;
	m_dwHeight = dwHeight;
#ifdef _DEBUG
	_ASSERT(_CrtCheckMemory());
#endif
	return TRUE;
}
DWORD CImageData::CreatePerLine(const char* pDest, int iMaxMemSize, const DWORD* pSrcBits, DWORD dwWidth, DWORD dwColorKey)
{
	PIXEL_STREAM*	pStreamList = (PIXEL_STREAM*)pDest;
	DWORD	dwStreamCount = 0;
	BOOL	bStarted = FALSE;

	DWORD x = 0;

	while (x < dwWidth)
	{
		// �÷�Ű�� �ƴ� �ȼ��� ���������� x������ ����
		while (pSrcBits[x] == dwColorKey)
		{
			x++;
			if (x >= dwWidth)
			{
				goto lb_return;
			}
			
		}
		DWORD dwCurPixel = pSrcBits[x];

		if (iMaxMemSize - sizeof(PIXEL_STREAM) < 0)
			__debugbreak();

		pStreamList[dwStreamCount].wPosX = (WORD)x;
		pStreamList[dwStreamCount].dwPixel = dwCurPixel;
		iMaxMemSize -= sizeof(PIXEL_STREAM);

		// ���� �ȼ��� �ٸ� �ȼ�(�÷�Ű ����)�� ���������� ����
		while (x < dwWidth && pSrcBits[x] == dwCurPixel)
		{
			pStreamList[dwStreamCount].wPixelNum++;
			x++;
		}
		dwStreamCount++;
	}
	
lb_return:
	return dwStreamCount;
}
CImageData::~CImageData()
{
	if (m_pCompressedImage)
	{
		free(m_pCompressedImage);
		m_pCompressedImage = nullptr;
	}
}

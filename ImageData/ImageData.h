#pragma once


#pragma pack(push,4)
struct PIXEL_STREAM
{
	WORD	wPosX;
	WORD	wPixelNum;
	DWORD	dwPixel;
};
#pragma pack(pop)
struct COMPRESSED_LINE
{
	DWORD	dwStreamNum;
	DWORD	dwLineDataSize;
	union
	{
		struct
		{
			PIXEL_STREAM*	pPixelStream;
		};
		struct
		{
			DWORD	dwOffset;
		};
	};
};
class CImageData
{
	DWORD	m_dwWidth = 0;
	DWORD	m_dwHeight = 0;
	COMPRESSED_LINE*	m_pCompressedImage = nullptr;
	DWORD	CreatePerLine(const char* pDest, int iMaxMemSize, const DWORD* pSrcBits, DWORD dwWidth, DWORD dwColorKey);
public:
	const COMPRESSED_LINE*	GetCompressedImage(int y) const
	{
	#ifdef _DEBUG
		if (y < 0)
			__debugbreak();
		if (y >= (int)m_dwHeight)
			__debugbreak();
	#endif
		return m_pCompressedImage + y; 
	}
	BOOL	Create(const char* pSrcBits, DWORD dwWidth, DWORD dwHeight, DWORD dwColorKey);
	DWORD	GetWidth() const { return m_dwWidth; }
	DWORD	GetHeight() const { return m_dwHeight; }

	CImageData();
	~CImageData();
};
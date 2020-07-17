#pragma once


#pragma pack(push,1)

struct TGA_HEADER
{
	char idLength;
	char ColorMapType;
	char ImageType;
	WORD ColorMapFirst;
	WORD ColorMapLast;
	char ColorMapBits;
	WORD FirstX;
	WORD FirstY;
	WORD width;
	WORD height;
	char Bits;
	char Descriptor;
};
#pragma pack(pop)

class CTGAImage
{
	DWORD				m_dwWidth;
	DWORD				m_dwHeight;
	char*				m_pRawImage;
	DWORD				m_dwBytePerPixel;


	void				Destroy();
	BOOL				LoadTGAImage(FILE* fp, DWORD dwBytesPerPixel);

public:
	DWORD	GetWidth() { return m_dwWidth; }
	DWORD	GetHeight() { return m_dwHeight; }
	char*	GetRawImage() { return m_pRawImage; }
	BOOL	Load24BitsTGA(char* szFileName, DWORD dwBytesPerPixel);
	DWORD	Blt(char* pDest, int iDestX, int iDestY, DWORD dwScreenWidth, DWORD dwScreenHeight, DWORD dwPitch, DWORD dwBytePerPixel, int iSrcStartX, int iSrcStartY, int iSrcWidth, int iSrcHeight);
	DWORD	GetPixel(DWORD x, DWORD y);
	CTGAImage();
	~CTGAImage();
};

BOOL WriteTGA(const char* szFileName, char* pSrc, DWORD dwWidth, DWORD dwHeight, DWORD dwPitch, DWORD bpp);
DWORD WriteTGAImage(FILE* fp, char* pSrc, DWORD dwWidth, DWORD dwHeight, DWORD dwPitch, DWORD bpp);
#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "TGAImage.h"


CTGAImage::CTGAImage()
{
	m_dwWidth = 0;
	m_dwHeight = 0;
	m_pRawImage = 0;
	m_dwBytePerPixel = 0;

}

BOOL CTGAImage::Load24BitsTGA(char* szFileName, DWORD dwBytesPerPixel)
{
	BOOL			bResult = FALSE;

	FILE*			fp = NULL;


	fopen_s(&fp, szFileName, "rb");

	if (!fp)
		goto lb_return;

	bResult = LoadTGAImage(fp, dwBytesPerPixel);

	fclose(fp);

	bResult = TRUE;

lb_return:
	return bResult;
}
BOOL CTGAImage::LoadTGAImage(FILE* fp, DWORD dwBytesPerPixel)
{
	BOOL	bResult = FALSE;


	Destroy();

	TGA_HEADER	header;
	memset(&header, 0, sizeof(TGA_HEADER));


	fread(&header, sizeof(header), 1, fp);
	/*
		// 이미지 타잎이 이 중 하나가 아니라면 이 파일은 로드할 수 없다.
		if (ImageType!=0x01 &&
			ImageType!=0x02 &&
			ImageType!=0x03 &&
			ImageType!=0x09 &&
			ImageType!=0x0a &&
			ImageType!=0x0b)
		{
			fclose(fp);
			return FALSE;
		}*/

	unsigned char* temp = new unsigned char[header.width * header.height * 4];
	memset(temp, 0, header.width * header.height * 4);

	DWORD		dwSize = 3 * header.width * header.height;

	fread(temp, sizeof(BYTE), dwSize, fp);

	DWORD		dwWidthBytes;
	DWORD		y, x, count = 0;
	DWORD		line = header.height;

	m_pRawImage = new char[header.width * header.height * 4]; // 이미지를 위한 메모리할당


	if (dwBytesPerPixel == 2)
	{
		dwWidthBytes = header.width * 2;
		m_dwBytePerPixel = 2;


		for (y = 0; y < header.height; y++)
		{
			line--;
			for (x = 0; x < header.width; x++)
			{
				*(WORD*)(m_pRawImage + line * header.width * 2 + x * 2) =
					temp[y * header.width * 3 + x * 3 + 0] >> 3 |
					temp[y * header.width * 3 + x * 3 + 1] >> 2 << 5 |
					temp[y * header.width * 3 + x * 3 + 2] >> 3 << 11;
			}
		}
		/*
				if (header.Descriptor)
				{

				}
				else
				{
					for (y=0; y<header.height; y++)
					{
						for (x=0; x<header.width; x++)
						{
							*(WORD*)(m_pRawImage+y*header.width*2+x*2) =
								temp[y*header.width*3+x*3+0]>>3 |
								temp[y*header.width*3+x*3+1]>>2<<5 |
								temp[y*header.width*3+x*3+2]>>3<<11;

						}
					}
				}*/
	}
	else if (dwBytesPerPixel == 4)
	{

		m_dwBytePerPixel = 4;
		dwWidthBytes = header.width * 4;

		for (y = 0; y < header.height; y++)
		{
			line--;
			for (x = 0; x < header.width; x++)
			{
				m_pRawImage[line * dwWidthBytes + x * 4 + 0] = temp[y * header.width * 3 + x * 3 + 0];
				m_pRawImage[line * dwWidthBytes + x * 4 + 1] = temp[y * header.width * 3 + x * 3 + 1];
				m_pRawImage[line * dwWidthBytes + x * 4 + 2] = temp[y * header.width * 3 + x * 3 + 2];
				m_pRawImage[line * dwWidthBytes + x * 4 + 3] = (BYTE)255;
			}
		}
		/*
				if (header.Descriptor)
				{

				}
				else
				{
					for (y=0; y<header.height; y++)
					{
						for (x=0; x<header.width; x++)
						{
							m_pRawImage[y*dwWidthBytes + x*4+0] = temp[y*header.width*3+x*3+0];
							m_pRawImage[y*dwWidthBytes + x*4+1] = temp[y*header.width*3+x*3+1];
							m_pRawImage[y*dwWidthBytes + x*4+2] = temp[y*header.width*3+x*3+2];
							m_pRawImage[y*dwWidthBytes + x*4+3] = (BYTE)255;

						}
					}
				}*/
	}
	delete[] temp;
	temp = NULL;

	m_dwWidth = header.width;
	m_dwHeight = header.height;

	bResult = TRUE;

	return bResult;
}


DWORD CTGAImage::Blt(char* pDestBits, int iDestX, int iDestY, DWORD dwScreenWidth, DWORD dwScreenHeight, DWORD dwPitch, DWORD dwBytePerPixel, int iSrcStartX, int iSrcStartY, int iSrcWidth, int iSrcHeight)
{






	DWORD					dwPixelNum = 0;

	int						dest_start_x, dest_start_y, dest_end_x, dest_end_y;


	int						iScreenStartX = iDestX;
	int						iScreenEndX = (int)dwScreenWidth - 1;

	int						iScreenStartY = iDestY;
	int						iScreenEndY = (int)dwScreenHeight - 1;

	DWORD					dwSrcPitch = m_dwWidth * m_dwBytePerPixel;

	if (!m_pRawImage)
		goto lb_return;

	if (dwBytePerPixel != m_dwBytePerPixel)
	{
		__debugbreak();
		goto lb_return;
	}


	// dest
	// 수평 클리핑
	dest_start_x = iDestX;
	dest_end_x = iDestX + iSrcWidth - 1;


	if (dest_start_x > iScreenEndX)
		goto lb_return;

	if (dest_end_x < 0)
		goto lb_return;


	if (dest_start_x < 0)
	{
		iSrcWidth += dest_start_x;
		iSrcStartX -= dest_start_x;
		dest_start_x = 0;
	}

	int	iMargine = iScreenEndX - dest_end_x;
	if (iMargine < 0)
	{
		iSrcWidth += iMargine;
	}

	// 수직 클리핑
	dest_start_y = iDestY;
	dest_end_y = iDestY + iSrcHeight - 1;


	if (dest_start_y > iScreenEndY)
		goto lb_return;

	if (dest_end_y < 0)
		goto lb_return;

	if (dest_start_y < 0)
	{
		iSrcHeight += dest_start_y;
		iSrcStartY -= dest_start_y;
		dest_start_y = 0;
	}

	iMargine = iScreenEndY - dest_end_y;
	if (iMargine < 0)
	{
		iSrcHeight += iMargine;
	}


	char*		pSrc = m_pRawImage + (iSrcStartX * m_dwBytePerPixel) + (iSrcStartY * dwSrcPitch);
	char*		pDest = pDestBits + (dest_start_x * dwBytePerPixel) + (dest_start_y * dwPitch);

	for (int y = 0; y < iSrcHeight; y++)
	{

		memcpy(pDest, pSrc, iSrcWidth * dwBytePerPixel);
		pSrc += dwSrcPitch;
		pDest += dwPitch;
		dwPixelNum += iSrcWidth;

	}


lb_return:
	return dwPixelNum;

}
BOOL WriteTGA(const char* szFileName, char* pSrc, DWORD dwWidth, DWORD dwHeight, DWORD dwPitch, DWORD bpp)
{
	FILE*	fp = NULL;

	BOOL	bResult = FALSE;

	fopen_s(&fp, szFileName, "wb");
	if (!fp)
		goto lb_return;

	DWORD	dwWriteSize = WriteTGAImage(fp, pSrc, dwWidth, dwHeight, dwPitch, bpp);

	bResult = TRUE;
	fclose(fp);

lb_return:
	return bResult;
}
DWORD WriteTGAImage(FILE* fp, char* pSrc, DWORD dwWidth, DWORD dwHeight, DWORD dwPitch, DWORD bpp)
{
	DWORD		dwOldPos = ftell(fp);

	TGA_HEADER	header;
	memset(&header, 0, sizeof(header));


	header.width = (WORD)dwWidth;
	header.height = (WORD)dwHeight;
	header.Bits = 24;
	header.ImageType = 2;

	BYTE		r;
	BYTE		g;
	BYTE		b;
	fwrite(&header, sizeof(header), 1, fp);

	DWORD	dwSize = dwWidth * dwHeight * 3;
	char* pBits = new char[dwSize];
	char* pDest = pBits;

	int	x, y;

	if (bpp == 16)
	{
		WORD		wPixel;
		for (y = (int)dwHeight - 1; y >= 0; y--)
		{
			for (x = 0; x < (int)dwWidth; x++)
			{
				wPixel = *(WORD*)(pSrc + (x << 1) + dwPitch * y);
				r = (wPixel & 0x001f) << 3;
				g = (wPixel & 0x07e0) >> 5 << 2;
				b = (wPixel & 0xf800) >> 11 << 3;

				*(pDest + 0) = r;
				*(pDest + 1) = g;
				*(pDest + 2) = b;
				/*
				fwrite(&r,1,1,fp);
				fwrite(&g,1,1,fp);
				fwrite(&b,1,1,fp);
				*/

				pDest += 3;

			}
		}
	}
	else if (bpp == 32)
	{
		for (y = (int)dwHeight - 1; y >= 0; y--)
		{
			for (x = 0; x < (int)dwWidth; x++)
			{

				*(pDest + 0) = *((pSrc + (x << 2) + dwPitch * y) + 0);
				*(pDest + 1) = *((pSrc + (x << 2) + dwPitch * y) + 1);
				*(pDest + 2) = *((pSrc + (x << 2) + dwPitch * y) + 2);

				pDest += 3;
			}
		}
	}
	else if (bpp == 24)
	{
		for (y = (int)dwHeight - 1; y >= 0; y--)
		{
			for (x = 0; x < (int)dwWidth; x++)
			{

				*(pDest + 0) = *((pSrc + (x * 3) + dwPitch * y) + 0);
				*(pDest + 1) = *((pSrc + (x * 3) + dwPitch * y) + 1);
				*(pDest + 2) = *((pSrc + (x * 3) + dwPitch * y) + 2);

				pDest += 3;
			}
		}
	}

	fwrite(pBits, sizeof(char), dwSize, fp);

	delete[] pBits;

	return ftell(fp) - dwOldPos;

}
DWORD CTGAImage::GetPixel(DWORD x, DWORD y)
{
	DWORD dwColor = *(DWORD*)(m_pRawImage + ((x + y * m_dwWidth) * 4));
	return dwColor;
}
void CTGAImage::Destroy()
{
	if (m_pRawImage)
	{
		delete[] m_pRawImage;
		m_pRawImage = NULL;
	}
}
CTGAImage::~CTGAImage()
{
	Destroy();
}

#include "stdafx.h"
#include "CPU_Filter.h"

#pragma pack(push,1)
struct RGBA
{
	BYTE	b;
	BYTE	g;
	BYTE	r;
	BYTE	a;
};
#pragma pack(pop)

float SAMPLE_MASK_3_3_BARTLETT[3 * 3] =
{
	1,  1, 1,
	1,  1, 1,
	1,  1, 1
};

float SAMPLE_MASK_3_3_LAPLACIAN[3 * 3] =
{
	-1,-1,-1,
	-1, 8,-1,
	-1,-1,-1
};

float SAMPLE_MASK_3_3_GAUSSIAN[3 * 3] =
{
	1,  4, 1,
	4, 12, 4,
	1,  4, 1
};

const int MASK_3_3_WIDTH = 3;
const int MASK_3_3_HEIGHT = 3;
const int MASK_3_3_CENTER_X = 1;
const int MASK_3_3_CENTER_Y = 1;

float SAMPLE_MASK_5_5_GAUSSIAN[5 * 5] =
{
	1,  4, 8, 4, 1,
	4, 12,15,12, 4,
	8, 25,49,25, 8,
	4, 12,15,12, 4,
	1,  4, 8, 4, 1
};

const int MASK_5_5_WIDTH = 5;
const int MASK_5_5_HEIGHT = 5;
const int MASK_5_5_CENTER_X = 2;
const int MASK_5_5_CENTER_Y = 2;

DWORD SampleBlurPixel32_CPU(char* pBits, int iWidth, int iHeight, int iPitch, int sx, int sy, float* pMask, int iMaskWidth, int iMaskHeight, int iMaskCenterX, int iMaskCenterY)
{
	//float	mask_center_value = pMask[iMaskCenterX + iMaskWidth*iMaskCenterY];

	int		src_start_x = sx - iMaskCenterX;

	int		mask_start_x = 0;
	int		mask_start_y = 0;

	if (src_start_x < 0)
		mask_start_x = 0 - src_start_x;

	int		src_start_y = sy - iMaskCenterY;

	if (src_start_y < 0)
		mask_start_y = 0 - src_start_y;

	int		src_end_x = src_start_x + iMaskWidth;
	int		src_end_y = src_start_y + iMaskHeight;

	if (src_start_x < 0)
		src_start_x = 0;

	if (src_start_y < 0)
		src_start_y = 0;

	if (src_end_x > iWidth)
		src_end_x = iWidth;

	if (src_end_y > iHeight)
		src_end_y = iHeight;

	int		sample_width = src_end_x - src_start_x;
	int		sample_height = src_end_y - src_start_y;

	float	color[4];
	float	total_color[4] = { 0,0,0,0 };
	DWORD	R, G, B, A;

	float	weight;
	DWORD	dwPixelOut, dwPixelIn;

	float	total = 0.0f;
	for (int y = 0; y < sample_height; y++)
	{
		for (int x = 0; x < sample_width; x++)
		{
			total += pMask[(x + mask_start_x) + (y + mask_start_y) * iMaskWidth];
		}
	}


	DWORD*		pPixel;
	for (int y = 0; y < sample_height; y++)
	{
		for (int x = 0; x < sample_width; x++)
		{
			weight = pMask[(x + mask_start_x) + (y + mask_start_y) * iMaskWidth] / total;
			if (weight != 0.0f)
			{
				pPixel = (DWORD*)(pBits + ((x + src_start_x) << 2) + (y + src_start_y) * iPitch);


				dwPixelIn = *pPixel;

				color[3] = (float)((dwPixelIn & 0xff000000) >> 24);
				color[2] = (float)((dwPixelIn & 0x00ff0000) >> 16);
				color[1] = (float)((dwPixelIn & 0x0000ff00) >> 8);
				color[0] = (float)(dwPixelIn & 0x000000ff);

				for (int k = 0; k < 4; k++)
				{
					total_color[k] += (color[k] * weight);
				}
			}
		}
	}
	A = (DWORD)total_color[3];
	R = (DWORD)total_color[2];
	G = (DWORD)total_color[1];
	B = (DWORD)total_color[0];


	dwPixelOut = (A << 24) | (R << 16) | (G << 8) | B;


	return dwPixelOut;


}

DWORD SampleEdgePixel32_CPU(char* pBits, int iWidth, int iHeight, int iPitch, int sx, int sy, float* pMask, int iMaskWidth, int iMaskHeight, int iMaskCenterX, int iMaskCenterY)
{
	float	mask_center_value = pMask[iMaskCenterX + iMaskWidth * iMaskCenterY];

	int		src_start_x = sx - iMaskCenterX;

	int		mask_start_x = 0;
	int		mask_start_y = 0;

	if (src_start_x < 0)
		mask_start_x = 0 - src_start_x;

	int		src_start_y = sy - iMaskCenterY;

	if (src_start_y < 0)
		mask_start_y = 0 - src_start_y;

	int		src_end_x = src_start_x + iMaskWidth;
	int		src_end_y = src_start_y + iMaskHeight;

	if (src_start_x < 0)
		src_start_x = 0;

	if (src_start_y < 0)
		src_start_y = 0;

	if (src_end_x > iWidth)
		src_end_x = iWidth;

	if (src_end_y > iHeight)
		src_end_y = iHeight;

	int		sample_width = src_end_x - src_start_x;
	int		sample_height = src_end_y - src_start_y;

	float	color[4];
	float	total_color[4] = { 0,0,0,0 };

	float	weight;
	DWORD	dwPixelOut, dwPixelIn;



	DWORD*		pPixel;
	for (int y = 0; y < sample_height; y++)
	{
		for (int x = 0; x < sample_width; x++)
		{
			weight = pMask[(x + mask_start_x) + (y + mask_start_y) * iMaskWidth];
			pPixel = (DWORD*)(pBits + ((x + src_start_x) << 2) + (y + src_start_y) * iPitch);

			dwPixelIn = *pPixel;

			color[3] = (float)((dwPixelIn & 0xff000000) >> 24);
			color[2] = (float)((dwPixelIn & 0x00ff0000) >> 16);
			color[1] = (float)((dwPixelIn & 0x0000ff00) >> 8);
			color[0] = (float)(dwPixelIn & 0x000000ff);

			for (int k = 0; k < 4; k++)
			{
				total_color[k] += (color[k] * weight);
			}
		}
	}

	int	bw = (DWORD)((total_color[2] * 0.3f) + (total_color[1] * 0.59f) + (total_color[0] * 0.11f));


	bw = 255 - bw;

	if (bw > 255)
		bw = 255;

	if (bw < 0)
		bw = 0;


	dwPixelOut = (bw << 24) | (bw << 16) | (bw << 8) | bw;

	return dwPixelOut;


}


void CPU_Blur_Filter(char* pDest, char* pSrc, DWORD dwWidth, DWORD dwHeight)
{
	DWORD	dwPitch = dwWidth * 4;

	DWORD	start_y = 0;
	DWORD	end_y = start_y + dwHeight;

	for (DWORD y = start_y; y < end_y; y++)
	{
		for (DWORD x = 0; x < dwWidth; x++)
		{
			DWORD	dwPixel = SampleBlurPixel32_CPU(pSrc, dwWidth, dwHeight, dwPitch, x, y, SAMPLE_MASK_5_5_GAUSSIAN, MASK_5_5_WIDTH, MASK_5_5_HEIGHT, MASK_5_5_CENTER_X, MASK_5_5_CENTER_Y);

			DWORD*	pDestColor = (DWORD*)pDest + x + (y * dwWidth);
			*pDestColor = dwPixel;
		}
	}

}
void CPU_Edge_Filter(char* pDest, char* pSrc, DWORD dwWidth, DWORD dwHeight)
{
	DWORD	dwPitch = dwWidth * 4;

	DWORD	start_y = 0;
	DWORD	end_y = start_y + dwHeight;

	for (DWORD y = start_y; y < end_y; y++)
	{
		for (DWORD x = 0; x < dwWidth; x++)
		{
			DWORD	dwPixel = SampleEdgePixel32_CPU(pSrc, dwWidth, dwHeight, dwPitch, x, y, SAMPLE_MASK_3_3_LAPLACIAN, MASK_3_3_WIDTH, MASK_3_3_HEIGHT, MASK_3_3_CENTER_X, MASK_3_3_CENTER_Y);
			DWORD*	pDestColor = (DWORD*)pDest + x + (y * dwWidth);
			*pDestColor = dwPixel;
		}
	}

}
void CPU_BW_Filter(char* pDest, char* pSrc, DWORD dwWidth, DWORD dwHeight)
{
	DWORD	dwPitch = dwWidth * 4;

	DWORD	start_y = 0;
	DWORD	end_y = start_y + dwHeight;

	for (DWORD y = start_y; y < end_y; y++)
	{
		for (DWORD x = 0; x < dwWidth; x++)
		{
			RGBA*	pSrcColor = (RGBA*)pSrc + x + (y * dwWidth);
			RGBA*	pDestColor = (RGBA*)pDest + x + (y * dwWidth);

			int		iBright = (int)(((float)pSrcColor->r * 0.3f) + ((float)pSrcColor->g * 0.59f) + ((float)pSrcColor->b * 0.11f));

			pDestColor->a = (BYTE)iBright;
			pDestColor->r = (BYTE)iBright;
			pDestColor->g = (BYTE)iBright;
			pDestColor->b = (BYTE)iBright;
		}
	}
}

#pragma once

#include <windows.h>
#include "Filter_typedef.h"

void CPU_BW_Filter(char* pDest, char* pSrc, DWORD dwWidth, DWORD dwHeight);
void CPU_Edge_Filter(char* pDest, char* pSrc, DWORD dwWidth, DWORD dwHeight);
void CPU_Blur_Filter(char* pDest, char* pSrc, DWORD dwWidth, DWORD dwHeight);


struct IMAGE_PROCESS_DESC
{
	char*		pSrc;
	char*		pDest;
	FILTER_TYPE	FilterType;
	DWORD		dwThreadNum;
	DWORD		Width;
	DWORD		Height;

};


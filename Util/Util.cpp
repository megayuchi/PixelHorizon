#include "stdafx.h"
#include <Windows.h>
#include "../Common/typedef.h"
#include "Util.h"


BOOL CalcClipArea(INT_VECTOR2* pivOutSrcStart, INT_VECTOR2* pivOutDestStart, INT_VECTOR2* pivOutDestSize, const INT_VECTOR2* pivPos, const INT_VECTOR2* pivImageSize, const INT_VECTOR2* pivBufferSize)
{
	BOOL	bResult = FALSE;
	//
	int dest_start_x = max(pivPos->x, 0);
	int dest_start_y = max(pivPos->y, 0);
	dest_start_x = min(dest_start_x, pivBufferSize->x);
	dest_start_y = min(dest_start_y, pivBufferSize->y);

	int dest_end_x = max(pivPos->x + pivImageSize->x, 0);
	int dest_end_y = max(pivPos->y + pivImageSize->y, 0);
	dest_end_x = min(dest_end_x, pivBufferSize->x);
	dest_end_y = min(dest_end_y, pivBufferSize->y);

	int	width = dest_end_x - dest_start_x;
	int	height = dest_end_y - dest_start_y;

	if (width <= 0 || height <= 0)
		goto lb_return;

	int src_start_x = dest_start_x - pivPos->x;
	int src_start_y = dest_start_y - pivPos->y;

	pivOutSrcStart->x = src_start_x;
	pivOutSrcStart->y = src_start_y;
	pivOutDestStart->x = dest_start_x;
	pivOutDestStart->y = dest_start_y;
	pivOutDestSize->x = width;
	pivOutDestSize->y = height;
	bResult = TRUE;
lb_return:
	return bResult;
}

BOOL IsCollisionRectVsRect(const INT_VECTOR2* pv3MinA, const INT_VECTOR2* pv3MaxA, const INT_VECTOR2* pv3MinB, const INT_VECTOR2* pv3MaxB)
{
	const int*	a_min = &pv3MinA->x;
	const int*	a_max = &pv3MaxA->x;
	const int*	b_min = &pv3MinB->x;
	const int*	b_max = &pv3MaxB->x;

	for (DWORD i = 0; i < 2; i++)
	{
		if (a_min[i] > b_max[i] || a_max[i] < b_min[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}
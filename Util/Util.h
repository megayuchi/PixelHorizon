#pragma once

BOOL CalcClipArea(INT_VECTOR2* pivOutSrcStart, INT_VECTOR2* pivOutDestStart, INT_VECTOR2* pivOutDestSize, const INT_VECTOR2* pivPos, const INT_VECTOR2* pivImageSize, const INT_VECTOR2* pivBufferSize);
BOOL IsCollisionRectVsRect(const INT_VECTOR2* pv3MinA, const INT_VECTOR2* pv3MaxA, const INT_VECTOR2* pv3MinB, const INT_VECTOR2* pv3MaxB);
#include "stdafx.h"
#include <Windows.h>
#include "../Common/typedef.h"
#include "../ImageData/ImageData.h"
#include "FlightObject.h"
#include "Game.h"
#include "game_typedef.h"

CFlightObject::CFlightObject()
{
}

int CFlightObject::GetWidth() const
{
	int iWidth = (int)m_pImgData->GetWidth();
	return iWidth;
}
int CFlightObject::GetHeight() const
{
	int iHeight = (int)m_pImgData->GetHeight();
	return iHeight;
}
void CFlightObject::GetInterpolatedPos(INT_VECTOR2* pivOutPos) const
{
	*pivOutPos = ivInterpolatedPos;
}
void CFlightObject::GetPos(INT_VECTOR2* pivOutPos) const
{
	*pivOutPos = ivPos;
}
void CFlightObject::SetPos(int x, int y, BOOL bInterpolation)
{
	if (bInterpolation)
	{
		ivOldPos = ivPos;
		//ivInterpolatedPos = ivPos;
	}
	else
	{
		ivOldPos.x = x;
		ivOldPos.y = y;
		ivInterpolatedPos.x = x;
		ivInterpolatedPos.y = y;
	}
	ivPos.x = x;
	ivPos.y = y;
}
void CFlightObject::SetPos(const INT_VECTOR2* pivPos, BOOL bInterpolation)
{
	if (bInterpolation)
	{
		ivOldPos = ivPos;
		//ivInterpolatedPos = ivPos;
	}
	else
	{
		ivOldPos = *pivPos;
		ivInterpolatedPos = *pivPos;
	}
	ivPos = *pivPos;
}

void CFlightObject::FixPos()
{
	ivOldPos = ivPos;
	ivInterpolatedPos = ivPos;
}
void CFlightObject::Interpolate(float fAlpha)
{
	if (fAlpha < 0.0f)
		fAlpha = 0.0f;

	if (fAlpha > 1.0f)
		fAlpha = 1.0f;

	float x_vel = (float)ivPos.x - (float)ivOldPos.x;
	ivInterpolatedPos.x = (int)((float)ivOldPos.x + x_vel * fAlpha);

	float y_vel = (float)ivPos.y - (float)ivOldPos.y;
	ivInterpolatedPos.y = (int)((float)ivOldPos.y + y_vel * fAlpha);
}

CFlightObject::~CFlightObject()
{
}
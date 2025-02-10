#include "stdafx.h"
#include <Windows.h>
#include "../Common/typedef.h"
#include "../ImageData/ImageData.h"
#include "FlightObject.h"
#include "GameObject.h"
#include "Game.h"
#include "game_typedef.h"


CFlightObject* CreatePlayer(CImageData* pImgData, int x, int y, int iSpeed)
{
	CFlightObject*	pNewObject = new CFlightObject;
	pNewObject->SetImageData(pImgData);
	pNewObject->SetPos(x, y, FALSE);
	//pNewObject->x = (g_iScreenWidth / 2) - IMG_WIDTH_FIGHTER_TYPE0 / 2;
	//pNewObject->y = (g_iScreenHeight / 2) - IMG_HEIGHT_FIGHTER_TYPE0 / 2;
	pNewObject->SetStatus(FLIGHT_OBJECT_STATUS_ALIVE);
	pNewObject->SetSpeed(iSpeed);

	return pNewObject;
}

CFlightObject* CreateEnemyRandom(CImageData* pImgData, int iScreenWidth, int iScreenHeight, int iSpeed)
{
	CFlightObject*	pNewObject = new CFlightObject;
	pNewObject->SetImageData(pImgData);

	int pos_x = rand() % (iScreenWidth - pImgData->GetWidth());

	pNewObject->SetPos(pos_x, 0, FALSE);
	pNewObject->SetStatus(FLIGHT_OBJECT_STATUS_ALIVE);
	pNewObject->SetSpeed(iSpeed);

	return pNewObject;
}


CFlightObject* CreateAmmo(CFlightObject* pShooter, CImageData* pImgData, int iSpeed)
{
	CFlightObject*	pAmmo = new CFlightObject;
	pAmmo->SetImageData(pImgData);
	pAmmo->SetStatus(FLIGHT_OBJECT_STATUS_ALIVE);

	INT_VECTOR2		ivShooterPos;
	pShooter->GetPos(&ivShooterPos);
	pAmmo->SetPos(ivShooterPos.x + pShooter->GetWidth() / 2, ivShooterPos.y, FALSE);
	pAmmo->SetSpeed(iSpeed);

	return pAmmo;
}
void DeleteFlightObject(CFlightObject* pObject)
{
	delete pObject;
}

void ChangeFlightObjectStatusToDead(CFlightObject* pObject, ULONGLONG CurTick)
{
	pObject->SetStatus(FLIGHT_OBJECT_STATUS_DEAD);
	pObject->SetDeadTick(CurTick);
}
BOOL MoveEmemy(CFlightObject* pObj, int iScreenWidth)
{
	if (pObj->GetStatus() != FLIGHT_OBJECT_STATUS_ALIVE)
		return FALSE;

	INT_VECTOR2	ivPos;
	pObj->GetPos(&ivPos);
	int move_x = (rand() % 3) - 2;
	//ivPos.x += (move_x * pObj->iSpeed);

	if (ivPos.x < 0)
	{
		ivPos.x = 0;
	}
	if (ivPos.x + pObj->GetWidth() > iScreenWidth)
	{
		ivPos.x -= (ivPos.x + pObj->GetWidth() - iScreenWidth);
	}
	//ivPos.y++;
	//ivPos.y += pObj->iSpeed;
	ivPos.y += 10;
	pObj->SetPos(&ivPos, TRUE);
	return TRUE;
}
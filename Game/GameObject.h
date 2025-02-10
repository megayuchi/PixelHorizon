#pragma once

class CFlightObject;
class CImageData;

CFlightObject* CreatePlayer(CImageData* pImgData, int x, int y, int iSpeed);
CFlightObject* CreateEnemyRandom(CImageData* pImgData, int iScreenWidth, int iScreenHeight, int iSpeed);
CFlightObject* CreateAmmo(CFlightObject* pShooter, CImageData* pImgData, int iSpeed);
void DeleteFlightObject(CFlightObject* pObject);
BOOL MoveEmemy(CFlightObject* pObj, int iScreenWidth);
void ChangeFlightObjectStatusToDead(CFlightObject* pObject, ULONGLONG CurTick);
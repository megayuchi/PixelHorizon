#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include "../DDrawLib/DDraw.h"
#include "../Util/Util.h"
#include "../Common/typedef.h"
#include "../Util/TGAImage.h"
#include "../ImageData/ImageData.h"
#include "../Util/QueryPerfCounter.h"
#include "FlightObject.h"
#include "GameObject.h"
#include "Game.h"
#include "game_typedef.h"


CGame* g_pGame = nullptr;

CGame::CGame()
{
	m_dwCurFPS = 0;
	m_fTicksPerGameFrame = 1000.0f / (float)m_dwGameFPS;
	m_PrvGameFrameTick = GetTickCount64();
	m_PrvCounter = QCGetCounter();

}


CFlightObject*	m_pPlayer = nullptr;


CFlightObject*	m_ppAmmoList[MAX_AMMO_NUM] = {};
DWORD m_dwCurAmmoNum = 0;

CFlightObject*	m_ppEnemyList[MAX_ENEMY_NUM] = {};
DWORD m_dwCurEnemiesNum = 0;



BOOL CGame::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;

	m_pDDraw = new CDDraw;
	m_pDDraw->InitializeDDraw(hWnd);

	int iScreenWidth = (int)m_pDDraw->GetWidth();
	int iScreenHeight = (int)m_pDDraw->GetHeight();

	srand(GetTickCount());

	CTGAImage* pPlayerImage = new CTGAImage;
	if (pPlayerImage->Load24BitsTGA("./data/galaga_player.tga", 4))
	{
		DWORD dwPlayerColorKey = pPlayerImage->GetPixel(0, 0);

		m_pPlayerImgData = new CImageData;
		m_pPlayerImgData->Create(pPlayerImage->GetRawImage(), pPlayerImage->GetWidth(), pPlayerImage->GetHeight(), dwPlayerColorKey);
	}
	delete pPlayerImage;
	pPlayerImage = nullptr;

	CTGAImage* pAmmoImage = new CTGAImage;
	if (pAmmoImage->Load24BitsTGA("./data/ammo.tga", 4))
	{
		DWORD	dwAmmoColorKey = pAmmoImage->GetPixel(0, 0);
		m_pAmmoImgData = new CImageData;
		m_pAmmoImgData->Create(pAmmoImage->GetRawImage(), pAmmoImage->GetWidth(), pAmmoImage->GetHeight(), dwAmmoColorKey);
	}
	delete pAmmoImage;
	pAmmoImage = nullptr;

	CTGAImage* pEnemyImage = new CTGAImage;
	if (pEnemyImage->Load24BitsTGA("./data/galaga_enemy.tga", 4))
	{
		DWORD dwEnemyColorKey = pEnemyImage->GetPixel(5, 0);

		m_pEnemyImgData = new CImageData;
		m_pEnemyImgData->Create(pEnemyImage->GetRawImage(), pEnemyImage->GetWidth(), pEnemyImage->GetHeight(), dwEnemyColorKey);
	}
	delete pEnemyImage;
	pEnemyImage = nullptr;

	//CImageData*	m_pEnemyImgData = nullptr;
	//m_pCircleImage = new CTGAImage;;
	//m_pCircleImage->Load24BitsTGA("./data/circle.tga", 4);
	//m_dwCircleColorKey = m_pCircleImage->GetPixel(0, 0);
	//m_pCircleImgData = new CImageData;
	//m_pCircleImgData->Create(m_pCircleImage->GetRawImage(), m_pCircleImage->GetWidth(), m_pCircleImage->GetHeight(), m_dwCircleColorKey);

	// 중간 스크롤 이미지
	CTGAImage* pMidScrollImage = new CTGAImage;
	if (pMidScrollImage->Load24BitsTGA("./data/mid_scroll_image.tga", 4))
	{
		DWORD dwMidScrollImageColorKey = pMidScrollImage->GetPixel(5, 0);

		m_pMidScrollImageData = new CImageData;
		m_pMidScrollImageData->Create(pMidScrollImage->GetRawImage(), pMidScrollImage->GetWidth(), pMidScrollImage->GetHeight(), dwMidScrollImageColorKey);
	}
	delete pMidScrollImage;
	pMidScrollImage = nullptr;	

	m_pBackImage = new CTGAImage;
	m_pBackImage->Load24BitsTGA("./data/yuki_back.tga", 4);

	int iPlayerPosX = (iScreenWidth - (int)m_pPlayerImgData->GetWidth()) / 2;
	int iPlayerPosY = (iScreenHeight - (int)m_pPlayerImgData->GetHeight()) / 2;

	m_pPlayer = CreatePlayer(m_pPlayerImgData, iPlayerPosX, iPlayerPosY, DEFAULT_PLAYER_SPEED);


	return TRUE;
}
void CGame::Process()
{
	LARGE_INTEGER	CurCounter = QCGetCounter();
	float	fElpasedTick = QCMeasureElapsedTick(CurCounter, m_PrvCounter);
	ULONGLONG CurTick = GetTickCount64();

	if (fElpasedTick > m_fTicksPerGameFrame)
	{
		FixPostionPostion();

		OnGameFrame(CurTick);
		m_PrvGameFrameTick = CurTick;
		m_PrvCounter = CurCounter;
		
	}
	else
	{
		float fAlpha = fElpasedTick / m_fTicksPerGameFrame;
		InterpolatePostion(fAlpha);
	}

	DrawScene();
}

void CGame::OnKeyDown(UINT nChar, UINT uiScanCode)
{
	switch (nChar)
	{
		case VK_LEFT:
			{
				m_bKeyDown_Left = TRUE;
			}
			break;
		case VK_RIGHT:
			{
				m_bKeyDown_Right = TRUE;
			}
			break;
		case VK_UP:
			{
				m_bKeyDown_Up = TRUE;
			}
			break;
		case VK_DOWN:
			{
				m_bKeyDown_Down = TRUE;
			}
			break;
		case VK_SPACE:
			{
				ShootFromPlayer();
			}
			break;
		case VK_RETURN:
			{
				//CaptureBackBuffer("backbuffer.tga");
			}
			break;
	}
}
void CGame::OnKeyUp(UINT nChar, UINT uiScanCode)
{
	switch (nChar)
	{
		case VK_LEFT:
			{
				m_bKeyDown_Left = FALSE;
			}
			break;
		case VK_RIGHT:
			{
				m_bKeyDown_Right = FALSE;
			}
			break;
		case VK_UP:
			{
				m_bKeyDown_Up = FALSE;
			}
			break;
		case VK_DOWN:
			{
				m_bKeyDown_Down = FALSE;
			}
			break;
	}
}
BOOL CGame::OnSysKeyDown(UINT nChar, UINT uiScanCode, BOOL bAltKeyDown)
{
	BOOL	bResult = FALSE;
	switch (nChar)
	{
		case VK_F9:
			{
				if (bAltKeyDown)
				{
					CaptureBackBuffer("backbuffer.tga");
					bResult = TRUE;

				}

			}
			break;
			

	}
	return bResult;
}
BOOL CGame::CaptureBackBuffer(char* szFileName)
{
	BOOL	bResult = FALSE;

	FILE*	fp = nullptr;
	fopen_s(&fp, szFileName, "wb");
	if (!fp)
		goto lb_return;
	
	if (m_pDDraw)
	{
		m_pDDraw->CaptureBackBuffer(fp);
	}
	fclose(fp);
	bResult = TRUE;
lb_return:
	return bResult;
}

void CGame::DrawScene()
{
	// 메시지가 없으면 게임루프

	m_pDDraw->BeginDraw();


	if (m_pBackImage)
	{
		m_pDDraw->DrawBitmap(m_iBackImagePosX, m_iBackImagePosY, m_pBackImage->GetWidth(), m_pBackImage->GetHeight(), m_pBackImage->GetRawImage());
	}
	else
	{

		m_pDDraw->Clear();
	}

	// mid scroll image
	m_pDDraw->DrawImageData(m_iMidScrollImagePosX, m_iMidScrollImagePosY, m_pMidScrollImageData);
	

	//m_pDDraw->DrawRect(sx, sy, iBoxWidth, iBoxHeight, 0xff00ff00);

	// Draw player
	INT_VECTOR2		ivPos;
	m_pPlayer->GetInterpolatedPos(&ivPos);
	//m_pPlayer->GetPos(&ivPos);
	DrawFlightObject(m_pPlayer, ivPos.x, ivPos.y);

	for (DWORD i = 0; i < m_dwCurEnemiesNum; i++)
	{
		m_ppEnemyList[i]->GetInterpolatedPos(&ivPos);
		DrawFlightObject(m_ppEnemyList[i], ivPos.x, ivPos.y);
	}

	for (DWORD i = 0; i < m_dwCurAmmoNum; i++)
	{
		m_ppAmmoList[i]->GetInterpolatedPos(&ivPos);
		DrawFlightObject(m_ppAmmoList[i], ivPos.x, ivPos.y);
	}
	m_pDDraw->EndDraw();

	HDC	hDC = nullptr;
	if (m_pDDraw->BeginGDI(&hDC))
	{
		m_pDDraw->DrawInfo(hDC);
		m_pDDraw->EndGDI(hDC);
	}
	m_pDDraw->OnDraw();
	m_pDDraw->CheckFPS();
	/*




	// Draw Enemies


	// Draw ammos

	//DrawFPS(1, 1);
	//DrawScore(m_iScreenWidth - 16, 1);

	Blt();
	*/
}
void CGame::OnUpdateWindowSize()
{
	if (m_pDDraw)
	{
		m_pDDraw->OnUpdateWindowSize();
		//UpdateBackImagePos();
	}
}
void CGame::OnUpdateWindowPos()
{
	if (m_pDDraw)
	{
		m_pDDraw->OnUpdateWindowPos();
		//		UpdateBackImagePos();
	}
}
void CGame::DrawFlightObject(CFlightObject* pFighter, int x, int y)
{
	m_pDDraw->DrawImageData(x, y, pFighter->GetImageData());
}
void CGame::InterpolatePostion(float fAlpha)
{
	if (m_pPlayer)
	{
		m_pPlayer->Interpolate(fAlpha);
	}

	for (DWORD i = 0; i < m_dwCurEnemiesNum; i++)
	{
		CFlightObject*	pEnemy = m_ppEnemyList[i];
		pEnemy->Interpolate(fAlpha);
	}
	for (DWORD i = 0; i < m_dwCurAmmoNum; i++)
	{
		CFlightObject*	pAmmo = m_ppAmmoList[i];
		pAmmo->Interpolate(fAlpha);
	}

}
void CGame::FixPostionPostion()
{
	if (m_pPlayer)
	{
		m_pPlayer->FixPos();
	}

	for (DWORD i = 0; i < m_dwCurEnemiesNum; i++)
	{
		CFlightObject*	pEnemy = m_ppEnemyList[i];
		pEnemy->FixPos();
	}
	for (DWORD i = 0; i < m_dwCurAmmoNum; i++)
	{
		CFlightObject*	pAmmo = m_ppAmmoList[i];
		pAmmo->FixPos();
	}

}
void CGame::OnGameFrame(ULONGLONG CurTick)
{

	int iScreenWidth = (int)m_pDDraw->GetWidth();
	int iScreenHeight = (int)m_pDDraw->GetHeight();

	UpdatePlayerPos(iScreenWidth, iScreenHeight);

	// 사망한 적들 제거

	DeleteDestroyedEnemies(CurTick);
	ProcessCollision(CurTick);

	// 탄환이 맵 끝에 도달하면 자동 파괴
	DWORD	dwIndex = 0;
	while (dwIndex < m_dwCurAmmoNum)
	{
		CFlightObject*	pAmmo = m_ppAmmoList[dwIndex];
		INT_VECTOR2	ivPos;
		pAmmo->GetPos(&ivPos);
		if (ivPos.y < 0)
		{
			DeleteFlightObject(pAmmo);
			m_dwCurAmmoNum--;
			m_ppAmmoList[dwIndex] = m_ppAmmoList[m_dwCurAmmoNum];
			m_ppAmmoList[m_dwCurAmmoNum] = nullptr;
		}
		else
		{
			//ivPos.y--;
			ivPos.y -= pAmmo->GetSpeed();
			pAmmo->SetPos(&ivPos, TRUE);
			dwIndex++;
		}
	}

	ProcessEnemies();

	UpdateBackground();

}
void CGame::UpdateBackground()
{
	/*
	static ULONGLONG PrvUpdateTick = 0;
	ULONGLONG CurTick = GetTickCount64();

	if (CurTick - PrvUpdateTick < 1000)
		return;

	PrvUpdateTick = CurTick;

	DWORD	dwTotalPixels = (DWORD)(m_iScreenWidth * m_iScreenHeight);


	WCHAR* pSrc = m_pBackground + (m_iScreenHeight - 1) * m_iScreenWidth;
	WCHAR* pDest = m_pBackgroundLineBuffer;

	for (int y = 0; y < m_iScreenHeight; y++)
	{
		memcpy(pDest, pSrc, sizeof(WCHAR) * m_iScreenWidth);
		pDest = pSrc;
		pSrc -= m_iScreenWidth;
	}
	memcpy(m_pBackground, m_pBackgroundLineBuffer, sizeof(WCHAR) * m_iScreenWidth);
	//memcpy(m_pBackground, m_pBackground + m_iScreenWidth, sizeof(WCHAR) * (dwTotalPixels - (DWORD)m_iScreenWidth));
	//memcpy(m_pBackground + (dwTotalPixels - (DWORD)m_iScreenWidth), m_pBackgroundLineBuffer, sizeof(WCHAR) * m_iScreenWidth);
	*/
}
void CGame::UpdatePlayerPos(int iScreenWidth, int iScreenHeight)
{
	INT_VECTOR2		ivPlayerPos;
	m_pPlayer->GetPos(&ivPlayerPos);


	if (m_bKeyDown_Left)
	{
		//m_iPlayerPosX--;
		ivPlayerPos.x -= m_pPlayer->GetSpeed();
	}
	if (m_bKeyDown_Right)
	{
		//m_iPlayerPosX++;
		ivPlayerPos.x += m_pPlayer->GetSpeed();
	}
	if (m_bKeyDown_Up)
	{
		//m_iPlayerPosY--;
		ivPlayerPos.y -= m_pPlayer->GetSpeed();
	}
	if (m_bKeyDown_Down)
	{
		//m_iPlayerPosY++;
		ivPlayerPos.y += m_pPlayer->GetSpeed();
	}

	if (m_pPlayerImgData)
	{
		int iPlayerImageWidth = (int)m_pPlayerImgData->GetWidth();
		int iPlayerImageHeight = (int)m_pPlayerImgData->GetHeight();

		if (ivPlayerPos.x < -(iPlayerImageWidth / 2))
		{
			ivPlayerPos.x = -(iPlayerImageWidth / 2);
			m_iBackImagePosX++;
			m_iMidScrollImagePosX += 2;
		}
		if (ivPlayerPos.x > iScreenWidth - iPlayerImageWidth + (iPlayerImageWidth / 2))
		{
			ivPlayerPos.x = iScreenWidth - iPlayerImageWidth + (iPlayerImageWidth / 2);
			m_iBackImagePosX--;
			m_iMidScrollImagePosX -= 2;
		}

		if (ivPlayerPos.y < -(iPlayerImageHeight / 2))
		{
			ivPlayerPos.y = -(iPlayerImageHeight / 2);
			m_iBackImagePosY++;
			m_iMidScrollImagePosY += 2;
		}
		if (ivPlayerPos.y > iScreenHeight - iPlayerImageHeight + (iPlayerImageHeight / 2))
		{
			ivPlayerPos.y = iScreenHeight - iPlayerImageHeight + (iPlayerImageHeight / 2);
			m_iBackImagePosY--;
			m_iMidScrollImagePosY -= 2;
		}
	}
	if (m_pBackImage)
	{
		int iBackImageWidth = (int)m_pBackImage->GetWidth();
		int iBackImageHeight = (int)m_pBackImage->GetHeight();

		if (iBackImageWidth > iScreenWidth)
		{
			if (m_iBackImagePosX > 0)
			{
				m_iBackImagePosX = 0;
			}
			if (m_iBackImagePosX < iScreenWidth - iBackImageWidth)
			{
				m_iBackImagePosX = iScreenWidth - iBackImageWidth;
			}
		}
		if (iBackImageHeight > iScreenHeight)
		{
			if (m_iBackImagePosY > 0)
			{
				m_iBackImagePosY = 0;
			}
			if (m_iBackImagePosY < iScreenHeight - iBackImageHeight)
			{
				m_iBackImagePosY = iScreenHeight - iBackImageHeight;
			}
		}
	}
	if (m_pMidScrollImageData)
	{
		int iMidScrollImageWidth = (int)m_pMidScrollImageData->GetWidth();
		int iMidScrollImageHeight = (int)m_pMidScrollImageData->GetHeight();

		if (iMidScrollImageWidth > iScreenWidth)
		{
			if (m_iMidScrollImagePosX > 0)
			{
				m_iMidScrollImagePosX = 0;
			}
			if (m_iMidScrollImagePosX < iScreenWidth - iMidScrollImageWidth)
			{
				m_iMidScrollImagePosX = iScreenWidth - iMidScrollImageWidth;
			}
		}
		if (iMidScrollImageHeight > iScreenHeight)
		{
			if (m_iMidScrollImagePosY > 0)
			{
				m_iMidScrollImagePosY = 0;
			}
			if (m_iMidScrollImagePosY < iScreenHeight - iMidScrollImageHeight)
			{
				m_iMidScrollImagePosY = iScreenHeight - iMidScrollImageHeight;
			}
		}
	}
	m_pPlayer->SetPos(&ivPlayerPos, TRUE);
}
void CGame::DeleteDestroyedEnemies(ULONGLONG CurTick)
{
	DWORD	dwIndex = 0;
	while (dwIndex < m_dwCurEnemiesNum)
	{
		CFlightObject*	pEnemy = m_ppEnemyList[dwIndex];
		if (pEnemy->GetStatus() == FLIGHT_OBJECT_STATUS_DEAD && CurTick - pEnemy->GetDeadTick() > DEAD_STATUS_WAIT_TICK)
		{
			DeleteFlightObject(pEnemy);
			m_dwCurEnemiesNum--;
			m_ppEnemyList[dwIndex] = m_ppEnemyList[m_dwCurEnemiesNum];
			m_ppEnemyList[m_dwCurEnemiesNum] = nullptr;
		}
		else
		{
			dwIndex++;
		}
	}
}
void CGame::ProcessCollision(ULONGLONG CurTick)
{
	DWORD	dwIndex = 0;
	while (dwIndex < m_dwCurAmmoNum)
	{
		CFlightObject*	pAmmo = m_ppAmmoList[dwIndex];
		if (ProcessCollisionAmmoVsEnemies(pAmmo, CurTick))
		{
			DeleteFlightObject(pAmmo);
			m_dwCurAmmoNum--;
			m_ppAmmoList[dwIndex] = m_ppAmmoList[m_dwCurAmmoNum];
			m_ppAmmoList[m_dwCurAmmoNum] = nullptr;
		}
		else
		{
			dwIndex++;
		}
	}
}


BOOL CGame::IsCollisionFlightObjectVsFlightObject(const CFlightObject* pObj0, const CFlightObject* pObj1)
{
	BOOL bResult = FALSE;

	if (pObj0->GetStatus() != FLIGHT_OBJECT_STATUS_ALIVE || pObj1->GetStatus() != FLIGHT_OBJECT_STATUS_ALIVE)
	{
		return FALSE;
	}
	INT_VECTOR2	ivPos0;
	pObj0->GetPos(&ivPos0);

	INT_VECTOR2	ivPos1;
	pObj1->GetPos(&ivPos1);

	INT_RECT2	objRect0 =
	{
		ivPos0.x, ivPos0.y,
		ivPos0.x + pObj0->GetWidth(), ivPos0.y + pObj0->GetHeight()
	};
	INT_RECT2	objRect1 =
	{
		ivPos1.x, ivPos1.y,
		ivPos1.x + pObj1->GetWidth(), ivPos1.y + pObj1->GetHeight()
	};

	bResult = IsCollisionRectVsRect(&objRect0.min, &objRect0.max, &objRect1.min, &objRect1.max);

lb_return:
	return bResult;
}
BOOL CGame::ProcessCollisionAmmoVsEnemies(CFlightObject* pAmmo, ULONGLONG CurTick)
{
	BOOL	bCollision = FALSE;
	for (DWORD i = 0; i < m_dwCurEnemiesNum; i++)
	{
		CFlightObject*	pEnemy = m_ppEnemyList[i];
		if (IsCollisionFlightObjectVsFlightObject(pAmmo, pEnemy))
		{
			// 적 타격 성공
			OnHitEnemy(pEnemy, CurTick);

			bCollision = TRUE;
			break;
		}
	}
	return bCollision;
}
void CGame::ProcessEnemies()
{
	int iScreenWidth = (int)m_pDDraw->GetWidth();
	int iScreenHeight = (int)m_pDDraw->GetHeight();

	// 적이 맵 끝(바닥)에 도달하면 자동파괴
	DWORD	dwIndex = 0;
	while (dwIndex < m_dwCurEnemiesNum)
	{
		CFlightObject*	pEnemy = m_ppEnemyList[dwIndex];
		INT_VECTOR2		ivPos;
		pEnemy->GetPos(&ivPos);
		if (ivPos.y >= iScreenHeight)
		{
			DeleteFlightObject(pEnemy);
			m_dwCurEnemiesNum--;
			m_ppEnemyList[dwIndex] = m_ppEnemyList[m_dwCurEnemiesNum];
			m_ppEnemyList[m_dwCurEnemiesNum] = nullptr;
		}
		else
		{
			dwIndex++;
		}
	}
	MoveEnemies();
	FillEnemies();
}
void CGame::OnHitEnemy(CFlightObject* pEnemy, ULONGLONG CurTick)
{
	// 지금 삭제하지 않고 상태만 바꾼다.
	ChangeFlightObjectStatusToDead(pEnemy, CurTick);
	AddScore(SCORE_PER_ONE_KILL);
}
DWORD CGame::AddScore(DWORD dwAddval)
{
	/*
	m_dwCurScore += dwAddval;
	return m_dwCurScore;
	*/
	return 0;
}
void CGame::MoveEnemies()
{
	int iScreenWidth = (int)m_pDDraw->GetWidth();
	int iScreenHeight = (int)m_pDDraw->GetHeight();

	static ULONGLONG PrvEnemyMoveTick = 0;
	ULONGLONG CurTick = GetTickCount64();
	if (CurTick - PrvEnemyMoveTick < ENEMY_MOVE_ACTION_DELAY_TICK)
	{
		return;
	}
	PrvEnemyMoveTick = CurTick;

	for (DWORD i = 0; i < m_dwCurEnemiesNum; i++)
	{
		MoveEmemy(m_ppEnemyList[i], iScreenWidth);
	}
}

void CGame::FillEnemies()
{
	static ULONGLONG PrvFillEnemyTick = 0;

	int iScreenWidth = (int)m_pDDraw->GetWidth();
	int iScreenHeight = (int)m_pDDraw->GetHeight();

	if (m_dwCurEnemiesNum >= MAX_ENEMY_NUM)
	{
		return;
	}

	ULONGLONG CurTick = GetTickCount64();
	if (CurTick - PrvFillEnemyTick < 3000)
	{
		return;
	}
	PrvFillEnemyTick = CurTick;

	DWORD	dwNeyEnemyNum = MAX_ENEMY_NUM - m_dwCurEnemiesNum;
	for (DWORD i = 0; i < dwNeyEnemyNum; i++)
	{
		CFlightObject*	pEnemy = CreateEnemyRandom(m_pEnemyImgData, iScreenWidth, iScreenHeight, DEFAULT_ENEMY_SPEED);
		m_ppEnemyList[m_dwCurEnemiesNum] = pEnemy;
		m_dwCurEnemiesNum++;
	}
}
void CGame::DeleteAllEnemies()
{
	for (DWORD i = 0; i < m_dwCurEnemiesNum; i++)
	{
		DeleteFlightObject(m_ppEnemyList[i]);
		m_ppEnemyList[i] = nullptr;
	}
	m_dwCurEnemiesNum = 0;
}
void CGame::DeleteAllAmmos()
{
	for (DWORD i = 0; i < m_dwCurAmmoNum; i++)
	{
		DeleteFlightObject(m_ppAmmoList[i]);
		m_ppAmmoList[i] = nullptr;
	}
	m_dwCurAmmoNum = 0;
}
void CGame::Cleanup()
{
	DeleteAllEnemies();
	DeleteAllAmmos();
	if (m_pPlayer)
	{
		DeleteFlightObject(m_pPlayer);
		m_pPlayer = nullptr;
	}
	if (m_pPlayerImgData)
	{
		delete m_pPlayerImgData;
		m_pPlayerImgData = nullptr;
	}
	if (m_pAmmoImgData)
	{
		delete m_pAmmoImgData;
		m_pAmmoImgData = nullptr;
	}
	if (m_pEnemyImgData)
	{
		delete m_pEnemyImgData;
		m_pEnemyImgData = nullptr;
	}
	//if (m_pCircleImage)
	//{
	//	delete m_pCircleImage;
	//	m_pCircleImage = nullptr;
	//}
	//if (m_pCircleImgData)
	//{
	//	delete m_pCircleImgData;
	//	m_pCircleImgData = nullptr;
	//}
	if (m_pMidScrollImageData)
	{
		delete m_pMidScrollImageData;
		m_pMidScrollImageData = nullptr;
	}
	if (m_pBackImage)
	{
		delete m_pBackImage;
		m_pBackImage = nullptr;
	}
	if (m_pDDraw)
	{
		delete m_pDDraw;
		m_pDDraw = nullptr;
	}
}
void CGame::ShootFromPlayer()
{
	if (m_dwCurAmmoNum >= MAX_AMMO_NUM)
		return;

	CFlightObject*	pAmmo = CreateAmmo(m_pPlayer, m_pAmmoImgData, DEFAULT_AMMO_SPEED);
	m_ppAmmoList[m_dwCurAmmoNum] = pAmmo;
	m_dwCurAmmoNum++;
}

void CGame::DrawScore(int x, int y)
{
	/*
	DWORD	dwOffset = x + y * m_iScreenWidth;
	WCHAR*	wchDest = m_pBackBuffer + dwOffset;
	DWORD	dwDesBufferLen = (DWORD)((m_iScreenWidth * m_iScreenHeight - (int)dwOffset));

	WCHAR	wchTxt[32];
	DWORD	dwLen = swprintf_s(wchTxt, L"Score:%08u", m_dwCurScore);
	memcpy(wchDest, wchTxt, sizeof(WCHAR) * dwLen);
	*/
}
CGame::~CGame()
{
	Cleanup();
}
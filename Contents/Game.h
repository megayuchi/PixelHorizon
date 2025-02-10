#pragma once

#include "game_typedef.h"
class CFlightObject;
class CTGAImage;
class CImageData;
class CDDraw;
class CGame
{
	CDDraw*	m_pDDraw = nullptr;
	HWND	m_hWnd = nullptr;
	DWORD m_dwGameFPS = 60;
	DWORD m_dwCurFPS = 0;
	float m_fTicksPerGameFrame = 16.6f;
	ULONGLONG m_PrvGameFrameTick = 0;
	LARGE_INTEGER	m_PrvCounter = {};

	
	CImageData*	m_pPlayerImgData = nullptr;
	CImageData*	m_pAmmoImgData = nullptr;
	CImageData*	m_pEnemyImgData = nullptr;
	CImageData*	m_pMidScrollImageData = nullptr;
	CTGAImage*	m_pBackImage = nullptr;

	//CTGAImage*	m_pCircleImage = nullptr;
	//CImageData*	m_pCircleImgData = nullptr;


	
	



	int	m_iCursorPosX = 0;
	int m_iCursorPosY = 0;

//	int m_iPlayerPosX = 0;
//	int m_iPlayerPosY = 0;
	int m_iBackImagePosX = 0;
	int m_iBackImagePosY = 0;

	int m_iMidScrollImagePosX = 0;
	int m_iMidScrollImagePosY = 0;

	BOOL m_bKeyDown_Up = FALSE;
	BOOL m_bKeyDown_Down = FALSE;
	BOOL m_bKeyDown_Left = FALSE;
	BOOL m_bKeyDown_Right = FALSE;


	CFlightObject*	m_pPlayer = nullptr;
	CFlightObject*	m_ppAmmoList[MAX_AMMO_NUM] = {};
	DWORD m_dwCurAmmoNum = 0;

	CFlightObject*	m_ppEnemyList[MAX_ENEMY_NUM] = {};
	DWORD m_dwCurEnemiesNum = 0;

	// control game status
	void InterpolatePostion(float fAlpha);
	void FixPostionPostion();
	void OnGameFrame(ULONGLONG CurTick);
	void ProcessEnemies();
	void OnHitEnemy(CFlightObject* pEnemy, ULONGLONG CurTick);
	DWORD AddScore(DWORD dwAddval);
	void FillEnemies();
	void MoveEnemies();
	BOOL IsCollisionFlightObjectVsFlightObject(const CFlightObject* pObj0, const CFlightObject* pObj1);
	BOOL ProcessCollisionAmmoVsEnemies(CFlightObject* pAmmo, ULONGLONG CurTick);
	void ProcessCollision(ULONGLONG CurTick);
	void DeleteDestroyedEnemies(ULONGLONG CurTick);
	void ShootFromPlayer();
	void DeleteAllAmmos();
	void DeleteAllEnemies();
	void UpdateBackground();
	void UpdatePlayerPos(int iScreenWidth, int iScreenHeight);

	// display
	void DrawScore(int x, int y);
	void DrawScene();
	void DrawFlightObject(CFlightObject* pFighter, int x, int y);

	// keyboard input
	

public:

	BOOL	Initialize(HWND hWnd);
	void	OnKeyDown(UINT nChar, UINT uiScanCode);
	void	OnKeyUp(UINT nChar, UINT uiScanCode);
	BOOL	OnSysKeyDown(UINT nChar, UINT uiScanCode, BOOL bAltKeyDown);
	void	OnUpdateWindowSize();
	void	OnUpdateWindowPos();
	void Process();
	void Cleanup();

	BOOL	CaptureBackBuffer(char* szFileName);
	CGame();
	~CGame();
};

extern CGame* g_pGame;

#pragma once

enum FLIGHT_OBJECT_STATUS
{
	FLIGHT_OBJECT_STATUS_ALIVE,
	FLIGHT_OBJECT_STATUS_DEAD,
};

class CImageData;
class CFlightObject
{

	INT_VECTOR2	ivOldPos = {};
	INT_VECTOR2	ivPos{};
	INT_VECTOR2	ivInterpolatedPos = {};

	const CImageData*	m_pImgData = nullptr;
	FLIGHT_OBJECT_STATUS m_status = FLIGHT_OBJECT_STATUS_ALIVE;
	ULONGLONG	m_DeadTick = 0;	// »ç¸ÁÇÑ ½Ã°£
	int		m_iSpeed = 0;

public:
	ULONGLONG	GetDeadTick() const { return m_DeadTick; }
	void		SetDeadTick(ULONGLONG Tick) { m_DeadTick = Tick; }
	FLIGHT_OBJECT_STATUS GetStatus() const { return m_status; }
	void SetStatus(FLIGHT_OBJECT_STATUS status) { m_status = status; }

	const CImageData* GetImageData() const { return m_pImgData; }
	void SetImageData(const CImageData* pImgData) { m_pImgData = pImgData; }
	int GetSpeed() const { return m_iSpeed; }
	void SetSpeed(int iSpeed) { m_iSpeed = iSpeed; }
	int GetWidth() const;
	int GetHeight() const;
	void GetInterpolatedPos(INT_VECTOR2* pivOutPos) const;
	void GetPos(INT_VECTOR2* pivOutPos) const;
	void SetPos(int x, int y, BOOL bInterpolation);
	void SetPos(const INT_VECTOR2* pivPos, BOOL bInterpolation);
	void FixPos();
	void Interpolate(float fAlpha);
	CFlightObject();
	~CFlightObject();
};
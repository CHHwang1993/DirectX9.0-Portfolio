#pragma once

#define TIMEMANAGER timeManager::GetInstance()


#include "timer.h"

class timeManager
{
	SINGLETONE(timeManager)

private:
	timer* _timer;
	LPD3DXFONT m_pFont;
	DWORD m_dwLastUpdateTime;

public:
	//초기화
	HRESULT init(void);

	//릴리즈
	void release(void);

	//프레임 적용
	void update(float lock = 0.0f);

	//해당 HDC에 정보 출력한다
	void render(void);

	//전체 시간 가져온다
	inline float getWorldTime(void) const { return _timer->getWorldTime(); }
	//프레임 경과시간
	inline float getElapsedTime(void) const { return _timer->getElapsedTime(); }

	inline float getLastUpdateTime(void) const { return m_dwLastUpdateTime / 1000.0f; }
};


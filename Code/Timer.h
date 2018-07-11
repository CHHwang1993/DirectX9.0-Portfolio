#pragma once

class timer
{
private:
	bool		_isHardware;		//성능 좋은 타이머 지원 되냐?
	float		_timeScale;			//시간의 경과량 조절
	float		_timeElapsed;		//마지막 시간과 현재 시간의 경과량
	__int64		_curTime;			//현재시간
	__int64		_lastTime;			//이전시간
	__int64		_periodFrequency;	//시간주기

	unsigned int	_frameRate;			//FPS
	unsigned int	_FPSFrameCount;		//FPS 카운트
	float			_FPSTimeElapsed;	//FPS 경과량
	float			_worldTime;			//게임 시작후부터 흘러간 전체 시간

public:
	//초기화
	HRESULT init(void);

	//현재 시간을 계산
	void tick(float lockFPS = 0.0f);

	//현재 FPS
	unsigned long getFrameRate(wchar_t* str = NULL) const;

	//마지막 시간과 현재 시간의 경과량
	inline float getElapsedTime(void) const { return _timeElapsed; }

	//전체 경과 시간
	inline float getWorldTime(void) const { return _worldTime; }

	timer(void) {};
	~timer(void) {};
};
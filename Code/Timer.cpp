#include "StdAfx.h"
#include "timer.h"
#include <MMSystem.h>

//TimeGetTime 함수를 사용하기 위한 라이브러리!!!
#pragma comment(lib, "winmm.lib")

//초기화
HRESULT timer::init(void)
{
	//타이머(고성능?) 지원이 되는지 체크한다
	//지원이 가능하면 _periodFrequency 값에 초당 파악할 수 있는
	//시간의 값이 들어온다!!
	//초당 밀리쎄컨 값이 들어감!!
	if (QueryPerformanceFrequency((LARGE_INTEGER*)&_periodFrequency))
	{
		_isHardware = true;
		QueryPerformanceCounter((LARGE_INTEGER*)&_lastTime);

		//초당 시간을 계산할 수 있는 수치
		_timeScale = 1.0f / _periodFrequency;
	}
	else
	{
		_isHardware = false;
		_lastTime = timeGetTime();
		_timeScale = 0.001f;
	}

	_frameRate = 0;
	_FPSFrameCount = 0;
	_FPSTimeElapsed = 0.0f;
	_worldTime = 0.0f;

	return S_OK;
}

//현재 시간을 계산
void timer::tick(float lockFPS)
{
	//지원이 되면 현재 시간을 얻어온다
	if (_isHardware)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&_curTime);
	}
	else
	{
		//지원하지 않으면 time 함수를 이용!!
		_curTime = timeGetTime();
	}

	//마지막 시간과 현재 시간의 경과량을 측정한다!!!!!!!
	_timeElapsed = (_curTime - _lastTime) * _timeScale;

	//고정 프레임 처리해준다 (전문용어로 프레임스키핑)
	if (lockFPS >= 0.0f)
	{
		//고정 프레임의 시간에 도달할때 까지 루프 돈다!!
		while (_timeElapsed < (1.0f / lockFPS))
		{
			if (_isHardware) QueryPerformanceCounter((LARGE_INTEGER*)&_curTime);
			else _curTime = timeGetTime();

			//마지막 시간과 현재 시간의 경과량을 측정한다!!!!!!!
			_timeElapsed = (_curTime - _lastTime) * _timeScale;
		}
	}

	//마지막 시간을 기록한다
	_lastTime = _curTime;

	//초당 프레임 카운트 증가시킨다
	_FPSFrameCount++;

	//초당 프레임 시간 경과량 증가
	_FPSTimeElapsed += _timeElapsed;

	//전체 시간 경과량 증가!!
	_worldTime += _timeElapsed;

	//프레임 초기화 (1초마다!!)
	if (_FPSTimeElapsed > 1.0f)
	{
		_frameRate = _FPSFrameCount;
		_FPSFrameCount = 0;
		_FPSTimeElapsed = 0.0f;
	}
}

//현재 FPS
unsigned long timer::getFrameRate(wchar_t* str) const
{
	return _frameRate;
}

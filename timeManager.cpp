#include "stdafx.h"
#include "timeManager.h"

timeManager::timeManager(void)
{
}

timeManager::~timeManager(void)
{
}

//초기화
HRESULT timeManager::init(void)
{
	m_pFont = NULL;

	//타이머 생성 및 초기화
	_timer = new timer;
	_timer->init();

	// font
	D3DXFONT_DESC	fd;
	ZeroMemory(&fd, sizeof(D3DXFONT_DESC));

	fd.Height = 15;
	fd.Width = 10;
	fd.Weight = FW_BOLD;
	fd.Italic = false;
	fd.CharSet = DEFAULT_CHARSET;
	fd.OutputPrecision = OUT_DEFAULT_PRECIS;
	fd.PitchAndFamily = FF_DONTCARE;
	strcpy_s(fd.FaceName, "모리스9");

	D3DXCreateFontIndirect(g_pD3DDevice, &fd, &m_pFont);

	return S_OK;
}

//릴리즈
void timeManager::release(void)
{
	SAFE_DELETE(_timer);
	SAFE_RELEASE(m_pFont);
}

//프레임 적용
void timeManager::update(float lock)
{
	if (_timer != NULL)
	{
		_timer->tick(lock);

		DWORD dwCurrTime = GetTickCount();

		m_dwLastUpdateTime = dwCurrTime;
	}
}

//해당 HDC에 정보 출력한다
void timeManager::render(void)
{
	char str[256];
	std::string strFrame;

	/*SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 255, 255));*/

#ifdef _DEBUG
	if (_timer != NULL)
	{
		//wchar_t* str = L"이것이 굴림체다";
		RECT rc;
		SetRect(&rc, WINSIZE_X-180, 10, WINSIZE_X, WINSIZE_Y);

		wchar_t str[32];
		
		////프레임 출력
		//_snwprintf_s(str, wcslen(str), L"framePerSec : %d", (int)_timer->getFrameRate());
		//m_pFont->DrawTextW(NULL,str, wcslen(str),&rc,DT_LEFT | DT_TOP,D3DCOLOR_XRGB(255, 255, 255, 255));
		//
		//SetRect(&rc, WINSIZE_X - 180, 30, WINSIZE_X, WINSIZE_Y);
		//
		//////월드 타임 출력
		//_snwprintf_s(str, wcslen(str), L"worldTime : %f", _timer->getWorldTime());
		//m_pFont->DrawTextW(NULL, str, wcslen(str), &rc, DT_LEFT | DT_TOP, D3DCOLOR_XRGB(255, 255, 255, 255));
		//
		//SetRect(&rc, WINSIZE_X- 180, 50, WINSIZE_X, WINSIZE_Y);
		////갱신 Tick 값 출력
		//_snwprintf_s(str, wcslen(str), L"elapsedTime : %.2f", _timer->getElapsedTime());
		//m_pFont->DrawTextW(NULL, str, wcslen(str), &rc, DT_LEFT | DT_TOP, D3DCOLOR_XRGB(255, 255, 255, 255));
	}
#else
	if (_timer != NULL)
	{
		//프레임 출력
		sprintf_s(str, "framePerSec : %d", _timer->getFrameRate());
		TextOut(hdc, 0, 0, str, strlen(str));
	}
#endif
}


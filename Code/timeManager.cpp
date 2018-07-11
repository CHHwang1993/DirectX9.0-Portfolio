#include "stdafx.h"
#include "timeManager.h"

timeManager::timeManager(void)
{
}

timeManager::~timeManager(void)
{
}

//�ʱ�ȭ
HRESULT timeManager::init(void)
{
	m_pFont = NULL;

	//Ÿ�̸� ���� �� �ʱ�ȭ
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
	strcpy_s(fd.FaceName, "�𸮽�9");

	D3DXCreateFontIndirect(g_pD3DDevice, &fd, &m_pFont);

	return S_OK;
}

//������
void timeManager::release(void)
{
	SAFE_DELETE(_timer);
	SAFE_RELEASE(m_pFont);
}

//������ ����
void timeManager::update(float lock)
{
	if (_timer != NULL)
	{
		_timer->tick(lock);

		DWORD dwCurrTime = GetTickCount();

		m_dwLastUpdateTime = dwCurrTime;
	}
}

//�ش� HDC�� ���� ����Ѵ�
void timeManager::render(void)
{
	char str[256];
	std::string strFrame;

	/*SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 255, 255));*/

#ifdef _DEBUG
	if (_timer != NULL)
	{
		//wchar_t* str = L"�̰��� ����ü��";
		RECT rc;
		SetRect(&rc, WINSIZE_X-180, 10, WINSIZE_X, WINSIZE_Y);

		wchar_t str[32];
		
		////������ ���
		//_snwprintf_s(str, wcslen(str), L"framePerSec : %d", (int)_timer->getFrameRate());
		//m_pFont->DrawTextW(NULL,str, wcslen(str),&rc,DT_LEFT | DT_TOP,D3DCOLOR_XRGB(255, 255, 255, 255));
		//
		//SetRect(&rc, WINSIZE_X - 180, 30, WINSIZE_X, WINSIZE_Y);
		//
		//////���� Ÿ�� ���
		//_snwprintf_s(str, wcslen(str), L"worldTime : %f", _timer->getWorldTime());
		//m_pFont->DrawTextW(NULL, str, wcslen(str), &rc, DT_LEFT | DT_TOP, D3DCOLOR_XRGB(255, 255, 255, 255));
		//
		//SetRect(&rc, WINSIZE_X- 180, 50, WINSIZE_X, WINSIZE_Y);
		////���� Tick �� ���
		//_snwprintf_s(str, wcslen(str), L"elapsedTime : %.2f", _timer->getElapsedTime());
		//m_pFont->DrawTextW(NULL, str, wcslen(str), &rc, DT_LEFT | DT_TOP, D3DCOLOR_XRGB(255, 255, 255, 255));
	}
#else
	if (_timer != NULL)
	{
		//������ ���
		sprintf_s(str, "framePerSec : %d", _timer->getFrameRate());
		TextOut(hdc, 0, 0, str, strlen(str));
	}
#endif
}


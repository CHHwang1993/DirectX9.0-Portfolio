#include "stdafx.h"
#include "TweakBar.h"

TweakBar* TweakBar::instance = NULL;

TweakBar * TweakBar::Get()
{
	if (instance == NULL)
		instance = new TweakBar();

	return instance;
}


void TweakBar::Delete()
{
	SAFE_DELETE(instance);
}

void TweakBar::Start()
{
	isDraw = false;

	BOOL result = TwInit(TW_DIRECT3D9, g_pD3DDevice);
	assert(result);

	bar = TwNewBar("TweakBar");
	TwDefine("TweakBar color = '255 0 0' alpha = 128");
	TwDefine("TweakBar size = '250 500'");
	TwDefine("TweakBar valuewidth=120");
	TwDefine("TweakBar movable=true");
	TwDefine("TweakBar iconified=true");
	TwDefine("TweakBar iconpos=topleft");
}

void TweakBar::InputProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	TwEventWin(wnd, msg, wParam, lParam);
}

void TweakBar::Render()
{
	TwDraw();
}

TweakBar::TweakBar()
{
}


TweakBar::~TweakBar()
{
	TwTerminate();
}

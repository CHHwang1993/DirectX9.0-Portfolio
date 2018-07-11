#pragma once

#include "BaseScene.h"
#include "BaseScene2.h"
#include "MainScene.h"

class cMainGame //메인 게임이 돌아갈 클래스
{
private:
	unsigned int bgColor;

public:
	cMainGame();
	virtual	~cMainGame();

	virtual HRESULT init();
	virtual void release();
	virtual void Update();
	virtual void Render();

	void SingleToneSetting();
	void SingleToneRelease();

};


#pragma once

#include "BaseScene.h"
#include "BaseScene2.h"
#include "MainScene.h"

class cMainGame //���� ������ ���ư� Ŭ����
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


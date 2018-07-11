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
	//�ʱ�ȭ
	HRESULT init(void);

	//������
	void release(void);

	//������ ����
	void update(float lock = 0.0f);

	//�ش� HDC�� ���� ����Ѵ�
	void render(void);

	//��ü �ð� �����´�
	inline float getWorldTime(void) const { return _timer->getWorldTime(); }
	//������ ����ð�
	inline float getElapsedTime(void) const { return _timer->getElapsedTime(); }

	inline float getLastUpdateTime(void) const { return m_dwLastUpdateTime / 1000.0f; }
};


#pragma once
class TweakBar
{
public:
	static TweakBar* Get();
	static void Delete();

	void Start();
	void InputProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Render();

	void GetDraw(bool* isDraw)
	{
		*isDraw = this->isDraw;
	}

	void SetDraw(bool isDraw)
	{
		this->isDraw = isDraw;

		if (isDraw == true)
			TwDefine("TweakBar iconified=false");
		else
			TwDefine("TweakBar iconified=true");
	}

	void ChangeDraw()
	{
		SetDraw(!isDraw);
	}

	TwBar* GetBar()
	{
		return bar;
	}

private:
	TweakBar();
	~TweakBar();

	static TweakBar* instance;

	bool isDraw;
	TwBar* bar;
};


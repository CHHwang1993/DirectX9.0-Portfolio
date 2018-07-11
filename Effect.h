#pragma once
class Effect
{
public:
	HRESULT LoadEffect(std::string fullPath); // 이펙트를 로드한다.

	void Begin(); // Effect 시작
	void End(); // Effect 종료
	void BeginPass(int index); // 패스 시작
	void EndPass(); // 패스 종료

	void CommitChange(); // 패스중 값 변경

	UINT NumPass() { return numPass; }

	//파라메터값 세팅 함수 
	void SetInt(std::string name, int value);
	void SetBool(std::string name, bool value);
	void SetFloat(std::string name, float value);
	void SetVector2(std::string name, D3DXVECTOR2 value);
	void SetVector3(std::string name, D3DXVECTOR3 value);
	void SetVector4(std::string name, D3DXVECTOR4 value);
	void SetMatrix(std::string name, D3DXMATRIXA16* value);
	void SetString(std::string name, std::string value);
	void SetTexture(std::string name, LPDIRECT3DTEXTURE9 value);
	void SetTexture(std::string name, LPDIRECT3DCUBETEXTURE9 value);
	void SetValue(std::string name, void* value, UINT size);

	//테크니크 세팅
	void SetTechnique(std::string name);


public:
	Effect();
	~Effect();
	
protected:
	LPD3DXEFFECT internalEffect;// 이펙트

	UINT numPass; // 패스번호
	bool isBegin; // Begin 시작
	bool isBeginPass; //BeginPass 시작
};


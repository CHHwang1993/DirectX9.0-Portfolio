#pragma once
class Effect
{
public:
	HRESULT LoadEffect(std::string fullPath); // ����Ʈ�� �ε��Ѵ�.

	void Begin(); // Effect ����
	void End(); // Effect ����
	void BeginPass(int index); // �н� ����
	void EndPass(); // �н� ����

	void CommitChange(); // �н��� �� ����

	UINT NumPass() { return numPass; }

	//�Ķ���Ͱ� ���� �Լ� 
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

	//��ũ��ũ ����
	void SetTechnique(std::string name);


public:
	Effect();
	~Effect();
	
protected:
	LPD3DXEFFECT internalEffect;// ����Ʈ

	UINT numPass; // �н���ȣ
	bool isBegin; // Begin ����
	bool isBeginPass; //BeginPass ����
};


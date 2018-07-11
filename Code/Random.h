#pragma once
class Random
{
private:
	static Random* instance;

public:
	static void Create();
	static void Delete();
	static Random* Get();


public:
	Random();
	~Random();

	//getInt
	int GetInt(int num) { return rand() % num; }
	int GetFromIntTo(int fromNum, int toNum)
	{
		return (rand() % (toNum - fromNum + 1)) + fromNum;
	}

	//rand 함수의 최대값 32767
	float GetFloat() { return ((float)rand() / (float)RAND_MAX); }

	float GetFloat(float num)
	{
		return ((float)rand() / (float)RAND_MAX) * num;
	}

	float GetFromFloatTo(float fromFloat, float toFloat)
	{
		float rnd = (float)rand() / (float)RAND_MAX;
		return (rnd * (toFloat - fromFloat) + fromFloat);
	}
};


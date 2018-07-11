#include "stdafx.h"
#include "Random.h"

Random* Random::instance = NULL;

void Random::Create()
{
	instance = new Random();

	srand(GetTickCount());
}

void Random::Delete()
{
	SAFE_DELETE(instance);
}

Random * Random::Get()
{
	assert(instance!=NULL);

	return instance;
}

Random::Random()
{
}


Random::~Random()
{
}

#include "bricks/core/random.h"
#include "bricks/core/time.h"

#define BRICKS_RANDOM_MAX 0x7FFFFFFF

namespace Bricks {
	Random::Random() :
		seed(Time::GetCurrentTime().GetTime())
	{

	}

	Random::Random(int seed) :
		seed(seed)
	{

	}

	void Random::Update()
	{
		seed = seed * 1103515245 + 12345;
	}

	int Random::Generate(int low, int high)
	{
		Update();
		return (seed & BRICKS_RANDOM_MAX) * (high - low) / BRICKS_RANDOM_MAX + low;
	}

	float Random::Generate(float low, float high)
	{
		Update();
		return (seed & BRICKS_RANDOM_MAX) * (high - low) / BRICKS_RANDOM_MAX + low;
	}
}

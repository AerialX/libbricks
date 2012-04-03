#pragma once

#include "bricks/core/object.h"
#include "bricks/core/sfinae.h"

namespace Bricks {
	class Random : public Object
	{
	protected:
		s64 seed;

		void Update();

	public:
		Random();
		Random(int seed);

		int Generate(int low, int high);
		float Generate(float low, float high);

		template<typename T> T Generate();
	};

	template<> inline int Random::Generate<int>() { return Generate(0, SFINAE::NumericTraits<int>::MaximumValue); }
	template<> inline float Random::Generate<float>() { return Generate(0.0f, 1.0f); }
}

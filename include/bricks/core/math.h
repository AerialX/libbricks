#pragma once

namespace Bricks { namespace Math {
	template<typename T, typename U> static inline T Min(T a, U b) { return a < b ? a : b; }
	template<typename T, typename U> static inline T Max(T a, U b) { return a > b ? a : b; }

	template<typename T> static inline T Abs(T num) { return num < 0 ? -num : num; }

	template<typename T, typename U> static inline T RoundDown(T num, U round) { return num / round * round; }
	template<typename T, typename U> static inline T RoundUp(T num, U round) { return RoundDown(num + round - 1, round); }

	template<typename T> static inline T Clamp(T num, T min = 0, T max = 1) { return Max(Min(num, max), min); }
} }

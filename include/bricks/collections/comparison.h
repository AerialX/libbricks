#pragma once

#include "bricks.h"

namespace Bricks { namespace Collections {
	namespace ComparisonResult {
		enum Enum {
			Equal	= 0,
			Greater	= 1,
			Less	= -1
		};
	}

	template<typename T>
	class ValueComparison : public virtual Object
	{
	public:
		virtual ComparisonResult::Enum Compare(const T& v1, const T& v2) = 0;
	};

	template<typename T>
	class OperatorValueComparison : public ValueComparison<T>
	{
	public:
		ComparisonResult::Enum Compare(const T& v1, const T& v2) { if (v1 > v2) return ComparisonResult::Greater; else if (v1 < v2) return ComparisonResult::Less; return ComparisonResult::Equal; }
	};

	template<typename T>
	class OperatorEqualityComparison : public ValueComparison<T>
	{
	public:
		ComparisonResult::Enum Compare(const T& v1, const T& v2) { if (v1 == v2) return ComparisonResult::Equal; return ComparisonResult::Less; }
	};
} }
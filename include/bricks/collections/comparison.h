#pragma once

#include "bricks/core/sfinae.h"
#include "bricks/core/object.h"
#include "bricks/core/autopointer.h"

namespace Bricks { namespace Collections {
	namespace ComparisonResult {
		enum Enum {
			Equal	= 0,
			Greater	= 1,
			Less	= -1
		};
	}

	template<typename T>
	class ValueComparison : public Object
	{
	public:
		virtual ComparisonResult::Enum Compare(const T& v1, const T& v2) = 0;
	};

	template<typename T, typename V = void>
	class OperatorValueComparison : public ValueComparison<T>
	{
	public:
		ComparisonResult::Enum Compare(const T& v1, const T& v2) { return ComparisonResult::Less; }
	};

	template<typename T>
	class OperatorValueComparison<T, typename SFINAE::EnableIf</*SFINAE::HasEqualityOperator<T>::Value &&*/ SFINAE::HasGreaterThanOperator<T>::Value && SFINAE::HasLessThanOperator<T>::Value>::Type> : public ValueComparison<T>
	{
	public:
		ComparisonResult::Enum Compare(const T& v1, const T& v2) { if (v1 > v2) return ComparisonResult::Greater; else if (v1 < v2) return ComparisonResult::Less; return ComparisonResult::Equal; }
	};

	template<typename T>
	class OperatorValueComparison<T, typename SFINAE::EnableIf<SFINAE::HasEqualityOperator<T>::Value && (!SFINAE::HasGreaterThanOperator<T>::Value || !SFINAE::HasLessThanOperator<T>::Value)>::Type> : public ValueComparison<T>
	{
	public:
		ComparisonResult::Enum Compare(const T& v1, const T& v2) { if (v1 == v2) return ComparisonResult::Equal; return ComparisonResult::Less; }
	};

	// TODO: Confirm by SFINAE that the addressof operator returns a pointer of type const T* to use this overload. Then use the other comparison operators.
	template<typename T>
	class OperatorValueComparison<T, typename SFINAE::EnableIf<!SFINAE::HasEqualityOperator<T>::Value && (!SFINAE::HasGreaterThanOperator<T>::Value || !SFINAE::HasLessThanOperator<T>::Value)>::Type> : public ValueComparison<T>
	{
	public:
		ComparisonResult::Enum Compare(const T& v1, const T& v2) { if (&v1 == &v2) return ComparisonResult::Equal; return ComparisonResult::Less; }
	};

	template<typename T>
	class PointerValueComparison : public ValueComparison<T*>
	{
	protected:
		AutoPointer<ValueComparison<T> > comparison;

	public:
		PointerValueComparison(ValueComparison<T>* comparison = autonew OperatorValueComparison<T>()) : comparison(comparison) { }
		ComparisonResult::Enum Compare(T*const& v1, T*const& v2) { return comparison->Compare(*v1, *v2); }
	};
} }

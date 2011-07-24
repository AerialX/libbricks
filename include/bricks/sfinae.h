#pragma once

#include "bricks/types.h"

#define BRICKS_SFINAE_TRUE(x) (sizeof(x) == sizeof(Bricks::SFINAE::True))
#define BRICKS_SFINAE_FALSE(x) (sizeof(x) == sizeof(Bricks::SFINAE::False))

namespace Bricks {
	namespace SFINAE {
		typedef struct { u8 value[3]; } True;
		typedef struct { u8 value[5]; } False;
		template<typename T> T* MakePointer();
		template<typename T> T& MakeReference();
		template<typename T> const T& MakeConstReference();

		struct AnyType { template<typename T> AnyType(const T&); };

		template<bool, typename T = void> struct EnableIf { };
		template<typename T> struct EnableIf<true, T> { typedef T Type; };

		template<bool, typename T = void> struct DisableIf { };
		template<typename T> struct DisableIf<false, T> { typedef T Type; };

		template<typename T, T t> struct ConfirmType { typedef T Type; };

		template<typename T> struct IsConst { static const bool Value = false; };
		template<typename T> struct IsConst<const T> { static const bool Value = true; };
		template<typename T> struct IsConst<const T&> { static const bool Value = true; };

		template<typename T, typename U> struct IsSameType {
			static True ConditionT(U*);
			static False ConditionT(...);
			static True ConditionU(T*);
			static False ConditionU(...);

			static const bool Value = BRICKS_SFINAE_TRUE(ConditionU(MakePointer<U>())) && BRICKS_SFINAE_TRUE(ConditionT(MakePointer<T>()));
		};

		template<typename T, typename U> struct IsCompatibleType {
			static True Condition(T*);
			static False Condition(...);

			static const bool Value = BRICKS_SFINAE_TRUE(Condition(MakePointer<U>()));
		};

		namespace Internal {
			template<typename T, typename U> static False operator ==(const T&, const U&);
			template<typename T, typename U> static False operator >(const T&, const U&);
			template<typename T, typename U> static False operator <(const T&, const U&);

			template<typename T, typename U> struct HasEqualityOperator {
				static True Condition(...);
				static False Condition(False);

				static const bool Value = BRICKS_SFINAE_TRUE(Condition(MakeConstReference<T>() == MakeConstReference<U>()));
			};

			template<typename T, typename U> struct HasGreaterThanOperator {
				static True Condition(...);
				static False Condition(False);

				static const bool Value = BRICKS_SFINAE_TRUE(Condition(MakeConstReference<T>() > MakeConstReference<U>()));
			};

			template<typename T, typename U> struct HasLessThanOperator {
				static True Condition(...);
				static False Condition(False);

				static const bool Value = BRICKS_SFINAE_TRUE(Condition(MakeConstReference<T>() < MakeConstReference<U>()));
			};
		}

		template<typename T, typename U = T> struct HasEqualityOperator {
			static const bool Value = Internal::HasEqualityOperator<T, U>::Value;
		};

		template<typename T, typename U = T> struct HasGreaterThanOperator {
			static const bool Value = Internal::HasGreaterThanOperator<T, U>::Value;
		};

		template<typename T, typename U = T> struct HasLessThanOperator {
			static const bool Value = Internal::HasLessThanOperator<T, U>::Value;
		};

		template<typename T> struct IsIntegerNumber { static const bool Value = false; };
		template<> struct IsIntegerNumber<s8> { static const bool Value = true; };
		template<> struct IsIntegerNumber<u8> { static const bool Value = true; };
		template<> struct IsIntegerNumber<s16> { static const bool Value = true; };
		template<> struct IsIntegerNumber<u16> { static const bool Value = true; };
		template<> struct IsIntegerNumber<s32> { static const bool Value = true; };
		template<> struct IsIntegerNumber<u32> { static const bool Value = true; };
		template<> struct IsIntegerNumber<s64> { static const bool Value = true; };
		template<> struct IsIntegerNumber<u64> { static const bool Value = true; };

		template<typename T> struct IsFloatingPointNumber { static const bool Value = false; };
		template<> struct IsFloatingPointNumber<f32> { static const bool Value = true; };
		template<> struct IsFloatingPointNumber<f64> { static const bool Value = true; };

		template<typename T> struct NumericTraits { };
#define BRICKS_INTERNAL_NUMERIC_TRAITS(t, min, max) template<> struct NumericTraits<t> { static const t MinimumValue = min; static const t MaximumValue = max; }
		BRICKS_INTERNAL_NUMERIC_TRAITS(s8, -0x7F, 0x80);
		BRICKS_INTERNAL_NUMERIC_TRAITS(u8, 0, 0xFF);
		BRICKS_INTERNAL_NUMERIC_TRAITS(s16, -0x7FFF, 0x8000);
		BRICKS_INTERNAL_NUMERIC_TRAITS(u16, 0, 0xFFFF);
		BRICKS_INTERNAL_NUMERIC_TRAITS(s32, -0x7FFFFFFF, 0x80000000);
		BRICKS_INTERNAL_NUMERIC_TRAITS(u32, 0, 0xFFFFFFFF);
		BRICKS_INTERNAL_NUMERIC_TRAITS(s64, -0x7FFFFFFFFFFFFFFFULL, 0x8000000000000000ULL);
		BRICKS_INTERNAL_NUMERIC_TRAITS(u64, 0, 0xFFFFFFFFFFFFFFFFULL);
#undef BRICKS_INTERNAL_NUMERIC_TRAITS
	}
}

#pragma once

#ifndef BRICKS_HEADER_BRICKS
#error Use bricks.h
#endif

namespace Bricks {
	namespace SFINAE {
		template<typename T> T* MakePointer();
		typedef u8 Yes;
		typedef u16 No;

		template<bool, typename T = void> struct EnableIf { };
		template<typename T> struct EnableIf<true, T> { typedef T Type; };

		template<typename T> struct IsConst { static const bool Value = false; };
		template<typename T> struct IsConst<const T> { static const bool Value = true; };
		template<typename T> struct IsConst<const T&> { static const bool Value = true; };

		template<typename T, typename U> struct IsSameType {
			static Yes ConditionT(U*);
			static No ConditionT(...);
			static Yes ConditionU(T*);
			static No ConditionU(...);

			static const bool Value = sizeof(ConditionU(MakePointer<U>())) == sizeof(Yes) && sizeof(ConditionT(MakePointer<T>())) == sizeof(Yes);
		};
	}
}

#pragma once

#include "bricks/threading/condition.h"

namespace Bricks { namespace Threading {
	class ConditionLock : public Condition
	{
	protected:
		vs32 condition;

	public:
		ConditionLock(int condition = 0);
		~ConditionLock();

		void SetCondition(int value) { condition = value; }
		int GetCondition() const { return condition; }

		using Condition::Lock;
		using Condition::TryLock;
		using Condition::Unlock;
		using Condition::Signal;
		using Condition::Broadcast;

		void Lock(int condition);
		bool Lock(int condition, const Timespan& timeout) { return Lock(condition, Time::GetCurrentTime() + timeout); }
		bool Lock(int condition, const Time& timeout);
		bool TryLock(int condition);
		void Unlock(int condition);
		void Signal(int condition);
		void Broadcast(int condition);
	};
} }

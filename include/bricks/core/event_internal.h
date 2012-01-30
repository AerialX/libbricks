#ifndef BRICKS_ARGLIST_COUNT
#error Use event.h
#endif

#include "bricks/collections/autoarray.h"

namespace Bricks {
	template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES> class Event<R(BRICKS_ARGLIST_TYPES)> : public Delegate<void(BRICKS_ARGLIST_TYPES)>
	{
	public:
		typedef Delegate<R(BRICKS_ARGLIST_TYPES)> EventItem;

	private:
		class EventItemComparison : public Collections::ValueComparison<EventItem*>
		{
		public:
			Collections::ComparisonResult::Enum Compare(EventItem*const& v1, EventItem*const& v2) {
				typedef const Internal::MethodFunctionBase<R(BRICKS_ARGLIST_TYPES)>* delegate;
				delegate d1 = dynamic_cast<delegate>(v1->function.GetValue());
				if (d1) {
					delegate d2 = dynamic_cast<delegate>(v2->function.GetValue());
					if (d2)
						return d1->pointer == d2->pointer ? Collections::ComparisonResult::Equal : Collections::ComparisonResult::Less;
				}
				return *v1 == *v2 ? Collections::ComparisonResult::Equal : Collections::ComparisonResult::Less;
			}
		};

		Collections::AutoArray<EventItem> list;

	public:
		Event() : list(autonew EventItemComparison()) { }

		Event& operator +=(EventItem* delegate) { list.AddItem(delegate); return *this; }
		Event& operator +=(const EventItem& delegate) { list.AddItem(autonew EventItem(delegate)); return *this; }

		Event& operator -=(EventItem* delegate) { list.RemoveItems(delegate); return *this; }
		Event& operator -=(const EventItem& delegate) { list.RemoveItems(tempnew delegate); return *this; }
		template<typename T> Event& operator -=(const T& object) { return operator-=(tempnew object); }
		template<typename T> Event& operator -=(T* object) { list.RemoveItems(tempnew EventItem(static_cast<Internal::BaseDelegate<R(BRICKS_ARGLIST_TYPES)>*>(tempnew Internal::MethodFunctionBase<R(BRICKS_ARGLIST_TYPES)>(static_cast<void*>(object))))); return *this; }

		operator bool() const { return list.GetCount(); }

		void operator ()(BRICKS_ARGLIST_TYPES_NAMES) { foreach (EventItem* item, list) (*item)(BRICKS_ARGLIST_ARGS); }
	};
}

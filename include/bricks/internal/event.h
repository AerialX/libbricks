#ifndef BRICKS_ARGLIST_COUNT
#error Use delegate.h
#endif

#include "bricks/collections/array.h"

namespace Bricks {
	template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class Event<R(BRICKS_ARGLIST_TYPES)> : public Delegate<void(BRICKS_ARGLIST_TYPES)>
	{
	public:
		typedef Delegate< R(BRICKS_ARGLIST_TYPES) > EventItem;
		typedef AutoPointer< EventItem > EventItemStorage;

	private:
		class EventItemComparison : public Bricks::Collections::ValueComparison< EventItem >
		{
		public:
			Bricks::Collections::ComparisonResult::Enum Compare(const EventItem& v1, const EventItem& v2) {
				typedef const Internal::MethodFunctionBase<R(BRICKS_ARGLIST_TYPES)>* delegate;
				delegate d1 = dynamic_cast<delegate>(v1.function.GetValue());
				if (d1) {
					delegate d2 = dynamic_cast<delegate>(v2.function.GetValue());
					if (d2)
						return d1->pointer == d2->pointer ? Bricks::Collections::ComparisonResult::Equal : Bricks::Collections::ComparisonResult::Less;
				}
				return v1 == v2 ? Bricks::Collections::ComparisonResult::Equal : Bricks::Collections::ComparisonResult::Less;
			}
		};
		AutoPointer<Collections::Collection< EventItem > > list;

	public:
		Event() : list(autonew Collections::Array< EventItem, EventItemStorage >(autonew EventItemComparison())) { }

		Event& operator +=(const Pointer<EventItem>& delegate) { list->AddItem(delegate); return *this; }
		Event& operator +=(const EventItem& delegate) { list->AddItem(CopyPointer<EventItem>(delegate)); return *this; }
		Event& operator -=(const Pointer<EventItem>& delegate) { list->RemoveItems(delegate); return *this; }
		template<typename T> Event& operator -=(const T& object) { Internal::MethodFunctionBase<R(BRICKS_ARGLIST_TYPES)> method(static_cast<void*>(const_cast<T*>(&object))); list->RemoveItems(Delegate<R(BRICKS_ARGLIST_TYPES)>(static_cast<Internal::BaseDelegate<R(BRICKS_ARGLIST_TYPES)>&>(method))); return *this; }
		template<typename T> Event& operator -=(const Pointer<T>& object) { return operator-=<T>(*object); }

		operator bool() const { return list->GetCount(); }

		void operator ()(BRICKS_ARGLIST_TYPES_NAMES) { foreach (EventItem& item, *list) item(BRICKS_ARGLIST_ARGS); }
	};
}

#ifndef BRICKS_ARGLIST_COUNT
#error Use event.h
#endif

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
				return (v1 == v2 || *v1 == *v2) ? Collections::ComparisonResult::Equal : Collections::ComparisonResult::Less;
			}
		};

		typedef Collections::AutoArray<EventItem> EventItemList;
		EventItemComparison comparison;
		EventItemList listInternal;
		Collections::ListGuard<EventItemList> list;

	public:
		Event() : listInternal(tempnew comparison), list(tempnew listInternal) { }
		Event(const Event& event) : listInternal(event.listInternal, tempnew comparison), list(tempnew listInternal) { }

		Event& operator =(const Event& event) { listInternal = EventItemList(event.listInternal, tempnew comparison); return *this; }

		Event& operator +=(EventItem* delegate) { list.AddItem(delegate); return *this; }
		Event& operator +=(const EventItem& delegate) { list.AddItem(autonew EventItem(delegate)); return *this; }

		Event& operator -=(EventItem* delegate) { list.RemoveItems(delegate); return *this; }
		Event& operator -=(const EventItem& delegate) { list.RemoveItems(tempnew delegate); return *this; }

		Event& operator -=(const Object* object) {
			BRICKS_FOR_EACH (EventItem* item, listInternal) {
				typedef const Internal::MethodFunctionBase<R(BRICKS_ARGLIST_TYPES)> FunctionDelegate;
				FunctionDelegate* d1 = CastToDynamic<FunctionDelegate>(item->function);
				if (d1 && d1->pointer == object) {
					list.RemoveItem(item);
					return operator-=(object);
				}
			}
			return *this;
		}

		operator bool() const { return list.GetCount(); }

		void operator ()(BRICKS_ARGLIST_TYPES_NAMES) { BRICKS_FOR_EACH (EventItem* item, list) (*item)(BRICKS_ARGLIST_ARGS); }

		bool IsCalling() const { return list.IsGuarding(); }
	};
}

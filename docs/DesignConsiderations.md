This document lists libbricks design decisions that have not been made yet.

alloc vs. Alloc<>
=================

Winner: new, autonew

alloc, autoalloc
----------------
 - Pros
    - It works, forwards arguments perfectly.
    - Easy to understand
 - Cons
    - It's a hack. There is no nice way of passing extra information to an object through operator new overloads. The old implementation used hacky TLS locks to pass the allocation state information along.
       - An extra (hidden) constructor argument is one solution, but it requires extra typing when writing the constructor for anything that derives from Bricks::Object.
    - The keyword conflicts with ObjC and possibly other applications.
       - The choice of a new keyword would solve this, any suggestions?

Alloc<>, AutoAlloc<>
--------------------
 - Pros
    - Less hacky, standard C++ template functions allocate objects of the given type.
 - Cons
    - Less desireable syntax, not as intuitive as a drop-in replacement for new as alloc is. It's not so bad though.
    - The forwarding problem exists. All constructor types must be determined by the compiler with the arguments alone.
       - See http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2002/n1385.htm for a description.
       - The current implementation uses a dangerous const_cast to avoid having to create exponential overloads.
       - Maybe not a problem with C++0x? But we don't want to depend on that.
       - Examples:
          - NULL, #define'd as 0, resolves to a type of long int in the template function call. It cannot then be passed on to a constructor expecting a pointer.
          - Function names must be prefixed with & to pass a function pointer to a constructor (which is proper anyway, but still)
          - Variable-length arrays cannot be passed to constructors without an explicit cast (my fault for using a C99 feature in C++03?)

new, autonew
------------
This is the new solution that was chosen for the transition to a focus on pointer types. Pointer<>s may be simply assigned with the standard C++ new. The autonew convenience macro is provided to create an AutoPointer<> (required, to keep the reference count from being one too high if you just assign a new object to an AutoPointer<> without manually Release()ing it).
The approach is to define autonew as new preceeded by an operator overload, allowing a temporary type to do something with the object before passing it back along. This could also be used in the autoalloc option to remove its hackiness cons.
 - Pros
    - Much less hacky. Just adds one new unintrusive keyword, "autonew"
    - None of the cons from the other options.

inheritance vs. virtual inheritance
===================================

Winner: standard inheritance

Inheritance
-----------
 - Pros
    - Fast and standard.
    - Encourages use of .NET style class and interface design.
 - Cons
    - Limits the logic that may go in interfaces.

Virtual Inheritance
-------------------
 - Pros
    - Multiple inheritance can be used with types that derive from Bricks::Object (read: all of them).
    - Easy to understand and just works.
 - Cons
    - Performance loss.
    - Puts heavy reliance on RTTI
    - Maybe encourages poor design behaviour and confusing inheritance graphs.

getWhatever() vs. GetWhatever()
===============================

The naming scheme in general is a disputed topic at the moment. Java vs. .NET and all that.

operator overloading vs EqualTo()
=================================

Since the migration to a focus on Pointer<> types, operator overloading will only be used for value types. Maybe. Dunno.

AutoPointer<> used as a return type in public interfaces
========================================================

Is it a safe to assume the behaviour of a function from the perspective of its interface? Is it always obvious whether an interface function should return an AutoPointer vs. a Pointer? Is it fair to restrict implementations if the interface designer decides it should be a Pointer<>? What alternatives do implementations have?

One solution is to just always use AutoPointer<>, screw it all and just needlessly retain/release whenever a function returns.
Alternatively, the Pointer<> class could be modified to behave like an AutoPointer when constructed in a certain way. There would be the added overhead of a sort of bool value per pointer, as well as a non-trivial destructor.

As much as I would like to use a less intrusive method, and leave this up to the implementation... Objective-C naming conventions define whether an object is retained or not, so libbricks should be able to use return types for the same thing. The added complexity comes from whether the object being returned is autoreleased, or just a weak pointer.

Options:
 - Public interfaces always return AutoPointer<>s. While this will "just work" it adds needless retain/releases upon function returns.
 - Make a MagicPointer<> that can be constructed to act like either a Pointer<> or AutoPointer<> on destruction. This adds extra data passed along with every return.
 - Interface design enforces either Pointer<> or AutoPointer<> return types. May cause problems with returning autoreleased pointers vs weak ones in an implementation.
    - A variant on this is to almost always return AutoPointer<>, unless it is explicitly supposed to return a weak pointer - as opposed to preferring Pointer<> return types. This is similar to the always-return-AutoPointer<> option.
 - Redesign to follow the Objective-C style: prefer working with raw pointers and just use autorelease pools.

Pointer<> vs Pointer* in public interfaces
==========================================

The pre-pointer interface used to prefer the passing of arguments by reference. Maybe this should have changed to things being passed by native C pointers rather than by Pointer<> types.

Basic concern: that the compiler is constructing a new Pointer<> object every time you call a function. This is unnecessary. If calling conventions say types by value and const references may only be used for value types, it is unnecessary to use the Pointer<> type for anything but null checking (which is a performance hit anyway).

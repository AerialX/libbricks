This document lists libbricks design decisions that have not been made yet.

alloc vs. Alloc<>
=================

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

inheritance vs. virtual inheritance
===================================

This is likely learning toward a conversion back to standard inheritance.

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

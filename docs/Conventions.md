Random Usage Notes
==================

 - Always use references where possible, and Pointer<>s or AutoPointer<>s when reseating is required.
 - The use of new on objects isn't suggested, but it works fine. Either call delete manually or Release() them.
 - Set up constructors and destructors like normal. Use alloc or autoalloc as necessary to create objects (same syntax as C++ new).
 - Prefer operator overloading (==, >, <, etc) over equivalents such as EqualTo()
    - I'm kind of torn on this one.
 - When to return a reference type vs. a Pointer<>? Iunno, if NULL is a valid return value I guess.
 - Delegates do not own objects when retaining a member pointer.
    - They do make copies of functors, however.
    - Events hold on to references to delegates.
       - This should probably change. Copies are good, use BaseDelegate for custom non-classic-C++ functors (or just use functors if a copy is okay).


Naming Conventions
==================

 - Macros are all UPPER_CASE #defines.
 - Generally, think of the .NET naming scheme.
 - Method names are always ProperCase(). So are functions. And class names. And namespaces.
 - Constants are static members of classes, again ProperCase.
 - Protected/private members are usually camelCase, but they should never be exposed without getters/setters anyway.
    - Public would be ProperCase, but again, getters plzkthx.
 - It's all indented with tab characters 4 spaces wide.


Non-Pointer Conventions
=======================

 - Objects on stack / member variables are fine. In fact, ObjectPools are often used on the stack RIAA style.
 - Be careful not to overrelease them.
 - However, use sparingly. Most classes shouldn't be used this way, consider AutoPointer<>s (or references, if you're up to manual release) instead.
    - AutoPointer<> helps because it makes your class copyable. Always use references/Pointers if using a user value, since anything may be subclassed.
 - Classes designed to be used as rvalues (for the sake of ease of use):
    - Bricks::Delegate (and friends. Sort of.)
    - Bricks::String
    - Anything that doesn't derive from Bricks::Object
      - Bricks::Pointer, AutoPointer, CopyPointer
      - Bricks::Threading::ThreadLocalStorage
 - The above listed types should be considered native types, and, unlike any other types, can be exposed publicly by value.
    - So, for example, a string may be returned and passed around by value instead of reference.
    - Subclassing these is not recommended, due to the use of them as value types.

Value Types
-----------
 - As mentioned above, some types are meant to be used as rvalues, and all over as value types.
 - These should be considered native types that 

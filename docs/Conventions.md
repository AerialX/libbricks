Random Usage Notes
==================

 - Always use references where possible, and Pointer<>s or AutoPointer<>s when reseating is required.
 - The use of new on objects isn't suggested, but it works fine. Either call delete manually or Release() them.
 - Set up constructors and destructors like normal. Use alloc or autoalloc as necessary to create objects (same syntax as C++ new).
 - Prefer operator overloading (==, >, <, etc) over equivalents such as EqualTo()
    - I'm kind of torn on this one.
 - When to return a reference type vs. a Pointer<>? Iunno.


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
 - However, use sparingly. Most classes shouldn't be used this way, consider AutoPointer<>s instead.
 - Classes designed to be used as rvalues (for the sake of ease of use):
    - Bricks::Delegate (and friends. Sort of.)
    - Bricks::String
    - Anything that doesn't derive from Bricks::Object
      - Bricks::Pointer, AutoPointer, CopyPointer
      - Bricks::Threading::ThreadLocalStorage

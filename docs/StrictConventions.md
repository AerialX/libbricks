This is like Conventions.md, but hard rules. Consider Conventions.md scribbled notes, these are teh rulez.

Calling Conventions For Public Interfaces
 - Types are preferably passed by reference (ideally const reference when applicable).
 - Types may be passed by Pointer<> instead, but only when a NULL value is valid in the context.
 - Types may not be passed by value, unless they are a designated value type (in which case, const references are still preferred).
 - No C pointers for non-value types (and prefer void* for raw data).
 - These same rules apply to return values.

Value Types
 - You may use any libbricks class as a value on the stack, or privately as class members in cases when polymorphism is unnecessary.
    - Note that when using them as class members, they will be copied along with the class rather than re-referenced if the parent is copied.
 - All classes must conform to standard C++ assignment operators and copy constructors.
    - However, this is mainly for the sake of compatibility. Standard libbricks behaviour does not use them for non-value types (though the shortcut copyable implementation does, this is optional).
    - To simplify code and use built-in copy constructors / assignment operators, use class members as either values or as AutoPointer<>/CopyPointer<>.
       - Refrain from manually Retain()ing and Release()ing in your constructor/destructors, unless you have manual copy constructors / assignment operators.
          - Or if you want to and are lazy, at least use BRICKS_COPY_HIDE(T) or inherit from Bricks::NoCopy to prevent the built-in ones from causing bugs.

Designated Value Types
 - The types are...
    - Bricks::String
    - Bricks::Delegate
    - Bricks::Event
    - Anything that does not derive from Bricks::Object
       - Bricks::Pointer, AutoPointer, CopyPointer
       - Bricks::Threading::ThreadLocalStorage
 - These types are meant to be considered value types. Though they may derive from Bricks::Object and use reference counting, they are not considered polymorphic. Do not subclass them.
    - Well, if you do, expect them to be copied by value to the original parent type when passed around.
 - Feel free to create your own and use them as if they were an official value type.

Operator Overloading
 - Use alternatives such as EqualTo() instead.
 - Overloaded operators will only be used for value types.

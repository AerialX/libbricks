This is like Conventions.md, but hard rules. Consider Conventions.md scribbled notes, these are teh rulez.

Calling Conventions For Public Interfaces
 - Value types are preferably passed by const reference, otherwise by value.
 - Non-value types are always passed by const Pointer<>&.
 - Use void* (preferably const void* if possible) for raw data.
 - Similar rules apply to return types, except in the case of non-value types:
    - Non-value types are preferably returned as ReturnPointer<>s. It is up to the implementation whether it is to be autoreleased upon return or not.
    - Non-value types may otherwise be returned as C pointers or Pointer<>s if and only if they are meant to be weak pointers. Keep in mind that ReturnPointer<> may be used as a weak pointer, and is thus preferred when defining class public interfaces.

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

libbricks
=========

libbricks is the bastard child of Objective-C and .NET, living with its C++
cousins. Srsly, wtf. Don't ask me why, I just happened to want some automagical
memory management in portable C++ land and it turned into this.


Building
--------

Uh, have GCC installed and run...

    make

You may specify an alternate GCC to build for Windows like so...

    make CXX=i486-mingw32-g++ AR=i486-mingw32-ar

Or alternatively, use Clang instead...

    make USECLANG=1


Configuration
-------------

There are some options in _include/bricks/config.h_ so feel free to modify
those as necessary.


Testing
-------

There's a simple test app in the _test/_ folder, it's easy to try:

    make run

And for when it inevitably crashes, the following shortcuts to gdb:

    make debug

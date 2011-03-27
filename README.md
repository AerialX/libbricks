libbricks
=========

libbricks is the bastard child of Objective-C and .NET, living with its C++
cousins. Srsly, wtf. Don't ask me why, I just happened to want some automagical
memory management in portable C++ land and it turned into this.


Building
--------

Uh, have a decent compiler (probably only compiles with GCC) and run...

    make


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

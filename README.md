libbricks
=========

libbricks is the bastard child of Objective-C and .NET, living with its C++
cousins. Srsly, wtf. Don't ask me why, I just happened to want some automagical
memory management in portable C++ land and it turned into this.


Building
--------

Uh, have GCC and cmake installed and run...

    make

You may specify an alternate GCC to build for Windows like so...

    make CXX=i486-mingw32-g++ AR=i486-mingw32-ar


Configuration
-------------

Use cmake to configure and select from the various compilation options.


Testing
-------

There's a simple test app in the _sample/_ folder, it's easy to try:

    make run

There also exist a few test units, which can be run with:

    make test

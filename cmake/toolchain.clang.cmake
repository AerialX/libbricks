set(CMAKE_C_COMPILER   "clang"    CACHE PATH "gcc" FORCE)
set(CMAKE_CXX_COMPILER "clang++"  CACHE PATH "g++" FORCE)

include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER("${CMAKE_C_COMPILER}" GNU)
CMAKE_FORCE_CXX_COMPILER("${CMAKE_CXX_COMPILER}" GNU)
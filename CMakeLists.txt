cmake_minimum_required(VERSION 2.6)
project(libbricks)

set(BRICKS_VERSION_MAJOR 0)
set(BRICKS_VERSION_MINOR 1)
set(BRICKS_VERSION "0.1")

option(BRICKS_CONFIG_RTTI "Enabled RTTI typeinfo for object.GetClass() and a more helpful GetDebugString()" ON)
option(BRICKS_CONFIG_CPP0X "Enables C++0x features, like lambdas and variadic templates" OFF)
option(BRICKS_CONFIG_LOGGING "Logs various information about libbricks to stderr" OFF)
option(BRICKS_CONFIG_LOGGING_HEAVY "A lot more logging. Requires BRICKS_CONFIG_LOGGING" OFF)
option(BRICKS_CONFIG_LOGGING_MEMLEAK "Report any unreleased objects at program termination. Requires BRICKS_CONFIG_LOGGING" OFF)
option(BRICKS_CONFIG_STL "Use the STL internally in collections" ON)
option(USE_CLANG "Build libbricks with Clang" OFF)

option(BRICKS_CONFIG_AUDIO_FFMPEG "Enable the FFMPEG audio decoder" OFF)

option(BRICKS_CONFIG_COMPRESSION_LIBZIP "Enable the libzip filesystem" OFF)

if (USE_CLANG)
	SET (CMAKE_C_COMPILER             "clang")
	SET (CMAKE_C_FLAGS                "-Wall -std=c99")
	SET (CMAKE_C_FLAGS_DEBUG          "-g")
	SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
	SET (CMAKE_C_FLAGS_RELEASE        "-O3 -DNDEBUG")
	#SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
	SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

	SET (CMAKE_CXX_COMPILER             "clang++")
	SET (CMAKE_CXX_FLAGS                "-Wall")
	SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
	SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
	SET (CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
	#SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
	SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

	#SET (CMAKE_AR                       "llvm-ar")
	#SET (CMAKE_LINKER                   "llvm-ld")
	#SET (CMAKE_NM                       "llvm-nm")
	#SET (CMAKE_OBJDUMP                  "llvm-objdump")
	#SET (CMAKE_RANLIB                   "llvm-ranlib")
endif(USE_CLANG)

configure_file("${PROJECT_SOURCE_DIR}/config.h.in" "${PROJECT_BINARY_DIR}/include/bricks/config.h" ESCAPE_QUOTES)

include_directories("include" "source" "${PROJECT_BINARY_DIR}/include")
add_definitions(-Wall)

add_library(bricks-core "source/exception.cpp" "source/object.cpp" "source/string.cpp")
add_library(bricks-io "source/io/console.cpp" "source/io/filesystem.cpp")
add_library(bricks-threading "source/threading/thread.cpp" "source/threading/threadlocalstorage.cpp")
add_library(bricks-audio "source/audio/midireader.cpp" "source/audio/ffmpegcodec.cpp")
add_library(bricks-compression "source/compression/zipfilesystem.cpp")

add_executable(sample "sample/source/main.cpp")
target_link_libraries(sample bricks-io bricks-core)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include" DESTINATION .)
install(FILES "${PROJECT_BINARY_DIR}/include/bricks/config.h" DESTINATION "include/bricks/")
install(TARGETS bricks-core bricks-io bricks-threading bricks-audio bricks-compression DESTINATION lib)

add_test(sample sample)
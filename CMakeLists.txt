cmake_minimum_required(VERSION 2.6)
project(libbricks)

macro(xcode_project target)
	if(APPLE)
		set_target_properties(${target} PROPERTIES MACOSX_BUNDLE true)
		set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "$ENV{IOS_SDK_CODESIGN_IDENTITY}")
		set(CMAKE_OSX_ARCHITECTURES $(ARCHS_STANDARD_32_BIT))
		set(MACOSX_BUNDLE_GUI_IDENTIFIER "$ENV{IOS_SDK_COMPANY_IDENTIFIER}.\${PRODUCT_NAME:identifier}")
		if(IOS)
			if (XCODE)
				set(CMAKE_OSX_SYSROOT iphoneos${IOS_SDK_VERSION})
			else()
				set(CMAKE_OSX_SYSROOT ${IOS_SDK_SYSROOT})
			endif()
		endif()
	endif()
endmacro()

set(BRICKS_VERSION_MAJOR 0)
set(BRICKS_VERSION_MINOR 1)
set(BRICKS_VERSION "0.1")

option(BRICKS_CONFIG_LOGGING "Logs various information about libbricks to stderr" OFF)
option(BRICKS_CONFIG_LOGGING_HEAVY "A lot more logging. Requires BRICKS_CONFIG_LOGGING" OFF)
option(BRICKS_CONFIG_LOGGING_ZOMBIES "Released objects become zombies and will terminate gracefully if used" OFF)

option(BRICKS_CONFIG_RTTI "Enabled RTTI typeinfo for object.GetClass() and a more helpful GetDebugString()" ON)
option(BRICKS_CONFIG_CPP0X "Enables C++0x features, like lambdas and variadic templates" OFF)
option(BRICKS_CONFIG_STL "Use the STL internally in collections" ON)

set(BRICKSLIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/lib/build")
if(IOS)
	set(BRICKS_LINK_DIRECTORIES "${BRICKSLIB_ROOT}/ios-universal/lib")
	if(XCODE)
		set(BRICKS_INCLUDE_DIRS "${BRICKSLIB_ROOT}/ios-universal/\$(PLATFORM_DIR)/include")
		set(BRICKSLIB_DIR "${BRICKSLIB_ROOT}/ios-armv6" "${BRICKSLIB_ROOT}/ios-armv7" "${BRICKSLIB_ROOT}/ios-i386")
	elseif(IOS_SIMULATOR)
		set(BRICKSLIB_DIR "${BRICKSLIB_ROOT}/ios-i386")
	elseif(HOST_APPLE)
		set(BRICKSLIB_DIR "${BRICKSLIB_ROOT}/ios-armv6" "${BRICKSLIB_ROOT}/ios-armv7")
	else()
		set(BRICKSLIB_DIR "${BRICKSLIB_ROOT}/ios-armv6")
	endif()
	set(BRICKSLIB_DIR "${BRICKSLIB_ROOT}/ios-universal" ${BRICKSLIB_DIR})
elseif(ANDROID)
	if(NDK_CPU_X86)
		set(BRICKSLIB_DIR "${BRICKSLIB_ROOT}/android-x86")
	elseif(NDK_CPU_ARM_V7A)
		set(BRICKSLIB_DIR "${BRICKSLIB_ROOT}/android-armv7")
	else()
		set(BRICKSLIB_DIR "${BRICKSLIB_ROOT}/android-armv5")
	endif()
else()
	execute_process(COMMAND echo "echo -n $OSTYPE"
		COMMAND bash
		OUTPUT_VARIABLE OSTYPE)
	set(BRICKSLIB_DIR "${BRICKSLIB_ROOT}/${OSTYPE}")
endif()
foreach(lib ${BRICKSLIB_DIR})
	link_directories(${lib}/lib)
endforeach()
set(CMAKE_FIND_ROOT_PATH ${BRICKSLIB_DIR} "${CMAKE_FIND_ROOT_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(IOS)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".dylib;.a")
endif()
find_package(PNG)
find_package(Freetype)
find_package(LibZip)
find_package(FFMPEG)
find_package(GTest)

if(PNG_FOUND)
	set(BRICKS_CONFIG_IMAGING_LIBPNG true)
	add_definitions(${PNG_DEFINITIONS})
	include_directories(${PNG_INCLUDE_DIR})
endif()
if(FREETYPE_FOUND)
	set(BRICKS_CONFIG_IMAGING_FREETYPE true)
	include_directories(${FREETYPE_INCLUDE_DIRS})

	# freetype2 may also support bzip2 compressed fonts
	find_package(BZip2)
	if(BZIP2_FOUND)
		set(FREETYPE_LIBRARIES ${FREETYPE_LIBRARIES} ${BZIP2_LIBRARIES})
	endif()
endif()
if(LIBZIP_FOUND)
	set(BRICKS_CONFIG_COMPRESSION_LIBZIP true)
	include_directories(${LIBZIP_INCLUDE_DIRS})
endif()
if(FFMPEG_FOUND)
	set(BRICKS_CONFIG_AUDIO_FFMPEG true)
	include_directories(${FFMPEG_INCLUDE_DIR})
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/bricks/config.h" ESCAPE_QUOTES)

set(BRICKS_DEFINITIONS ${BRICKS_DEFINITIONS} "-Wall -Wno-empty-body")

if (BRICKS_CONFIG_RTTI)
	set(BRICKS_DEFINITIONS ${BRICKS_DEFINITIONS} "-frtti")
endif()

if (ANDROID)
	set(BRICKS_CORE_LINK_LIBRARIES gnustl_static)
endif()

set(BRICKS_CORE_SOURCE_FILES
	"source/core/object.cpp"
	"source/core/exception.cpp"
	"source/core/string.cpp" "source/core/value.cpp"
	"source/core/time.cpp" "source/core/timespan.cpp"
)

set(BRICKS_IO_LINK_LIBRARIES bricks-core)
set(BRICKS_IO_SOURCE_FILES
	"source/io/console.cpp" "source/io/filesystem.cpp"
	"source/io/substream.cpp" "source/io/cachestream.cpp"
)

if (NOT APPLE AND NOT ANDROID)
	set(BRICKS_THREADING_LINK_LIBRARIES ${BRICKS_THREADING_LINK_LIBRARIES} rt)
endif()
if (NOT ANDROID)
	set(BRICKS_THREADING_LINK_LIBRARIES ${BRICKS_THREADING_LINK_LIBRARIES} pthread)
endif()
set(BRICKS_THREADING_SOURCE_FILES
	"source/threading/thread.cpp" "source/threading/threadlocalstorage.cpp"
	"source/threading/mutex.cpp" "source/threading/condition.cpp" "source/threading/conditionlock.cpp" "source/threading/semaphore.cpp"
	"source/threading/mutexlock.cpp"
)

set(BRICKS_AUDIO_LINK_LIBRARIES bricks-io)
set(BRICKS_AUDIO_SOURCE_FILES
	"source/audio/midireader.cpp"
)

set(BRICKS_IMAGING_LINK_LIBRARIES bricks-io)
set(BRICKS_IMAGING_SOURCE_FILES
	"source/imaging/colour.cpp" "source/imaging/image.cpp" "source/imaging/bitmap.cpp"
	"source/imaging/font.cpp"
	"source/imaging/videocodec.cpp"
)

set(BRICKS_CRYPTOGRAPHY_LINK_LIBRARIES bricks-io)
set(BRICKS_CRYPTOGRAPHY_SOURCE_FILES
	"source/cryptography/hash.cpp"
)

set(BRICKS_COMPRESSION_LINK_LIBRARIES bricks-io)
set(BRICKS_COMPRESSION_SOURCE_FILES
)

macro(add_any_library libvar libdirvar libs)
	foreach(lib ${libs})
		string(REGEX MATCH ".+/" libdir ${lib})
		string(REGEX MATCH "[^/]+$" libname ${lib})
		string(FIND ${lib} ".framework" is_framework)
		if(is_framework GREATER "-1")
			string(REPLACE ".framework" "" libname ${libname})
			set(${libvar} ${${libvar}} "-framework ${libname}")
		else()
			string(FIND ${lib} "/Developer/Platforms/" is_xcode_path)
			if(is_xcode_path LESS "0")
				set(${libdirvar} ${${libdirvar}} ${libdir})
			endif()
			string(REGEX REPLACE "\\..+$" "" libname ${libname})
			string(REGEX REPLACE "^lib" "" libname ${libname})
			set(${libvar} ${${libvar}} ${libname})
		endif()
	endforeach()
endmacro()

if(BRICKS_CONFIG_IMAGING_LIBPNG)
	set(BRICKS_IMAGING_SOURCE_FILES ${BRICKS_IMAGING_SOURCE_FILES} "source/imaging/png.cpp")
	add_any_library(BRICKS_IMAGING_LINK_LIBRARIES BRICKS_LINK_DIRECTORIES "${PNG_LIBRARIES}")
endif()

if(BRICKS_CONFIG_IMAGING_FREETYPE)
	set(BRICKS_IMAGING_SOURCE_FILES ${BRICKS_IMAGING_SOURCE_FILES} "source/imaging/freetypefont.cpp")
	add_any_library(BRICKS_IMAGING_LINK_LIBRARIES BRICKS_LINK_DIRECTORIES "${FREETYPE_LIBRARIES}")
endif()

if(BRICKS_CONFIG_COMPRESSION_LIBZIP)
	set(BRICKS_COMPRESSION_SOURCE_FILES ${BRICKS_COMPRESSION_SOURCE_FILES} "source/compression/zipfilesystem.cpp")
	add_any_library(BRICKS_COMPRESSION_LINK_LIBRARIES BRICKS_LINK_DIRECTORIES "${LIBZIP_LIBRARIES}")
endif()

if(BRICKS_CONFIG_AUDIO_FFMPEG)
	set(BRICKS_AUDIO_SOURCE_FILES ${BRICKS_AUDIO_SOURCE_FILES}
		"source/audio/ffmpegdecoder.cpp" "source/audio/ffmpegaudiodecoder.cpp"
	)
	set(BRICKS_IMAGING_SOURCE_FILES ${BRICKS_IMAGING_SOURCE_FILES}
		"source/imaging/ffmpegvideodecoder.cpp"
	)
	add_any_library(BRICKS_AUDIO_LINK_LIBRARIES BRICKS_LINK_DIRECTORIES "${FFMPEG_LIBRARIES}")
	add_any_library(BRICKS_IMAGING_LINK_LIBRARIES BRICKS_LINK_DIRECTORIES "${FFMPEG_LIBRARIES}")
	set(BRICKS_IMAGING_LINK_LIBRARIES ${BRICKS_IMAGING_LINK_LIBRARIES} bricks-audio)
endif()

if(NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH} PARENT_SCOPE)
	set(BRICKS_DEFINITIONS ${BRICKS_DEFINITIONS} PARENT_SCOPE)
	set(BRICKS_LINK_DIRS ${BRICKS_LINK_DIRECTORIES} PARENT_SCOPE)
	set(BRICKS_INCLUDE_DIRS ${BRICKS_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include" PARENT_SCOPE)
endif()

add_definitions(${BRICKS_DEFINITIONS})
link_directories(${BRICKS_LINK_DIRECTORIES})
include_directories("include" "source" "${CMAKE_CURRENT_BINARY_DIR}/include" ${BRICKS_INCLUDE_DIRS})

add_library(bricks-core ${BRICKS_CORE_SOURCE_FILES})
target_link_libraries(bricks-core ${BRICKS_CORE_LINK_LIBRARIES})
set(BRICKS_TARGETS ${BRICKS_TARGETS} bricks-core)

add_library(bricks-io ${BRICKS_IO_SOURCE_FILES})
target_link_libraries(bricks-io ${BRICKS_IO_LINK_LIBRARIES})
set(BRICKS_TARGETS ${BRICKS_TARGETS} bricks-io)

add_library(bricks-threading ${BRICKS_THREADING_SOURCE_FILES})
target_link_libraries(bricks-threading ${BRICKS_THREADING_LINK_LIBRARIES})
set(BRICKS_TARGETS ${BRICKS_TARGETS} bricks-threading)

add_library(bricks-audio ${BRICKS_AUDIO_SOURCE_FILES})
target_link_libraries(bricks-audio ${BRICKS_AUDIO_LINK_LIBRARIES})
set(BRICKS_TARGETS ${BRICKS_TARGETS} bricks-audio)

if(BRICKS_COMPRESSION_SOURCE_FILES)
	add_library(bricks-compression ${BRICKS_COMPRESSION_SOURCE_FILES})
	target_link_libraries(bricks-compression ${BRICKS_COMPRESSION_LINK_LIBRARIES})
	set(BRICKS_TARGETS ${BRICKS_TARGETS} bricks-compression)
endif()

add_library(bricks-imaging ${BRICKS_IMAGING_SOURCE_FILES})
target_link_libraries(bricks-imaging ${BRICKS_IMAGING_LINK_LIBRARIES})
set(BRICKS_TARGETS ${BRICKS_TARGETS} bricks-imaging)

add_library(bricks-cryptography ${BRICKS_CRYPTOGRAPHY_SOURCE_FILES})
target_link_libraries(bricks-cryptography ${BRICKS_CRYPTOGRAPHY_LINK_LIBRARIES})
set(BRICKS_TARGETS ${BRICKS_TARGETS} bricks-cryptography)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	add_executable(sample "sample/source/main.cpp")
	target_link_libraries(sample bricks-io bricks-core)
	xcode_project(sample)

	if(GTEST_FOUND)
		add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
	endif()
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include" DESTINATION .)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/include/bricks/config.h" DESTINATION "include/bricks/")
install(TARGETS ${BRICKS_TARGETS} DESTINATION lib)

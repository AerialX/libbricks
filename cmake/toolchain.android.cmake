set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

if (APPLE)
	set (HOST_APPLE true)
elseif (UNIX)
	set (HOST_UNIX true)
endif()

set (APPLE false)
set (UNIX true)

set(ANDROID_NDK_DEFAULT_SEARCH_PATH /opt/android-ndk)
set(ANDROID_NDK_SUPPORTED_VERSIONS -r6 -r5c -r5b -r5 "")
set(ANDROID_NDK_TOOLCHAIN_DEFAULT_SEARCH_PATH /opt/android-toolchain)
set(TOOL_OS_SUFFIX "")

macro( __TOOLCHAIN_DETECT_API_LEVEL _path )
	SET( _expected ${ARGV1} )
	if( NOT EXISTS ${_path} )
		message( FATAL_ERROR "Could not verify Android API level. Probably you have specified invalid level value, or your copy of NDK/toolchain is broken." )
	endif()
	SET( API_LEVEL_REGEX "^[\t ]*#define[\t ]+__ANDROID_API__[\t ]+([0-9]+)[\t ]*$" )
	FILE( STRINGS ${_path} API_FILE_CONTENT REGEX "${API_LEVEL_REGEX}")
	if( NOT API_FILE_CONTENT )
		message( FATAL_ERROR "Could not verify Android API level. Probably you have specified invalid level value, or your copy of NDK/toolchain is broken." )
	endif()
	string( REGEX REPLACE "${API_LEVEL_REGEX}" "\\1" ANDROID_LEVEL_FOUND "${API_FILE_CONTENT}" )
	if( DEFINED _expected )
		if( NOT ${ANDROID_LEVEL_FOUND} EQUAL ${_expected} )
			message( FATAL_ERROR "Specified Android API level does not match level found. Probably your copy of NDK/toolchain is broken." )
		endif()
	endif()
	set( ANDROID_API_LEVEL ${ANDROID_LEVEL_FOUND} CACHE STRING "android API level" FORCE )
endmacro()

if (NOT DEFINED ANDROID_NDK)
	set(ANDROID_NDK "$ENV{ANDROID_NDK}")
endif()

if (NOT DEFINED ANDROID_NDK_TOOLCHAIN_ROOT)
	set(ANDROID_NDK_TOOLCHAIN_ROOT $ENV{ANDROID_NDK_TOOLCHAIN_ROOT})
endif()

if (NOT EXISTS "${ANDROID_NDK}")
	foreach (ndk_version ${ANDROID_NDK_SUPPORTED_VERSIONS})
		if (EXISTS ${ANDROID_NDK_DEFAULT_SEARCH_PATH}${ndk_version})
			set(ANDROID_NDK "${ANDROID_NDK_DEFAULT_SEARCH_PATH}${ndk_version}")
			message(STATUS "Using default path for android NDK ${ANDROID_NDK}")
			message(STATUS "  If you prefer to use a different location, please define the variable: ANDROID_NDK")
			break()
		endif()
	endforeach()
endif()

if (EXISTS "${ANDROID_NDK}")
	if(APPLE)
		set(NDKSYSTEM "darwin-x86")
	elseif(WIN32)
		set(NDKSYSTEM "windows")
		set(TOOL_OS_SUFFIX ".exe")
	elseif(UNIX)
		set(NDKSYSTEM "linux-x86")
	else()
		message(FATAL_ERROR "Your platform is not supported")
	endif()

	if (NOT ANDROID_API_LEVEL GREATER 2)
		if (NDK_CPU_X86)
			set(ANDROID_API_LEVEL 9)
		else()
			set(ANDROID_API_LEVEL 8)
		endif()
		message(STATUS "Using default android API level android-${ANDROID_API_LEVEL}")
		message(STATUS "  If you prefer to use a different API level, please define the variable: ANDROID_API_LEVEL")
	endif()

	if (NDK_CPU_X86)
		set(ANDROID_NDK_TOOLCHAIN_ROOT "${ANDROID_NDK}/toolchains/x86-4.4.3/prebuilt/${NDKSYSTEM}")
		set(ANDROID_NDK_SYSROOT "${ANDROID_NDK}/platforms/android-${ANDROID_API_LEVEL}/arch-x86")
	else()
		set(ANDROID_NDK_TOOLCHAIN_ROOT "${ANDROID_NDK}/toolchains/arm-linux-androideabi-4.4.3/prebuilt/${NDKSYSTEM}")
		set(ANDROID_NDK_SYSROOT "${ANDROID_NDK}/platforms/android-${ANDROID_API_LEVEL}/arch-arm")
	endif()

	__TOOLCHAIN_DETECT_API_LEVEL("${ANDROID_NDK_SYSROOT}/usr/include/android/api-level.h" ${ANDROID_API_LEVEL})
endif()

if (NDK_CPU_X86)
	set(ARM_PREFIX "i686-android-linux")
else()
	set(ARM_PREFIX "arm-linux-androideabi")
endif()

set(CMAKE_C_COMPILER   "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ARM_PREFIX}-gcc${TOOL_OS_SUFFIX}"     CACHE PATH "gcc" FORCE)
set(CMAKE_CXX_COMPILER "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ARM_PREFIX}-g++${TOOL_OS_SUFFIX}"     CACHE PATH "g++" FORCE)
set(CMAKE_AR           "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ARM_PREFIX}-ar${TOOL_OS_SUFFIX}"      CACHE PATH "archive" FORCE)
set(CMAKE_LINKER       "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ARM_PREFIX}-ld${TOOL_OS_SUFFIX}"      CACHE PATH "linker" FORCE)
set(CMAKE_NM           "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ARM_PREFIX}-nm${TOOL_OS_SUFFIX}"      CACHE PATH "nm" FORCE)
set(CMAKE_OBJCOPY      "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ARM_PREFIX}-objcopy${TOOL_OS_SUFFIX}" CACHE PATH "objcopy" FORCE)
set(CMAKE_OBJDUMP      "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ARM_PREFIX}-objdump${TOOL_OS_SUFFIX}" CACHE PATH "objdump" FORCE)
set(CMAKE_STRIP        "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ARM_PREFIX}-strip${TOOL_OS_SUFFIX}"   CACHE PATH "strip" FORCE)
set(CMAKE_RANLIB       "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ARM_PREFIX}-ranlib${TOOL_OS_SUFFIX}"  CACHE PATH "ranlib" FORCE)

if (NDK_CPU_ARM)
	set(ANDROID_NDK_CPU_NAME "armeabi")
	set(NDK_CPU_ARM_NEON false)
	set(CMAKE_SYSTEM_PROCESSOR "armv5te")
elseif (NDK_CPU_X86)
	set(ANDROID_NDK_CPU_NAME "x86")
	set(CMAKE_SYSTEM_PROCESSOR "x86")
elseif (NDK_CPU_ARM_V7A)
	set(NDK_CPU_ARM_NEON false)
	set(NDK_CPU_ARM_VFPV3 false)
	set(NDK_CPU_ARM true)
	set(ANDROID_NDK_CPU_NAME "armeabi-v7a")
	set(CMAKE_SYSTEM_PROCESSOR "armv7-a")
endif()

set(CMAKE_FIND_ROOT_PATH "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin" "${ANDROID_NDK_TOOLCHAIN_ROOT}/${ARM_PREFIX}" "${ANDROID_NDK_SYSROOT}" "${CMAKE_INSTALL_PREFIX}" "${CMAKE_INSTALL_PREFIX}/share")

set(STL_PATH "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++")
set(STL_LIBRARIES_PATH "${STL_PATH}/libs/${ANDROID_NDK_CPU_NAME}")
include_directories(SYSTEM "${STL_PATH}/include" "${STL_LIBRARIES_PATH}/include")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_C_FLAGS "-fPIC -DANDROID -Wno-psabi -fsigned-char")

set(FORCE_ARM OFF CACHE BOOL "Use 32-bit ARM instructions instead of Thumb-1")
if (NDK_CPU_X86)
elseif (NOT FORCE_ARM)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthumb")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -marm")
endif()

set(CMAKE_C_FLAGS "--sysroot=${ANDROID_NDK_SYSROOT} ${CMAKE_C_FLAGS}")

include (CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER("${CMAKE_C_COMPILER}" GNU)
CMAKE_FORCE_CXX_COMPILER("${CMAKE_CXX_COMPILER}" GNU)

if (NDK_CPU_ARM_V7A)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfloat-abi=softfp")
	if (NDK_CPU_ARM_NEON)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
	elseif (NDK_CPU_ARM_VFPV3)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=vfpv3")
	endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "c++ flags")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "c flags")
      
set(LINKER_FLAGS "-L\"${STL_LIBRARIES_PATH}\" -L\"${CMAKE_INSTALL_PREFIX}/libs/${ANDROID_NDK_CPU_NAME}\" -lstdc++ -lsupc++")
if (NOT NDK_CPU_X86)
	set(LINKER_FLAGS "-Wl,--fix-cortex-a8 ${LINKER_FLAGS}")
endif()

set(CMAKE_SHARED_LINKER_FLAGS "${LINKER_FLAGS}" CACHE STRING "linker flags" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS "${LINKER_FLAGS}" CACHE STRING "linker flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "${LINKER_FLAGS}" CACHE STRING "linker flags" FORCE)

set(ANDROID true)
set(BUILD_ANDROID true)

MARK_AS_ADVANCED(FORCE_ARM NO_UNDEFINED)

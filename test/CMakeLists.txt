enable_testing()
include_directories(${GTEST_INCLUDE_DIRS})

if (EMSCRIPTEN)
	set(BRICKS_TEST_PATH "/")
else()
	set(BRICKS_TEST_PATH ${CMAKE_CURRENT_SOURCE_DIR})
endif()

add_definitions(-DTEST_PATH=\"${BRICKS_TEST_PATH}\") 

macro(test_project target source)
	add_executable(${target} ${source})
	target_link_libraries(${target} ${ARGN} bricks-core bricks-io ${GTEST_BOTH_LIBRARIES} pthread)

	if (EMSCRIPTEN)
		set_target_properties(${target} PROPERTIES LINK_FLAGS
			"--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/data@data"
			)
		add_test(NAME ${target} COMMAND node ${target}.js)
	else()
		add_test(${target} ${target})
	endif()
endmacro()

test_project(bricks-test-core-value core-value.cpp)

test_project(bricks-test-collections-listguard collections-listguard.cpp)

test_project(bricks-test-audio-midi audio-midi.cpp bricks-audio)

test_project(bricks-test-io-stream io-stream.cpp)

test_project(bricks-test-io-navigator io-navigator.cpp)

if (BRICKS_CONFIG_COMPRESSION_LIBZIP)
	test_project(bricks-test-compression-zipfilesystem compression-zipfilesystem.cpp bricks-compression)
endif()

if (BRICKS_CONFIG_IMAGING_LIBPNG)
	test_project(bricks-test-imaging-png imaging-png.cpp bricks-imaging)
endif()

if (BRICKS_CONFIG_AUDIO_FFMPEG)
	test_project(bricks-test-audio-ffmpeg audio-ffmpeg.cpp bricks-audio)

	test_project(bricks-test-imaging-ffmpeg imaging-ffmpeg.cpp bricks-imaging)
endif()

test_project(bricks-test-imaging-image imaging-image.cpp bricks-imaging)

test_project(bricks-test-imaging-font imaging-font.cpp bricks-imaging)

test_project(bricks-test-threading-thread threading-thread.cpp bricks-threading)

test_project(bricks-test-cryptography-hash cryptography-hash.cpp bricks-cryptography)

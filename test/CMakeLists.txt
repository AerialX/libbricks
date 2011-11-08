enable_testing()
include_directories(${GTEST_INCLUDE_DIRS})
add_definitions(-DTEST_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}\") 

add_executable(bricks-test-core-value core-value.cpp)
target_link_libraries(bricks-test-core-value bricks-core bricks-io ${GTEST_BOTH_LIBRARIES} pthread)
add_test(bricks-test-core-value bricks-test-core-value)

add_executable(bricks-test-audio-midi audio-midi.cpp)
target_link_libraries(bricks-test-audio-midi bricks-audio bricks-io ${GTEST_BOTH_LIBRARIES} pthread)
add_test(bricks-test-audio-midi bricks-test-audio-midi)

add_executable(bricks-test-io-stream io-stream.cpp)
target_link_libraries(bricks-test-io-stream bricks-io ${GTEST_BOTH_LIBRARIES} pthread)
add_test(bricks-test-io-stream bricks-test-io-stream)

add_executable(bricks-test-io-navigator io-navigator.cpp)
target_link_libraries(bricks-test-io-navigator bricks-io ${GTEST_BOTH_LIBRARIES} pthread)
add_test(bricks-test-io-navigator bricks-test-io-navigator)

if (BRICKS_CONFIG_COMPRESSION_LIBZIP)
	add_executable(bricks-test-compression-zipfilesystem compression-zipfilesystem.cpp)
	target_link_libraries(bricks-test-compression-zipfilesystem bricks-compression ${GTEST_BOTH_LIBRARIES} pthread)
	add_test(bricks-test-compression-zipfilesystem bricks-test-compression-zipfilesystem)
endif()

if (BRICKS_CONFIG_IMAGING_LIBPNG)
	add_executable(bricks-test-imaging-png imaging-png.cpp)
	target_link_libraries(bricks-test-imaging-png bricks-imaging ${GTEST_BOTH_LIBRARIES} pthread)
	add_test(bricks-test-imaging-png bricks-test-imaging-png)
endif()

if (BRICKS_CONFIG_AUDIO_FFMPEG)
	add_executable(bricks-test-audio-ffmpeg audio-ffmpeg.cpp)
	target_link_libraries(bricks-test-audio-ffmpeg bricks-audio ${GTEST_BOTH_LIBRARIES} pthread)
	add_test(bricks-test-audio-ffmpeg bricks-test-audio-ffmpeg)
endif()

add_executable(bricks-test-imaging-image imaging-image.cpp)
target_link_libraries(bricks-test-imaging-image bricks-imaging ${GTEST_BOTH_LIBRARIES} pthread)
add_test(bricks-test-imaging-image bricks-test-imaging-image)

add_executable(bricks-test-imaging-font imaging-font.cpp)
target_link_libraries(bricks-test-imaging-font bricks-imaging ${GTEST_BOTH_LIBRARIES} pthread)
add_test(bricks-test-imaging-font bricks-test-imaging-font)

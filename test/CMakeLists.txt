enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_definitions(-DTEST_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}\") 

add_executable(bricks-test-audio-midi audio-midi.cpp)
target_link_libraries(bricks-test-audio-midi bricks-audio bricks-io bricks-core ${GTEST_BOTH_LIBRARIES} pthread)
add_test(bricks-test-audio bricks-test-audio-midi)

add_executable(bricks-test-io-navigator io-navigator.cpp)
target_link_libraries(bricks-test-io-navigator bricks-io bricks-core ${GTEST_BOTH_LIBRARIES} pthread)
add_test(bricks-test-io bricks-test-io-navigator)

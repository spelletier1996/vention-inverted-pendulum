
find_package(Boost REQUIRED)

set(test_list "shm" "tcp")

foreach(test IN LISTS test_list)
    add_executable(${test} ${test}.cpp)
    target_link_libraries(${test} Boost::boost)
    target_include_directories(${test} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    add_test(${test} ${test})
endforeach()
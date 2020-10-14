set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
enable_testing()
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_executable(s11n_test
        built_in_type_test.cpp
        util_test.cpp
        custom_record_test.cpp
        rtti_type_test.cpp
        rtti_table.cpp
        field_test.cpp)
target_link_libraries(s11n_test ${GTEST_BOTH_LIBRARIES} Threads::Threads)
target_include_directories(s11n_test PUBLIC ${GTEST_INCLUDE_DIRS})
target_include_directories(s11n_test PUBLIC ../include)

add_test(AllTests s11n_test)


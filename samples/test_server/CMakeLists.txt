cmake_minimum_required(VERSION 3.15)

add_subdirectory(schema)

find_package(Boost 1.82 REQUIRED COMPONENTS system filesystem json)
include_directories(${Boost_INCLUDE_DIRS})

add_library(test_server_graphql STATIC TestServerWrapper.cpp)
target_link_libraries(test_server_graphql PUBLIC test_server_schema)
target_include_directories(test_server_graphql PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/schema)
target_compile_options(test_server_graphql PRIVATE  -g -O0)

add_executable(test_server_query test_server_query.cpp)
target_link_libraries(test_server_query PRIVATE
    test_server_graphql
    graphqljson)
target_compile_options(test_server_query PRIVATE  -g -O0)

add_executable(test_server_mutation test_server_mutation.cpp)
target_link_libraries(test_server_mutation PRIVATE
    test_server_graphql
    graphqljson)
target_compile_options(test_server_mutation PRIVATE  -g -O0)

add_executable(test_server_subscription test_server_subscription.cpp)
target_link_libraries(test_server_subscription PRIVATE
    test_server_graphql
    graphqljson)
target_compile_options(test_server_subscription PRIVATE  -g -O0)

add_executable(test_server_web_subscription test_server_web_subscription.cpp)
target_link_libraries(test_server_web_subscription PRIVATE
    test_server_graphql
    graphqljson)
target_compile_options(test_server_web_subscription PRIVATE  -g -O0)

add_executable(test_server_web_old_subscription test_server_web_old_subscription.cpp)
target_link_libraries(test_server_web_old_subscription PRIVATE
    test_server_graphql
    graphqljson Boost::system Boost::filesystem Boost::json)
target_compile_options(test_server_web_old_subscription PRIVATE  -g -O0)

add_executable(test_server_web_async_old_subscription test_server_web_async_old_subscription.cpp)
target_link_libraries(test_server_web_async_old_subscription PRIVATE
    test_server_graphql
    graphqljson Boost::system Boost::filesystem Boost::json)
target_compile_options(test_server_web_async_old_subscription PRIVATE  -g -O0)

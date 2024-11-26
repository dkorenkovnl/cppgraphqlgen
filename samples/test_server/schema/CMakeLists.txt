cmake_minimum_required(VERSION 3.15)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/cppgraphqlgen-functions.cmake)
update_graphql_schema_files(test_server ./schema.test_server.graphql TestServer test_server --stubs)
add_graphql_schema_target(test_server)

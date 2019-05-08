cmake_minimum_required(VERSION 2.8)

project(person-server)

if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else()
    add_definitions(-D_WIN32_WINNT=0x600)
endif()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

set(person_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/person.pb.h")
set(person_srcs "${CMAKE_CURRENT_SOURCE_DIR}/person.pb.cc")
set(person_grpc_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/person.grpc.pb.h")
set(person_grpc_srcs "${CMAKE_CURRENT_SOURCE_DIR}/person.grpc.pb.cc")

# Proto file
get_filename_component(person_proto "${CMAKE_CURRENT_SOURCE_DIR}/person.proto" ABSOLUTE)
get_filename_component(person_proto_path "${person_proto}" PATH)

message("person_proto: ${person_proto}")
message("person_proto_path: ${person_proto_path}")

#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/protobuf/include")
#link_directories("${CMAKE_CURRENT_SOURCE_DIR}/protobuf/lib")

EXEC_PROGRAM(which 
    ARGS grpc_cpp_plugin
    OUTPUT_VARIABLE cpp_plugin)

message("cpp_plugin: ${cpp_plugin}")

add_custom_command(
        OUTPUT ${person_hdrs} ${person_srcs} ${person_grpc_hdrs} ${person_grpc_srcs}
        COMMAND protoc 
        --grpc_out "${person_proto_path}"
        --cpp_out "${person_proto_path}" 
        -I "${person_proto_path}"
        --plugin=protoc-gen-grpc="${cpp_plugin}"
        "${person_proto}"
        COMMENT "protoc compile ${person_proto}"
        DEPENDS "${person_proto}")

set(targets person_client person_server)

foreach(_target
        ${targets})

    
    add_executable(${_target} "${_target}.cpp" ${person_hdrs} ${person_srcs} ${person_grpc_srcs})

    target_link_libraries(${_target}
        grpc++_unsecure
        protobuf
        gpr
        protobuf
        pthread
        folly
        glog
        dl
        double-conversion
        iberty
        )
    
endforeach()

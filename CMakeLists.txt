# Minimum CMake required
cmake_minimum_required(VERSION 3.15)

# Project
project(warehouse)

# Protobuf
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

# Protobuf-compiler
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# gRPC
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# Proto file
get_filename_component(whd_proto "proto/warehouse.proto" ABSOLUTE)
get_filename_component(whd_proto_path "${whd_proto}" PATH)

# Generated sources
set(whd_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/warehouse.pb.cc")
set(whd_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/warehouse.pb.h")
set(whd_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/warehouse.grpc.pb.cc")
set(whd_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/warehouse.grpc.pb.h")
add_custom_command(
      OUTPUT "${whd_proto_srcs}" "${whd_proto_hdrs}" "${whd_grpc_srcs}" "${whd_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${whd_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${whd_proto}"
      DEPENDS "${whd_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Targets (client|server)
foreach(_target
  client server)
  add_executable(${_target} "src/${_target}.cpp"
    ${whd_proto_srcs}
    ${whd_grpc_srcs})
  target_link_libraries(${_target}
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()

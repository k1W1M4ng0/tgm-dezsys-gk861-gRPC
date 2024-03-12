.PHONY: proto

proto: proto/helloworld.proto
	protoc -I proto --grpc_out=src/proto --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` proto/helloworld.proto
	protoc -I proto --cpp_out=src/proto proto/helloworld.proto


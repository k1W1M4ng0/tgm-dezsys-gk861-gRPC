CXXFLAGS = -std=c++20

.PHONY: proto

proto: proto/warehouse.proto
	mkdir -p src/proto
	protoc -I proto --grpc_out=src/proto --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` proto/warehouse.proto
	protoc -I proto --cpp_out=src/proto proto/warehouse.proto

protoPython: proto/warehouse.proto
	mkdir -p python/src
	python -m grpc_tools.protoc -I proto --python_out=python/src --pyi_out=python/src --grpc_python_out=python/src proto/warehouse.proto

python:
	python python/src/client.py


SOURCES = $(shell (find src -name '*.cc';find src -name '*.cpp'))
# SOURCES = (find src -name '*.cc';find src -name '*.cpp')
# OBJECTS = $(shell echo $(SOURCES) | sed 's/$/.o/')
OBJECTS = $(shell echo $(SOURCES) | sed 's/.cc/.cc.o /g' | sed 's/.cpp/.cpp.o/')
# OBJECTS = `echo $(SOURCES) | xargs | sed 's/ /.o /g`

a: $(shell find -name test.cpp)
	g++ test.cpp -o a

$(OBJECTS): $(SOURCES)
	cd bin
	g++ -c $(SOURCES)

.PHONY: s
# s: $(OBJECTS)
# 	g++ $(shell find -name '*.o') `pkgconf --libs grpc++` `pkgconf --libs protobuf` -pthread -o server
# 	./server 4657

LDFLAGS += `pkgconf --libs grpc++` 
LDFLAGS += `pkgconf --libs protobuf` 
	
s: 
	g++ `find src/proto -name '*.cc'` src/server.cpp $(LDFLAGS) -o server
	./server 4657

c:
	g++ `find src/proto -name '*.cc'` src/client.cpp $(LDFLAGS) -o client
	./client

	
run: 
	mkdir -p cmake/build
	cd cmake/build && cmake ../.. && make

s2: 
	cmake/build/server 4657

c2: 
	cmake/build/client

test: 
	echo $(SOURCES)
	echo $(OBJECTS)
	@# ./server


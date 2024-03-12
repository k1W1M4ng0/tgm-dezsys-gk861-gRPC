CXXFLAGS = -std=c++20

.PHONY: proto

proto: proto/helloworld.proto
	protoc -I proto --grpc_out=src/proto --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` proto/helloworld.proto
	protoc -I proto --cpp_out=src/proto proto/helloworld.proto

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
s: $(OBJECTS)
	g++ $(shell find -name '*.o') `pkgconf --libs grpc++` `pkgconf --libs protobuf` -pthread -o server
	./server 4657
	

	

test: 
	echo $(SOURCES)
	echo $(OBJECTS)
	@# ./server


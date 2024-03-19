# SYT Dezentrale Systeme GK861 gRPC

## Questions

### What is gRPC and why does it work accross languages and platforms?

gRPC is a RPC framework that supports many languages.
It uses Protocol Buffers (.proto files) to specify the RPC format, and uses the
protoc compiler to support different languages and platforms.

### Describe the RPC life cycle starting with the RPC client?

The client has a stub that implements the same functions as the server, and the client
can just call this function. Their implementations wrap the parameters into a protobuf message,
and send it to the server, which handles this protobuf message and replies with another protobuf msg.

### Describe the workflow of Protocol Buffers?

First, you write a .proto file with the specifications, 
then you execute `protoc` and it creates code in your specified language.
Now you can use the code in your projects.

### What are the benefits of using protocol buffers?

It is very portable, and also backwards compatible.

### When is the use of protocol not recommended?

I think the question meant is "When is the use of Protocol Buffers not recommended?".
If a human readable format is desired, one should use JSON instead.

### List 3 different data types that can be used with protocol buffers?

- string
- int32
- double

## Hello World with gRPC

For the first basic task, I decided to implement a hello world program in C++.

One major obstacle was compiling the sources since I wanted to write a Makefile with automatic file detection.
At the end, I just copied the CMakeLists.txt from the tutorial on medium.com and modified it.

### proto file

First, I created a .proto file with a service and two messages:

```proto
syntax = "proto3";

package helloworld;

// The greeting service definition.
service Greeter {
    // Sends a greeting
    rpc SayHello (HelloRequest) returns (HelloReply) {}
}

// The request message containing the user's name.
message HelloRequest {
    string name = 1;
}

// The response message containing the greetings
message HelloReply {
    string message = 1;
}
```

I saved this file to *proto/helloworld.proto*

### server

For the server, I need to create a class that inherits `Greeter::Service`.
(Greeter is the name of the service that I specified in the .proto file)

The next step was to override the `SayHello` function.

The `RunServer` function is then used to run a server under a specific port.

#### Code snippets

```cpp
class HelloWorldImpl final : public Greeter::Service {
    Status SayHello(ServerContext* context, const HelloRequest* request,
                    HelloReply* response) override {
        std::string prefix{"Hello "};
        response->set_message(prefix + request->name());
        return Status::OK;
    }
};
```

This is the class with the overridden function:
- first, I create a string with the prefix ("Hello ")
- then I set the message in the response to "Hello " + name
- then I return the ok status code

```cpp
void RunServer(uint16_t port) {
  std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
  HelloWorldImpl service{};

  // grpc::EnableDefaultHealthCheckService(true);
  // grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}
```

### client

For the client, a Channel has to be created, and using it, a Stub can be created (with `Greeter::newStub()`).
After that, I can just call `stub->SayHello()`.

```cpp
class HelloWorldClient {
    public:
        HelloWorldClient(std::shared_ptr<grpc::Channel> channel) 
            : stub(Greeter::NewStub(channel)) {}

        std::string sendRequest(std::string name) {
            HelloRequest request{};
            request.set_name(name);

            // variable for the response
            HelloReply reply{};

            ClientContext context{};


            // actual rpc
            Status status = stub->SayHello(&context, request, &reply);

            if(status.ok()) {
                return reply.message();
            }
            else {
                std::cout << status.error_code() << ": " << status.error_message() << '\n';
                return "RPC Failed";
            }
        }
    private:
        std::unique_ptr<Greeter::Stub> stub;
};
```

In the constructor, a new stub is created from a channel.

`stub->SayHello()` executes the rpc.

```cpp
void RunClient() {
    std::string targetAddress{"0.0.0.0:4657"};
    HelloWorldClient client(
            CreateChannel(targetAddress, InsecureChannelCredentials())
            );

    std::string response{};
    std::string name{"Simon"};

    response = client.sendRequest(name);

    std::cout << response << '\n';
}
```

Here, a new channel with insecure credentials is created and put into the HelloWorldClient object.
Then, a request is sent and the response printed on stdout.


### running the program

I created a Makefile with the following commands:
- make proto: create the proto files
- make run: compile
- make s2: start the server
- make c2: start the client

Example:  
```bash
# create proto output
make proto

# compile with cmake
make run

# starting the server
cmake/build/server 4657
# output:
# owo running on port 4657
# Server listening on 0.0.0.0:4657

# starting a client
cmake/build/client
# output:
# Hello Simon
```

## Warehouse data with gRPC

The next task was to send a WarehouseData (from past exercises) via gRPC.

Following parts needed to be changed:  
- proto file
- Makefile
- CMakeLists
- the implementation class in server.cpp
- the stub in client.cpp
- printing the result in client.cpp

Diff: 

```diff
diff --git a/Makefile b/Makefile
index 8bd661c..c0cd001 100644
--- a/Makefile
+++ b/Makefile
@@ -2,9 +2,10 @@ CXXFLAGS = -std=c++20
 
 .PHONY: proto
 
-proto: proto/helloworld.proto
-	protoc -I proto --grpc_out=src/proto --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` proto/helloworld.proto
-	protoc -I proto --cpp_out=src/proto proto/helloworld.proto
+proto: proto/warehouse.proto
+	mkdir -p src/proto
+	protoc -I proto --grpc_out=src/proto --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` proto/warehouse.proto
+	protoc -I proto --cpp_out=src/proto proto/warehouse.proto
 
 SOURCES = $(shell (find src -name '*.cc';find src -name '*.cpp'))
 # SOURCES = (find src -name '*.cc';find src -name '*.cpp')
diff --git a/src/client.cpp b/src/client.cpp
index eabf6ed..1beacdb 100644
--- a/src/client.cpp
+++ b/src/client.cpp
@@ -1,5 +1,5 @@
-#include "proto/helloworld.grpc.pb.h"
-#include "proto/helloworld.pb.h"
+#include "proto/warehouse.grpc.pb.h"
+#include "proto/warehouse.pb.h"
 #include <grpcpp/channel.h>
 #include <grpcpp/client_context.h>
 #include <grpcpp/create_channel.h>
@@ -8,52 +8,69 @@
 #include <memory>
 #include <string>
 
-using namespace helloworld;
+using namespace warehouse;
 using namespace grpc;
 
-class HelloWorldClient {
+class WarehouseClient {
     public:
-        HelloWorldClient(std::shared_ptr<grpc::Channel> channel) 
-            : stub(Greeter::NewStub(channel)) {}
+        WarehouseClient(std::shared_ptr<grpc::Channel> channel) 
+            : stub(Warehouse::NewStub(channel)) {}
 
-        std::string sendRequest(std::string name) {
-            HelloRequest request{};
-            request.set_name(name);
+        WarehouseData sendRequest(int id) {
+            getDataRequest request{};
+            request.set_id(id);
 
             // variable for the response
-            HelloReply reply{};
+            WarehouseData reply{};
 
             ClientContext context{};
 
 
             // actual rpc
-            Status status = stub->SayHello(&context, request, &reply);
+            Status status = stub->getDataForID(&context, request, &reply);
 
             if(status.ok()) {
-                return reply.message();
+                return reply;
             }
             else {
                 std::cout << status.error_code() << ": " << status.error_message() << '\n';
-                return "RPC Failed";
+                return {};
             }
         }
     private:
-        std::unique_ptr<Greeter::Stub> stub;
+        std::unique_ptr<Warehouse::Stub> stub;
 };
 
 
 void RunClient() {
     std::string targetAddress{"0.0.0.0:4657"};
-    HelloWorldClient client(
+    WarehouseClient client(
             CreateChannel(targetAddress, InsecureChannelCredentials())
             );
 
-    std::string response{};
-    std::string name{"Simon"};
+    WarehouseData response{};
+    int id{-1};
+    std::cout << "Enter the id: ";
+    std::cin >> id;
 
-    response = client.sendRequest(name);
+    response = client.sendRequest(id);
 
-    std::cout << response << '\n';
+    printf("WarehouseID: %s\n name: %s\n adress: %s %s %s %s\n time: %s\n",
+            response.warehouseid().c_str(),
+            response.warehousename().c_str(),
+            response.warehouseaddress().c_str(),
+            response.warehousepostalcode().c_str(),
+            response.warehousecity().c_str(),
+            response.warehousecountry().c_str(),
+            response.timestamp().c_str());
+    for(Product p : response.productdata()) {
+        printf("  ProductID: %s\n   name: %s\n   category: %s\n   quantity: %d %s\n   ",
+                p.productid().c_str(),
+                p.productname().c_str(),
+                p.productcategory().c_str(),
+                p.productquantity(),
+                p.productunit().c_str());
+    }
 }
 
 int main() {
diff --git a/src/server.cpp b/src/server.cpp
index 4fb237b..f1b39d6 100644
--- a/src/server.cpp
+++ b/src/server.cpp
@@ -1,25 +1,42 @@
-#include "proto/helloworld.grpc.pb.h"
-#include "proto/helloworld.pb.h"
+#include "proto/warehouse.grpc.pb.h"
+#include "proto/warehouse.pb.h"
 #include <grpcpp/grpcpp.h>
+#include <grpcpp/support/config.h>
 #include <grpcpp/support/status.h>
 #include <iostream>
 #include <string>
 
-using namespace helloworld;
+using namespace warehouse;
 using namespace grpc;
 
-class HelloWorldImpl final : public Greeter::Service {
-    Status SayHello(ServerContext* context, const HelloRequest* request,
-                    HelloReply* response) override {
-        std::string prefix{"Hello "};
-        response->set_message(prefix + request->name());
+const string warehouseApplicationID{"1"};
+
+class WarehouseService final : public Warehouse::Service {
+    Status getDataForID(ServerContext* context, const getDataRequest* request,
+                    WarehouseData* response) override {
+        int id = request->id();
+        string idStr = std::to_string(id);
+        response->set_warehouseapplicationid(warehouseApplicationID);
+        response->set_warehouseid(idStr);
+        response->set_warehousename("name " + idStr);
+        response->set_warehouseaddress("adress " + idStr);
+        response->set_warehousepostalcode(std::to_string(1200 + id));
+        response->set_warehousecity("city " + idStr);
+        response->set_warehousecountry("country " + idStr);
+        response->set_timestamp(idStr);
+
+        Product* p1 = response->add_productdata();
+        p1->set_productname("product " + idStr);
+
+        Product* p2 = response->add_productdata();
+        p2->set_productname("product2 " + idStr);
         return Status::OK;
     }
 };
 
 void RunServer(uint16_t port) {
   std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
-  HelloWorldImpl service{};
+  WarehouseService service{};
 
   // grpc::EnableDefaultHealthCheckService(true);
   // grpc::reflection::InitProtoReflectionServerBuilderPlugin();
```

# Sources

- [official gRPC tutorial for C++](https://grpc.io/docs/languages/cpp/basics/)
- [medium.com tutorial for C++](https://medium.com/@shradhasehgal/get-started-with-grpc-in-c-36f1f39367f4)


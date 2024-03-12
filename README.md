# SYT Dezentrale Systeme GK861 gRPC

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
- make run: compile
- make s2: start the server
- make c2: start the client

Example:  
```bash
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


# Sources

- [official gRPC tutorial for C++](https://grpc.io/docs/languages/cpp/basics/)
- [medium.com tutorial for C++](https://medium.com/@shradhasehgal/get-started-with-grpc-in-c-36f1f39367f4)


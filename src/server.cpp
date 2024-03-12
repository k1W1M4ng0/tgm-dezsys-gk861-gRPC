#include "proto/helloworld.grpc.pb.h"
#include "proto/helloworld.pb.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>
#include <iostream>
#include <string>

using namespace helloworld;
using namespace grpc;

class HelloWorldImpl final : public Greeter::Service {
    Status SayHello(ServerContext* context, const HelloRequest* request,
                    HelloReply* response) override {
        std::string prefix{"Hello "};
        response->set_message(prefix + request->name());
        return Status::OK;
    }
};

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

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cerr << "not enough args\n";
        exit(1);
    }
    int port = std::stoi(argv[1]);
    std::cout << "uwu running on port " << port << '\n';

    RunServer(port);
    return 0;
}

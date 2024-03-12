#include "proto/helloworld.grpc.pb.h"
#include "proto/helloworld.pb.h"
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <iostream>
#include <memory>
#include <string>

using namespace helloworld;
using namespace grpc;

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

int main() {
    RunClient();
    return 0;
}

#include "proto/warehouse.grpc.pb.h"
#include "proto/warehouse.pb.h"
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <iostream>
#include <memory>
#include <string>

using namespace warehouse;
using namespace grpc;

class WarehouseClient {
    public:
        WarehouseClient(std::shared_ptr<grpc::Channel> channel) 
            : stub(Warehouse::NewStub(channel)) {}

        WarehouseData sendRequest(int id) {
            getDataRequest request{};
            request.set_id(id);

            // variable for the response
            WarehouseData reply{};

            ClientContext context{};


            // actual rpc
            Status status = stub->getDataForID(&context, request, &reply);

            if(status.ok()) {
                return reply;
            }
            else {
                std::cout << status.error_code() << ": " << status.error_message() << '\n';
                return {};
            }
        }
    private:
        std::unique_ptr<Warehouse::Stub> stub;
};


void RunClient() {
    std::string targetAddress{"0.0.0.0:4657"};
    WarehouseClient client(
            CreateChannel(targetAddress, InsecureChannelCredentials())
            );

    WarehouseData response{};
    int id{-1};
    std::cout << "Enter the id: ";
    std::cin >> id;

    response = client.sendRequest(id);

    printf("WarehouseID: %s\n name: %s\n adress: %s %s %s %s\n time: %s\n",
            response.warehouseid().c_str(),
            response.warehousename().c_str(),
            response.warehouseaddress().c_str(),
            response.warehousepostalcode().c_str(),
            response.warehousecity().c_str(),
            response.warehousecountry().c_str(),
            response.timestamp().c_str());
    for(Product p : response.productdata()) {
        printf("  ProductID: %s\n   name: %s\n   category: %s\n   quantity: %d %s\n   ",
                p.productid().c_str(),
                p.productname().c_str(),
                p.productcategory().c_str(),
                p.productquantity(),
                p.productunit().c_str());
    }
}

int main() {
    RunClient();
    return 0;
}

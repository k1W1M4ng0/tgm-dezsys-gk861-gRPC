#include "proto/warehouse.grpc.pb.h"
#include "proto/warehouse.pb.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/config.h>
#include <grpcpp/support/status.h>
#include <iostream>
#include <string>

using namespace warehouse;
using namespace grpc;

const string warehouseApplicationID{"1"};

class WarehouseService final : public Warehouse::Service {
    Status getDataForID(ServerContext* context, const getDataRequest* request,
                    WarehouseData* response) override {
        int id = request->id();
        string idStr = std::to_string(id);
        response->set_warehouseapplicationid(warehouseApplicationID);
        response->set_warehouseid(idStr);
        response->set_warehousename("name " + idStr);
        response->set_warehouseaddress("adress " + idStr);
        response->set_warehousepostalcode(std::to_string(1200 + id));
        response->set_warehousecity("city " + idStr);
        response->set_warehousecountry("country " + idStr);
        response->set_timestamp(idStr);

        Product* p1 = response->add_productdata();
        p1->set_productname("product " + idStr);

        Product* p2 = response->add_productdata();
        p2->set_productname("product2 " + idStr);
        return Status::OK;
    }
};

void RunServer(uint16_t port) {
  std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
  WarehouseService service{};

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

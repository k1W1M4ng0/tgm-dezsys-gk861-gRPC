import grpc
import warehouse_pb2
import warehouse_pb2_grpc

# Create a channel to the server
with grpc.insecure_channel('localhost:4657') as channel:
    # Create a stub (client)
    stub = warehouse_pb2_grpc.WarehouseStub(channel)

    # Create a request
    request = warehouse_pb2.getDataRequest(id=123)

    # Make the call
    response = stub.getDataForID(request)

    # Print the response
    print(response)

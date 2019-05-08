#include <grpcpp/resource_quota.h>
#include "person_manage.h"

// maxThreadNum 根据计算机硬件设置
const int maxThreadNum = 20;

void RunServer() {
    std::string server_address("localhost:50001");
    PersonService service;

    grpc::ServerBuilder builder;
    grpc::ResourceQuota quota;
    quota.SetMaxThreads(maxThreadNum);
    builder.SetResourceQuota(quota);
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();

    return 0;
}

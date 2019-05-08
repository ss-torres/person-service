#include <memory>
#include <vector>
#include <thread>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "person.grpc.pb.h"

class PersonManip {
public:
    PersonManip(std::shared_ptr<grpc::Channel> channel)
        : m_stub(tutorial::Manage::NewStub(channel)) {
    }

    // 添加一个用户
    bool AddPerson(const tutorial::Person& person) {
        grpc::ClientContext context;
        tutorial::Result res;
        grpc::Status status = m_stub->AddPerson(&context, person, &res);
        if (!status.ok()) {
            std::cout << "status error: " << status.error_message() << std::endl;
            return false;
        }
        return res.success();
    }

    // 添加多个用户, 当前的服务端实现可能造成部分插入的情况
    bool AddPersons(const std::vector<tutorial::Person>& persons) {
        grpc::ClientContext context;
        tutorial::Result res;
        std::unique_ptr<grpc::ClientWriter<tutorial::Person>> writer(
                m_stub->AddPersons(&context, &res));
        for (const auto& person : persons) {
            if (!writer->Write(person)) {
                // Broken stream.
                break;
            }
        }
        writer->WritesDone();
        grpc::Status status = writer->Finish();
        if (!status.ok()) {
            std::cout << "status error: " << status.error_message() << std::endl;
            return false;
        }

        return res.success();
    }

    // 获取限定数目的用户
    bool GetPersonsLimit(int limitNum, std::vector<tutorial::Person>& persons) {
        grpc::ClientContext context;
        tutorial::ReqNum limit;
        limit.set_num(limitNum);
        std::unique_ptr<grpc::ClientReader<tutorial::Person>> reader(
                m_stub->GetPersonsLimit(&context, limit));
        tutorial::Person person;
        while (reader->Read(&person)) {
            persons.push_back(person);
        }
        grpc::Status status = reader->Finish();
        if (!status.ok()) {
            std::cout << "status error: " << status.error_message() << std::endl;
            return false;
        }

        return true;
    }

    // 获取所有指定名字的用户
    bool GetPersons(const std::vector<std::string>& personNames, std::vector<tutorial::Person>& persons) {
        grpc::ClientContext context;

        std::shared_ptr<grpc::ClientReaderWriter<tutorial::ReqName, tutorial::Person>> stream(
                m_stub->GetPersons(&context));
        std::thread writer([stream, &personNames]() {
                for (const auto& personName : personNames) {
                    tutorial::ReqName name;
                    name.set_name(personName);
                    stream->Write(name);
                }
                stream->WritesDone();
        });

        tutorial::Person person;
        while (stream->Read(&person)) {
            persons.push_back(person);
        }
        writer.join();
        grpc::Status status = stream->Finish();
        if (!status.ok()) {
            std::cout << "status error: " << status.error_message() << std::endl;
            return false;
        }

        return true;
    }

private:
    std::unique_ptr<tutorial::Manage::Stub> m_stub;
};

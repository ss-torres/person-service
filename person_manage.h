#include <string>
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <folly/concurrency/ConcurrentHashMap.h>
#include "person.grpc.pb.h"

class PersonManager {
public:
    explicit PersonManager() {
    }

    // AddPerson 用来添加一个人
    bool AddPerson(const tutorial::Person& p) {
        m_persons.insert(p.name(), p);
        return true;
    }

    // GetPerson 用来查找一个人
    tutorial::Person GetPerson(const std::string& name) const {
        return m_persons.at(name);
    }

    // GetPersons 用来获取多个人
    std::vector<tutorial::Person> GetPersons(int num) const {
        std::vector<tutorial::Person> personList;
        auto it = m_persons.begin();
        while (it != m_persons.end()) {
            if (static_cast<int>(personList.size()) > num) {
                return personList;
            }
            personList.push_back(it->second);
            ++it;
        }
        return personList;
    }

private:
    folly::ConcurrentHashMap<std::string, tutorial::Person> m_persons;
};

class PersonService : public tutorial::Manage::Service {
public:
    explicit PersonService() {
    }

    // AddPerson 用来添加一个人
    grpc::Status AddPerson(grpc::ServerContext* context, const tutorial::Person *person, 
            tutorial::Result *res) override {
        m_mgr.AddPerson(*person);
        res->set_success(true);
        return grpc::Status::OK;
    }

    // AddPersons 用来添加多个用户
    grpc::Status AddPersons(grpc::ServerContext* context, grpc::ServerReader<tutorial::Person>* reader,
            tutorial::Result *res) override {
        tutorial::Person person;
        while (reader->Read(&person)) {
            m_mgr.AddPerson(person);
        }
        res->set_success(true);
        return grpc::Status::OK;
    }

    // GetPersonsLimit 用来查询一个人
    grpc::Status GetPersonsLimit(grpc::ServerContext* context, const tutorial::ReqNum *num,
            grpc::ServerWriter<tutorial::Person>* writer) override {
        auto persons = m_mgr.GetPersons(num->num());
        for (const auto& person : persons) {
            writer->Write(person);
        }
        return grpc::Status::OK;
    }

    // GetPersons 用来根据人名获取所有的人
    grpc::Status GetPersons(grpc::ServerContext *context, 
            grpc::ServerReaderWriter<tutorial::Person, tutorial::ReqName>* stream) override {
        tutorial::ReqName name;
        while (stream->Read(&name)) {
            try {
                stream->Write(m_mgr.GetPerson(name.name()));
            } catch (const std::out_of_range& ex) {
                // 如果出现越界的问题, 则说明不存在
            }
        }
        return grpc::Status::OK;
    }

private:
    PersonManager m_mgr;
};

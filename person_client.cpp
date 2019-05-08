#include "person_manip.h"

tutorial::Person makePerson(const std::string& name, int id, 
        const std::string& email) {
    tutorial::Person person;
    person.set_name(name);
    person.set_id(id);
    person.set_email(email);
    return person;
}

void printPersons(const std::vector<tutorial::Person>& persons) {
    for (const auto& p : persons) {
        std::cout << "name: " << p.name() << " "
            << "id: " << p.id() << " "
            << "email: " << p.email() << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char **argv) {
    PersonManip manip(
            grpc::CreateChannel("localhost:50001",
                grpc::InsecureChannelCredentials()));
    auto person = makePerson("Tom", 1, "tom@gmail.com");
    auto suc = manip.AddPerson(person);
    if (!suc) {
        std::cout << "manip.AddPerson failed." << std::endl;
        return -1;
    }

    person = makePerson("Lilly", 2, "lilly@gmail.com");
    auto person2 = makePerson("Jim", 3, "jim@gmail.com");
    
    std::vector<tutorial::Person> persons{person, person2};
    suc = manip.AddPersons(persons);
    if (!suc) {
        std::cout << "manip.AddPersons failed." << std::endl;
        return -1;
    }

    std::vector<tutorial::Person> resPersons;
    suc = manip.GetPersonsLimit(5, resPersons);
    if (!suc) {
        std::cout << "manip.GetPersonsLimit failed." << std::endl;
        return -1;
    }
    std::cout << "manip.GetPersonsLimit output:" << std::endl;
    printPersons(resPersons);

    resPersons.clear();
    std::vector<std::string> personNames;
    for (const auto& p : persons) {
        personNames.push_back(p.name());
    }
    suc = manip.GetPersons(personNames, resPersons);
    if (!suc) {
        std::cout << "manip.GetPersons failed." << std::endl;
        return -1;
    }
    std::cout << "manip.GetPersons output:" << std::endl;
    printPersons(resPersons);
    return 0;
}

#include "example_config.h"

#include <iostream>

int main()
{
    ///////////////////////////////////////
    std::cout << "This example shows how to parse json string and map it to a simple structure:" << std::endl;
    const char *appleJsonStr = R"({"name": "apple", "quantity": 10, "price": 1.23})";
    nlohmann::json appleJson = nlohmann::json::parse(appleJsonStr);
    Item apple = appleJson;
    std::cout << apple << std::endl;

    ///////////////////////////////////////
    std::cout << "\nThis example shows how to serialize/deserialize structure with optional field:" << std::endl;
    nlohmann::json j =
        R"([
{
    "name": "John",
    "address": "1234 Elm Street",
    "age": 30
},
{
    "name": "Jane",
    "address": "1234 Elm Street",
    "age": 30,
    "nickname": "JJ"
}])"_json;

    std::vector<Person> persons = j.get<std::vector<Person>>();
    for (const auto &person : persons)
    {
        std::cout << person << std::endl;
    }

    nlohmann::json jsonAgain = persons;
    std::cout << jsonAgain.dump(2) << std::endl;

    return 0;
}

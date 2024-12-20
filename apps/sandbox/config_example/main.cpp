#include <nlohmann/json.hpp>

#include <iostream>
#include <optional>
namespace nlohmann
{
template <typename T> struct adl_serializer<std::optional<T>>
{
    static void to_json(json &j, const std::optional<T> &opt)
    {
        if (!opt)
        {
            j = nullptr;
        }
        else
        {
            j = *opt;
        }
    }

    static void from_json(const json &j, std::optional<T> &opt)
    {
        if (j.is_null())
        {
            opt = std::nullopt;
        }
        else
        {
            opt = j.template get<T>();
        }
    }
};
} // namespace nlohmann

struct Person
{
    std::string name, address;
    int age;
    std::optional<std::string> nickname;

    friend std::ostream &operator<<(std::ostream &os, const Person &pers)
    {
        os << "Person {name: " << pers.name << ", address: " << pers.address << ", age: " << pers.age;
        if (pers.nickname)
        {
            os << ", nickname: " << *pers.nickname;
        }
        return os << '}';
    }
};

void to_json(nlohmann::json &j, const Person &p)
{
    j = nlohmann::json{{"name", p.name}, {"address", p.address}, {"age", p.age}, {"nickname", p.nickname}};
}
void from_json(const nlohmann::json &j, Person &p)
{
    j.at("name").get_to(p.name);
    j.at("address").get_to(p.address);
    j.at("age").get_to(p.age);
    if (j.contains("nickname"))
        j.at("nickname").get_to(p.nickname);
}

struct Item
{
    std::string name;
    int quantity;
    double price;

    friend std::ostream &operator<<(std::ostream &os, const Item &item)
    {
        os << "Item {name: " << item.name << ", quantity: " << item.quantity << ", price: " << item.price << '}';
        return os;
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Item, name, quantity, price);

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

#pragma once

#include <config/config.h>

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

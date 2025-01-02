#pragma once

#include <nlohmann/json.hpp>

#include <optional>

#define DEFINE_CONFIG(...) NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(__VA_ARGS__)
#define DEFINE_CONFIG_ENUM(...) NLOHMANN_JSON_SERIALIZE_ENUM(__VA_ARGS__)

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

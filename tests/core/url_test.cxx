#include <catch2/catch_all.hpp>
#include <util/url.h>

TEST_CASE("URL", "[core]")
{
    SECTION("URL no schema")
    {
        CHECK_THROWS_AS(URL("www.example.com"), std::logic_error);
    }
    SECTION("HTTPS URL with explicit port")
    {
        URL url("https://www.example.com:8080/path/to/resource?query=123#fragment");

        CHECK(url.scheme() == "https");
        CHECK(url.host() == "www.example.com");
        CHECK(url.port() == "8080");
        CHECK(url.path() == "/path/to/resource");
        CHECK(url.query() == "query=123");
        CHECK(url.fragment() == "fragment");
    }

    SECTION("WebSocket URL without port")
    {
        URL url("wss://www.example.com");

        CHECK(url.scheme() == "wss");
        CHECK(url.host() == "www.example.com");
        CHECK(url.port() == "443");
        CHECK(url.path() == "/");
        CHECK(url.query().empty());
        CHECK(url.fragment().empty());
    }

    SECTION("URL target")
    {
        CHECK(URL("https://www.example.com/path/to/resource?query=123#fragment").target() ==
              "/path/to/resource?query=123#fragment");
        CHECK(URL("wss://www.example.com").target() == "/");
    }
}

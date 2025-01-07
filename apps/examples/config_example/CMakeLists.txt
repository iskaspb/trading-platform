file(GLOB SRC *.cpp)
app(
    config_example EXCLUDE_FROM_ALL SRC ${SRC}
    LINK_OPTIONS nlohmann_json::nlohmann_json
)

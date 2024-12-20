file(GLOB SRC *.cpp)
app(
    config_example EXCLUDE_FROM_ALL SRC ${SRC}
    DEPENDENCIES ${BOOST_LIBS} nlohmann_json::nlohmann_json
)

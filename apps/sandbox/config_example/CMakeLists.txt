file(GLOB SRC *.cpp)
app(
    config_example EXCLUDE_FROM_ALL SRC ${SRC}
    DEPENDENCIES nlohmann_json::nlohmann_json #${BOOST_LIBS}
)

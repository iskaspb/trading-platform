file(GLOB SRC *.cpp)
app(
    app_example EXCLUDE_FROM_ALL SRC ${SRC}
    #COMPILER_OPTIONS ...
    LINK_OPTIONS ${Boost_LIBRARIES} nlohmann_json::nlohmann_json
)

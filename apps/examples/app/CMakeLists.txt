file(GLOB SRC *.cpp)
app(
    app_example EXCLUDE_FROM_ALL
    SRC ${SRC}
    #COMPILER_OPTIONS ...
    LINK_OPTIONS ${Boost_LIBRARIES} nlohmann_json::nlohmann_json
)

# this is example of a similar app, but with different name that uses the same config as the first app
app(
    app_example_other EXCLUDE_FROM_ALL
    SRC ${SRC}
    #COMPILER_OPTIONS ...
    LINK_OPTIONS ${Boost_LIBRARIES} nlohmann_json::nlohmann_json
)

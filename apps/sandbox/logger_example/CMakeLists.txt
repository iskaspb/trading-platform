file(GLOB SRC *.cpp)
app(
    logger_example EXCLUDE_FROM_ALL SRC ${SRC}
    DEPENDENCIES ${BOOST_LIBS}
)

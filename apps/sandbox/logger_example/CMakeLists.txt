file(GLOB SRC *.cpp)
app(
    logger_example EXCLUDE_FROM_ALL SRC ${SRC}
    LINK_OPTIONS ${BOOST_LIBS}
)

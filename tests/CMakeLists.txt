
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/tests" ${CMAKE_MODULE_PATH})
include(register_unit_test)

c2test(basic_test SRC basic/*_test.cxx)
c2test(md_test SRC md/*_test.cxx)
c2test(ob_test SRC ob/*_test.cxx)

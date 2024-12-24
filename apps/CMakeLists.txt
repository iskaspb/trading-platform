function(app APP_NAME)
    if(NOT APP_NAME)
        message(FATAL_ERROR "App name is not provided")
        return()
    endif()

    set(options EXCLUDE_FROM_ALL)
    set(multiValueArgs SRC COMPILE_OPTIONS LINK_OPTIONS)
    cmake_parse_arguments(ARG "${options}" "" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_SRC)
        message(FATAL_ERROR "SRC is not provided for app ${APP_NAME}")
        return()
    endif()

    if(ARG_EXCLUDE_FROM_ALL)
        add_executable(${APP_NAME} EXCLUDE_FROM_ALL ${ARG_SRC})
    else()
        add_executable(${APP_NAME} ${ARG_SRC})
    endif()

    if(ARG_COMPILE_OPTIONS)
        separate_arguments(ARG_COMPILE_OPTIONS)
        target_compile_options(${APP_NAME} PRIVATE ${ARG_COMPILE_OPTIONS})
    endif()
    target_include_directories(${APP_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

    if(ARG_LINK_OPTIONS)
        separate_arguments(ARG_LINK_OPTIONS)
        target_link_libraries(${APP_NAME} ${ARG_LINK_OPTIONS}) # you might want to add ${CMAKE_THREAD_LIBS_INIT}
    endif()

    if(NOT ARG_EXCLUDE_FROM_ALL)
        install(TARGETS ${APP_NAME} RUNTIME DESTINATION ${TARGET_BIN_DIR})
    endif()
endfunction()

add_subdirectory(md)
add_subdirectory(sandbox)

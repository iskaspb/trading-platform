function(app APP_NAME)
    if(NOT APP_NAME)
        message(FATAL_ERROR "App name is not provided")
        return()
    endif()

    set(options EXCLUDE_FROM_ALL)
    set(multiValueArgs SRC DEPENDENCIES)
    cmake_parse_arguments(ARG "${options}" "" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_SRC)
        message(FATAL_ERROR "SRC is not provided for app ${APP_NAME}")
        return()
    endif()

    if(TARGET_EXCLUDE_FROM_ALL)
        add_executable(${APP_NAME} EXCLUDE_FROM_ALL ${ARG_SRC})
    else()
        add_executable(${APP_NAME} ${ARG_SRC})
    endif()

    target_include_directories(${APP_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

    separate_arguments(ARG_DEPENDENCIES)
    target_link_libraries(${APP_NAME} ${ARG_DEPENDENCIES}) # you might want to add ${CMAKE_THREAD_LIBS_INIT}

    if(NOT TARGET_EXCLUDE_FROM_ALL)
        install(TARGETS ${APP_NAME} RUNTIME DESTINATION ${TARGET_BIN_DIR})
    endif()
endfunction()

add_subdirectory(md)
add_subdirectory(sandbox)

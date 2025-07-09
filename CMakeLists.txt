cmake_minimum_required(VERSION 3.24)

project(oryx-crt-cpp VERSION 0.0.1 LANGUAGES CXX)

option(ORYX_CRT_BUILD_SHARED_LIBS "Build shared library" ${BUILD_SHARED_LIBS})
option(ORYX_CRT_BUILD_TESTS "Build tests" OFF)
option(ORYX_CRT_INSTALL "Install the project" ${PROJECT_IS_TOP_LEVEL})

set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
endif()


if (ORYX_CRT_BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED)
    set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})
else()
    add_library(${PROJECT_NAME} STATIC)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall 
        -Wextra 
        -Wuninitialized 
        -Wno-unused-function 
        -Wunused-variable
    )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_sources(${PROJECT_NAME} PRIVATE 
    src/frame_rate_controller.cpp
)


if(ORYX_CRT_BUILD_TESTS)
    set(test_exe ${PROJECT_NAME}_tests)
    add_executable(${test_exe} 
        tests/main.cpp 
        tests/argparse_test.cpp
        tests/stopwatch_test.cpp
        tests/error_test.cpp
        tests/string_split_test.cpp
        tests/callback_list_test.cpp
    )
    target_link_libraries(${test_exe} PRIVATE ${PROJECT_NAME})
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_definitions(${test_exe} PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
    endif()
endif()

if (ORYX_CRT_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    configure_package_config_file(cmake/${PROJECT_NAME}-config.cmake.in
      ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
      INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )

    file(GLOB_RECURSE ORYX_CRT_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/include/*" )

    target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET oryx_headers
        TYPE HEADERS
        BASE_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        FILES ${ORYX_CRT_HEADERS}
    )

    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-exports
        FILE_SET oryx_headers DESTINATION ${INCLUDE_INSTALL_DIR}
    )

    install(
        EXPORT ${PROJECT_NAME}-exports
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        NAMESPACE oryx::
    )
endif ()
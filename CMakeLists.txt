cmake_minimum_required(VERSION 3.24)

project(oryx-crt-cpp VERSION 0.1.0 LANGUAGES CXX)

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
    target_compile_options(${PROJECT_NAME} 
        PRIVATE
            /W4
    )
else()
    target_compile_options(${PROJECT_NAME} 
        PRIVATE
            -Wall 
            -Wextra
            -Werror
            -Wuninitialized 
            -Wno-unused-function 
            -Wunused-variable
    )
endif()

target_include_directories(${PROJECT_NAME} 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

file(GLOB_RECURSE ORYX_CRT_HEADERS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*"
)

target_sources(${PROJECT_NAME} 
    PRIVATE 
        src/frame_rate_controller.cpp
        src/uuid.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES ${ORYX_CRT_HEADERS}
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
        tests/from_chars_test.cpp
        tests/is_one_off_test.cpp
        tests/unique_file_ptr_test.cpp
        tests/error_group_test.cpp
        tests/traits_test.cpp
    )

    target_link_libraries(${test_exe} PRIVATE ${PROJECT_NAME})
    
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_definitions(${test_exe} PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
    endif()
endif()

if (ORYX_CRT_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-version.cmake"
        COMPATIBILITY ExactVersion
    )

    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        FILE_SET HEADERS
    )

    install(
        FILES 
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
        COMPONENT ${PROJECT_NAME}
    )

    install(
        EXPORT ${PROJECT_NAME}-targets
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
        NAMESPACE oryx::
        FILE ${PROJECT_NAME}-targets.cmake
        COMPONENT ${PROJECT_NAME}
    )
endif ()
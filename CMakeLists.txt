cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
project(CloudCompanion VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(cmake/tools.cmake)

if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(
            FATAL_ERROR
            "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
    )
endif ()

include(cmake/CPM.cmake)

# PackageProject.cmake will be used to make our target installable
CPMAddPackage(
        NAME PackageProject.cmake
        GITHUB_REPOSITORY TheLartians/PackageProject.cmake
        VERSION 1.10.0
)

set(CMAKE_CXX_STANDARD 20)

file(GLOB actions CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/actions/implementations/*.hpp")
foreach (action IN LISTS actions)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/include/" "" action "${action}")
    string(CONCAT action_header "#include \"" "${action}" "\"\n")
    string(APPEND action_headers "${action_header}")

    string(REPLACE "actions/implementations/" "" action "${action}")
    string(REPLACE ".hpp" "" action "${action}")
    string(CONCAT action_insert "\tactions.insert(\"" "${action}" "\", new " "${action}" "());\n")
    string(APPEND action_inserts "${action_insert}")
endforeach ()

configure_file(source/actions/ActionDiscovery.cpp.in ActionDiscovery.cpp)

file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

add_library(CloudCompanion
        ${headers}
        ${sources}
        ${CMAKE_CURRENT_BINARY_DIR}/ActionDiscovery.cpp
        )

set_target_properties(CloudCompanion PROPERTIES
        CXX_STANDARD 17
        AUTOMOC TRUE)

#target_compile_options(CloudCompanion PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive>")

find_package(Qt6 REQUIRED COMPONENTS Core Concurrent)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(libssh CONFIG REQUIRED)
find_package(kubernetes CONFIG REQUIRED)

target_link_libraries(CloudCompanion
        PRIVATE Microsoft.GSL::GSL
        PUBLIC Qt::Core kubernetes::kubernetes)

target_include_directories(
        CloudCompanion PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

string(TOLOWER ${PROJECT_NAME}/version.h VERSION_HEADER_LOCATION)

packageProject(
        NAME ${PROJECT_NAME}
        VERSION ${PROJECT_VERSION}
        BINARY_DIR ${PROJECT_BINARY_DIR}
        INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
        INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
        VERSION_HEADER "${VERSION_HEADER_LOCATION}"
        DEPENDENCIES ""
)

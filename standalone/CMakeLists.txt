cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
option(INSTALL "Run deployment scripts" OFF)
project(CloudCompanionStandalone LANGUAGES CXX)

# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

CPMAddPackage(NAME CloudCompanion SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(libssh CONFIG REQUIRED)

find_package(AWSSDK CONFIG REQUIRED COMPONENTS core ec2 ecr sts eks)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

add_definitions(-D_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING)
find_package(Qt6 REQUIRED COMPONENTS
        Core
        Qml
        Gui
        Concurrent
        Quick
        QuickControls2
        Svg
        Xml
        Widgets
        Core5Compat)

find_package(Qaterial CONFIG COMPONENTS
        Qaterial Fonts Icons Components)

if (NOT Qaterial_FOUND)
    include(../cmake/FetchQaterial.cmake)
endif ()

include(../cmake/QtGeneratorCMake/cmake/GenerateQmldir.cmake)
include(../cmake/QtGeneratorCMake/cmake/GenerateQrc.cmake)

qt_generate_qmldir(CLOUDCOMPANIONSTANDALONE_QMLDIR_FILE
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml/CloudCompanionStandalone
        MODULE "CloudCompanionStandalone")

qt_generate_qrc(CLOUDCOMPANIONSTANDALONE_QML_QRC
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml/CloudCompanionStandalone
        DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/CloudCompanionStandaloneGenerated
        NAME CloudCompanionStandalone.qrc
        PREFIX "CloudCompanionStandalone"
        GLOB_EXPRESSION "*.qml;qmldir"
        RECURSE)

qt_generate_qrc(CLOUDCOMPANIONSTANDALONE_RES_QML_QRC
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml/CloudCompanionStandalone
        DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/CloudCompanionStandaloneGenerated
        NAME CloudCompanionStandaloneRes.qrc
        PREFIX "CloudCompanionStandalone"
        GLOB_EXPRESSION "*.png;*svg"
        RECURSE)

qt6_add_resources(CLOUDCOMPANIONSTANDALONE_QML_RES ${CLOUDCOMPANIONSTANDALONE_QML_QRC})
file(GLOB_RECURSE sources CONFIGURE_DEPENDS source/*.cpp)
file(GLOB_RECURSE headers CONFIGURE_DEPENDS include/*.hpp)
file(GLOB_RECURSE CLOUDCOMPANIONSTANDALONE_QML_SRCS qml/CloudCompanionStandalone/*.qml)
source_group("Qml" FILES "${CLOUDCOMPANIONSTANDALONE_QML_SRCS}")

set(CLOUDCOMPANIONSTANDALONE_SRCS
        ${sources}
        ${headers}
        ${CLOUDCOMPANIONSTANDALONE_QML_RES}
        ${CLOUDCOMPANIONSTANDALONE_RES_QML_QRC}
        )

# Don't Run Moc on cpp file generated from qml
foreach (GENERATED_RES ${CLOUDCOMPANIONSTANDALONE_QML_RES})
    set_property(SOURCE ${GENERATED_RES} PROPERTY SKIP_AUTOMOC ON)
    source_group("QtQuickCompiler Files" FILES ${GENERATED_RES})
endforeach ()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(MACOSX_BUNDLE_ICON_FILE cloudcompanion.icns)
    set(APP_ICON_MACOSX ../platforms/macos/cloudcompanion.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
endif ()

qt_add_executable(CloudCompanionStandalone WIN32 MACOSX_BUNDLE
        ${APP_ICON_MACOSX}
        ${CLOUDCOMPANIONSTANDALONE_SRCS})

set_property(TARGET CloudCompanionStandalone APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/../platforms/android)

set_property(TARGET CloudCompanionStandalone APPEND PROPERTY QT_ANDROID_SDK_BUILD_TOOLS_REVISION
        33.0.1)

set_property(TARGET CloudCompanionStandalone APPEND PROPERTY QT_WASM_INITIAL_MEMORY 512MB)

target_link_libraries(CloudCompanionStandalone PUBLIC
        CloudCompanion
        Qt::Widgets
        Qt::Quick
        Qt::Concurrent
        Qt::Core5Compat
        Qaterial::Qaterial
        ${AWSSDK_LINK_LIBRARIES}
        Microsoft.GSL::GSL
        ssh
        )

if (INSTALL)
    include(../platforms/Deploy.cmake)
endif ()

target_include_directories(
        CloudCompanionStandalone PRIVATE include/
)

#target_compile_options(CloudCompanionStandalone PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive>")
if (WIN32)
    add_compile_definitions(NOMINMAX)
endif ()

add_custom_target(solutions ALL COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/solutions.json
        $<TARGET_FILE_DIR:CloudCompanionStandalone>/solutions.json
        COMMENT "Copying solutions.json")

include(../cmake/PVS-Studio.cmake)
pvs_studio_add_target(TARGET CloudCompanionStandalone.analyze
        OUTPUT FORMAT html
        ANALYZE CloudCompanion CloudCompanionStandalone
        CONFIG ${CMAKE_CURRENT_LIST_DIR}/PVS-Studio.cfg
        LOG PVS-Studio.html
        HIDE_HELP
        )

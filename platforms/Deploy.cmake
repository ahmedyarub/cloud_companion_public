get_target_property(QT_TARGET_TYPE Qt6::Core TYPE)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    include(../cmake/QtWindowsCMake/AddQtWindowsExe.cmake)

    # Don't deploy when using static cmake since we are not using any qml file
    if (${QT_TARGET_TYPE} STREQUAL "STATIC_LIBRARY")
        set(PLATFORM_NO_DEPLOY NO_DEPLOY)
    endif ()

    add_qt_windows_exe(CloudCompanionStandalone
            NAME "Cloud Companion"
            VERSION 0.0.1
            PUBLISHER "Mind Power"
            PRODUCT_URL "https://www.linkedin.com/in/ahmed-yarub-hani-al-nuaimi-77186267/"
            PACKAGE "com.mindpower.cloud_companion"
            ICON ${CMAKE_CURRENT_SOURCE_DIR}/../platforms/windows/icon.ico
            ICON_RC ${CMAKE_CURRENT_SOURCE_DIR}/../platforms/windows/icon.rc
            QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml
            NO_TRANSLATIONS
            VERBOSE_INSTALLER
            ${PLATFORM_NO_DEPLOY}
            )

    if (MSVC)
        set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CloudCompanionStandalone)
    endif ()
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    include(../cmake/QtLinuxCMake/cmake/AddQtLinuxAppImage.cmake)

    add_qt_linux_appimage(CloudCompanionStandalone
            APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../platforms/linux/AppDir
            QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml
            NO_COPYRIGHT_FILES
            )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set_source_files_properties(cloudcompanion.icns PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")

    include(../cmake/QtMacosCMake/AddQtMacosDmg.cmake)

    add_qt_macos_dmg(CloudCompanionStandalone
            ALL
            NAME "CloudCompanion"
            QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml
            OVERWRITE_EXISTING
            )
endif ()

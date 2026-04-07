cmake_minimum_required(VERSION 3.16)

project(QuickStatus2 VERSION 0.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

set(WPILIB_CPP "${CMAKE_CURRENT_SOURCE_DIR}/wpicpp")

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg)

qt_standard_project_setup()

set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.icns")

qt_add_executable(QuickStatus2
    MACOSX_BUNDLE
    src/Main.cpp
    src/MainWindow.cpp
    src/FuelWidget.cpp
    src/NTManager.cpp
    src/NTPopup.cpp
    src/AutoPopup.cpp
    src/ShiftWidget.cpp
    src/AutoWidget.cpp
    src/SwerveWidget.cpp
    src/ShooterWidget.cpp
    src/ControlModeWidget.cpp
    src/SpacerWidget.cpp
    src/StatusBar.cpp
    include/MainWindow.h
    include/FuelWidget.h
    include/NTManager.h
    include/NTPopup.h
    include/AutoPopup.h
    include/ShiftWidget.h
    include/AutoWidget.h
    include/SwerveWidget.h
    include/ShooterWidget.h
    include/ControlModeWidget.h
    include/SpacerWidget.h
    include/StatusBar.h
    resources/resources.qrc
    ${app_icon_macos}
)

target_include_directories(QuickStatus2 PRIVATE include src)

target_include_directories(QuickStatus2 PRIVATE
    ${WPILIB_CPP}/include/wpimath
    ${WPILIB_CPP}/include/wpilibc
    ${WPILIB_CPP}/include/wpiutil
    ${WPILIB_CPP}/include/wpinet
    ${WPILIB_CPP}/include/hal
    ${WPILIB_CPP}/include/ntcore
)

target_link_directories(QuickStatus2 PRIVATE ${WPILIB_CPP}/lib)

target_link_libraries(QuickStatus2 PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
    wpiutil
    wpinet
    ntcore
    wpimath
    wpilibc
    wpiHal
)

set_source_files_properties(${app_icon_macos}
    PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
)

set_target_properties(QuickStatus2 PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_ICON_FILE "icon.icns"
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS QuickStatus2
    BUNDLE DESTINATION .
    RUNTIME DESTINATION .
)

if(WIN32)
    enable_language(RC)
    target_sources(QuickStatus2 PRIVATE
        resources/app.rc
        resources/icon.ico
    )

    add_custom_command(TARGET QuickStatus2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${WPILIB_CPP}/lib"
            "$<TARGET_FILE_DIR:QuickStatus2>"
    )

    add_custom_command(TARGET QuickStatus2 POST_BUILD
        COMMAND "${Qt6_DIR}/../../../bin/windeployqt.exe"
            --no-translations
            $<TARGET_FILE:QuickStatus2>
        COMMENT "Running windeployqt..."
    )

    install(DIRECTORY "$<TARGET_FILE_DIR:QuickStatus2>/"
        DESTINATION .
        FILES_MATCHING
            PATTERN "*.dll"
            PATTERN "*.exe"
    )

    foreach(plugin_dir platforms imageformats iconengines styles)
        install(DIRECTORY "$<TARGET_FILE_DIR:QuickStatus2>/${plugin_dir}"
            DESTINATION .
            OPTIONAL
        )
    endforeach()

    file(WRITE "${CMAKE_BINARY_DIR}/qt.conf" "[Paths]\nPlugins=.\n")
    install(FILES "${CMAKE_BINARY_DIR}/qt.conf" DESTINATION .)

    file(WRITE "${CMAKE_BINARY_DIR}/README.txt"
"To run, just open QuickStatus2.exe.
Keep all files in this folder together, otherwise program may break.
Check this out if you want to set this as the default dashboard for FRC Driver Station: https://docs.wpilib.org/en/stable/docs/software/driverstation/manually-setting-the-driver-station-to-start-custom-dashboard.html

Version: ${PROJECT_VERSION}
")
    install(FILES "${CMAKE_BINARY_DIR}/README.txt" DESTINATION .)

    set(CPACK_GENERATOR "ZIP")
    set(CPACK_PACKAGE_NAME "QuickStatus2")
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_PACKAGE_FILE_NAME "QuickStatus2-${PROJECT_VERSION}-win64")
    set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)
    include(CPack)
endif()
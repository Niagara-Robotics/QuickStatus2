cmake_minimum_required(VERSION 3.16)

project(QuickStatus2 VERSION 0.1 LANGUAGES CXX)

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
    include/StatusBar.h
    resources/resources.qrc
    ${app_icon_macos}
)

target_include_directories(QuickStatus2 PRIVATE include)
target_include_directories(QuickStatus2 PRIVATE src)

target_include_directories(QuickStatus2 PRIVATE
    # ${WPILIB_CPP}/include
    ${WPILIB_CPP}/include/wpimath
    ${WPILIB_CPP}/include/wpilibc
    ${WPILIB_CPP}/include/wpiutil
    ${WPILIB_CPP}/include/wpinet
    ${WPILIB_CPP}/include/hal
    ${WPILIB_CPP}/include/ntcore
)

target_link_directories(QuickStatus2 PRIVATE
    ${WPILIB_CPP}/lib
)

target_link_libraries(QuickStatus2 PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg

    wpiutil
    wpinet
    ntcore
    # cscore
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

include(GNUInstallDirs)
install(TARGETS QuickStatus2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(WIN32)
    enable_language(RC)
    target_sources(QuickStatus2 PRIVATE
        resources/app.rc
        resources/icon.ico
    )
endif()

if(WIN32)
    # Copy WPILib DLLs
    add_custom_command(TARGET QuickStatus2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${WPILIB_CPP}/lib"
            $<TARGET_FILE_DIR:QuickStatus2>
    )

    # Copy Qt DLLs
    add_custom_command(TARGET QuickStatus2 POST_BUILD
        COMMAND "${Qt6_DIR}/../../../bin/windeployqt.exe"
            --no-translations
            $<TARGET_FILE:QuickStatus2>
        COMMENT "Running windeployqt..."
    )
endif()
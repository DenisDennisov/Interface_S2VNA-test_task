cmake_minimum_required(VERSION 3.16)

project(interface_test_task VERSION 0.1 LANGUAGES CXX)

add_link_options(-static-libgcc -static-libstdc++)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc
     "IDI_ICON1 ICON DISCARDABLE \"${CMAKE_CURRENT_SOURCE_DIR}/images/my_app.ico\"")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets
    Network
    Charts
)

set(PROJECT_SOURCES
        main.cpp
        # Model
        src/Model/VnaConfig.h
        src/Model/VnaConfig.cpp
        src/Model/VnaScpiClient.h
        src/Model/VnaScpiClient.cpp
        # Interfaces
        src/Interfaces/IVnaModel.h
        src/Interfaces/IVnaView.h
        src/Interfaces/IVnaConfig.h
        # Presenter
        src/Presenter/MeasurementPresenter.h
        src/Presenter/MeasurementPresenter.cpp
        # View
        src/View/mainwindow.h
        src/View/mainwindow.cpp
        src/View/mainwindow.ui
        # Workers
        src/Workers/VnaWorker.h
        src/Workers/VnaWorker.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(interface_test_task
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET interface_test_task APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(interface_test_task SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(interface_test_task
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(interface_test_task PRIVATE src)

target_link_libraries(interface_test_task PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Charts
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.interface_test_task)
endif()
set_target_properties(interface_test_task PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS interface_test_task
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(interface_test_task)
endif()

cmake_minimum_required(VERSION 3.5)
project(RedBerry)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets Network REQUIRED)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/pentest.cpp
    src/nmap_scanner.cpp
    src/settings.cpp
    src/historique.cpp
    resources.qrc
)

set(HEADERS
    include/mainwindow.h
    include/pentest.h
    include/nmap_scanner.h
    include/settings.h
    include/historique.h
)

set(RESOURCES
    resources.qrc
)

add_executable(RedBerry
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

target_link_libraries(RedBerry PRIVATE
    Qt6::Widgets
    Qt6::Network
)

target_include_directories(RedBerry PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
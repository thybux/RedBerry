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
    src/pages/pentest.cpp
    src/libs/nmap_scanner.cpp
    src/pages/settings.cpp
    src/pages/historique.cpp
    src/pages/scan_result_page.cpp
    src/libs/nmap_parser.cpp
    resources.qrc
)

set(HEADERS
    src/mainwindow.h
    src/pages/include/pentest.h
    src/libs/include/nmap_scanner.h
    src/pages/include/settings.h
    src/pages/include/historique.h
    src/pages/include/scan_result_page.h
    src/libs/include/nmap_parser.h
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
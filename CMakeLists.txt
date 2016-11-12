cmake_minimum_required(VERSION 3.0)

project(${PROJECT_NAME} client_server)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

set(SOURCES ${SOURCE_DIR}/client.c ${SOURCE_DIR}/server.c ${SOURCE_DIR}/client_server.c ${SOURCE_DIR}/common.c)
set(HEADERS ${INCLUDE_DIR}/client.h ${INCLUDE_DIR}/server.h ${INCLUDE_DIR}/common.h)

include_directories(${INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
target_link_libraries(${PROJECT_NAME} pthread)

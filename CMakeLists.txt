cmake_minimum_required(VERSION 3.21)

# Project Name and App Name
set(APP_NAME "example")
set(APP_PROJECT "PROJECT THREADMASTERR")

# Folder Names and Locations
set(SOURCE "./src")
set(BINARIES "./bin")

# Set the project name and version
project(${APP_PROJECT} VERSION 1.0)

# Specify the C standard
set(CMAKE_C_STANDARD 17)

# Include header files from the include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${APP_NAME} ${SOURCE}/main.c ${CMAKE_SOURCE_DIR}/include/threadlib/threadlib.c)

# Display a message when compiling
message(STATUS "Compiling ${APP_NAME}...")
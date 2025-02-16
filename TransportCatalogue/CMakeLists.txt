# Set cmake version
cmake_minimum_required(VERSION 3.11)

# Set the project name
project(TransportCatalogueProject)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files
file(GLOB SOURCES "src/*.cpp")


# Add the executable
add_executable(TransportCatalogue ${SOURCES})

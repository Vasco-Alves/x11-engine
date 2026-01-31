cmake_minimum_required(VERSION 3.20)

project(X11EngineWorkspace
    VERSION 3.0
    DESCRIPTION "X11 Engine for learning purposes"
    LANGUAGES CXX
)

# Global C++ Settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories for built binaries/libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable optimizations for Release builds
# set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -funroll-loops")
# set(CMAKE_BUILD_TYPE Release)

# Include subprojects
add_subdirectory(engine)
add_subdirectory(sandbox)

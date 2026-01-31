project(X11Engine)

# 1. Find Dependencies
find_package(X11 REQUIRED)

# 2. Gather Source Files
# (It's better to list them explicitly, but GLOB works for prototyping)
file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp")

# 3. Create the Library Target
add_library(X11Engine STATIC ${ENGINE_SOURCES})

# 4. Setup Include Directories
target_include_directories(X11Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 5. Link Dependencies
target_link_libraries(X11Engine PRIVATE X11::X11)
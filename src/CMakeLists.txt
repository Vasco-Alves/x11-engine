# 1. Find the X11 package required for Xlib.h and linking
find_package(X11 REQUIRED)

# 2. Define the executable target using your main source file
add_executable(X11Engine main.cpp)

# 3. Add the project's include directory so "renderer.hpp" can be found
# ${CMAKE_SOURCE_DIR} refers to the root folder (X11_engine/)
target_include_directories(X11Engine PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 4. Link against X11
# This automatically handles the include paths for Xlib and adds the "-lX11" flag.
target_link_libraries(X11Engine PRIVATE X11::X11)
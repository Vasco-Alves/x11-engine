project(Sandbox)

# 1. Add Executable
add_executable(Sandbox src/main.cpp)

# 2. Link against the Engine
target_link_libraries(Sandbox PRIVATE X11Engine)
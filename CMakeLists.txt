cmake_minimum_required(VERSION 3.8)

project(MyOpenGL)

option(USE_CUDA "Enable CUDA support" OFF)

if(USE_CUDA)
    add_definitions(-DUSE_CUDA)
    enable_language(CUDA)
    find_package(CUDA REQUIRED)
endif()




# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

set(GLFW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Libraries/include/GLFW)
set(GLFW_LIB_DIR ${CMAKE_SOURCE_DIR}/Libraries/lib)

# Find and link GLFW library
#find_package(glfw3 REQUIRED)
find_package(Threads REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})  # This line is optional, but it's good practice

link_directories(${GLFW_LIB_DIR})

# Include directories for header files
include_directories(
    include 
    headers
    Libraries/include
    #Libraries/include/GLFW
)


# Add glad.c to your project
add_library(glad glad.c)

file(GLOB IMGUI_SOURCES "Libraries/include/ImGui/*.cpp")
message(STATUS IMGUI_SOURCES: ${IMGUI_SOURCES})

add_library(imgui ${IMGUI_SOURCES})

# Source files
file(GLOB SOURCES "src/*.cpp")

if(USE_CUDA)
    file(GLOB_RECURSE CUDA_SOURCES "src/*.cu")
    list(APPEND SOURCES ${CUDA_SOURCES})
endif()

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

# Copy shader files to the build directory
file(GLOB SHADER_FILES "${SHADER_DIR}/*.glsl")
file(COPY ${SHADER_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)

# Create executable
add_executable(MyOpenGLApp ${SOURCES})

target_link_libraries(MyOpenGLApp
    imgui
    Threads::Threads
    glad  # Example for linking against OpenGL
    glfw3
    #dl
)


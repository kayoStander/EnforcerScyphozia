cmake_minimum_required(VERSION 3.10)
project(EnforcerSyphozia VERSION 1.0)

# Compiler settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-sign-conversion -Wstrict-overflow=2 -mcmodel=small -Wvla -O3")

# Include directories
include_directories(
        ${CMAKE_SOURCE_DIR}/game
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/external
        ${CMAKE_SOURCE_DIR}/external/imgui
        /usr/include  # Ensure this is where discord-rpc.h is located
)

# Source files: Recursively find all .cpp files
file(GLOB_RECURSE SRCS
        game/*.cpp
        src/*.cpp
        src/systems/*.cpp
        common/*.cpp
        common/logging/*.cpp
)

file(GLOB_RECURSE HDRS
        game/*.hpp
        src/*.hpp
        src/systems/*.hpp
        common/*.hpp
        common/logging/*.hpp
)


# ImGui source files
file(GLOB IMGUI_SRCS
        external/imgui/*.cpp
        external/imgui/backends/imgui_impl_vulkan.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
)

# Add all ImGui files to the main SRCS list
list(APPEND SRCS ${IMGUI_SRCS})

# Ensure SRCS is populated
if(NOT SRCS)
    message(FATAL_ERROR "No source files found! Please check your source file directories.")
endif()

# Find Vulkan library explicitly in the system path
find_library(VULKAN_LIBRARY NAMES Vulkan libvulkan.so PATHS /usr/lib/x86_64-linux-gnu)

# If Vulkan is found, print its path
if(VULKAN_LIBRARY)
    message(STATUS "Found Vulkan library at ${VULKAN_LIBRARY}")
else()
    message(FATAL_ERROR "Could not find Vulkan library!")
endif()

# Find other libraries
find_library(GLFW_LIBRARY glfw PATHS /usr/lib /lib)
find_library(FMT_LIBRARY fmt PATHS /usr/lib /lib)

# Ensure the libraries are found
if(GLFW_LIBRARY)
    message(STATUS "Found GLFW library")
else()
    message(FATAL_ERROR "Could not find glfw library.")
endif()

if(FMT_LIBRARY)
    message(STATUS "Found FMT library")
else()
    message(FATAL_ERROR "Could not find fmt library.")
endif()

# Find the Discord RPC static library
find_library(DISCORD_RPC_LIBRARY NAMES discord-rpc libdiscord-rpc.a PATHS /usr/lib /usr/local/lib)

if(NOT DISCORD_RPC_LIBRARY)
    message(FATAL_ERROR "Could not find Discord RPC library!")
else()
    message(STATUS "Found Discord RPC library at ${DISCORD_RPC_LIBRARY}")
endif()

# Define the target executable
add_executable(${PROJECT_NAME} ${SRCS} ${HDRS})

# Link dependencies
target_link_libraries(${PROJECT_NAME}
        ${VULKAN_LIBRARY}   # Vulkan
        ${GLFW_LIBRARY}     # GLFW
        ${FMT_LIBRARY}      # fmt
        ${DISCORD_RPC_LIBRARY}  # Discord RPC
)

# Optional: If you want to set additional preprocessor definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)

# Custom install command (to install ImGui if it's not present)
install(CODE "
    if(EXISTS \${CMAKE_SOURCE_DIR}/external/imgui)
        message(STATUS \"ImGui already installed.\")
    else()
        execute_process(COMMAND git clone https://github.com/ocornut/imgui \${CMAKE_SOURCE_DIR}/external/imgui)
    endif()
")

# Rename 'clean' to 'custom_clean' to avoid CMake conflict
add_custom_target(custom_clean
        COMMAND rm -f ${PROJECT_NAME}
        COMMAND rm -f ${SRCS}/*.o
        COMMENT "Cleaning project"
)

# Ensure proper rebuild behavior
add_custom_target(rebuild
        COMMAND ${CMAKE_BUILD_TOOL} custom_clean
        COMMAND ${CMAKE_BUILD_TOOL} all
        COMMENT "Rebuilding project"
)
set(NAME EnforcerScyphozia)

project(${NAME} VERSION 0.23.0)

if (DEFINED VULKAN_SDK_PATH)
  set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include")
  set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib")
  set(Vulkan_FOUND "True")
else()
  find_package(Vulkan REQUIRED)
  message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if (NOT Vulkan_FOUND)
  message(FATAL_ERROR "Could not find Vulkan library")
else()
  message(STATUS "Using Vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

# TODO:

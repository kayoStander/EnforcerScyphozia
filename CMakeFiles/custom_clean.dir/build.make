# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/heck/prog/cpp/vulkan/EnforcerScyphozia

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/heck/prog/cpp/vulkan/EnforcerScyphozia

# Utility rule file for custom_clean.

# Include any custom commands dependencies for this target.
include CMakeFiles/custom_clean.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/custom_clean.dir/progress.make

CMakeFiles/custom_clean:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/heck/prog/cpp/vulkan/EnforcerScyphozia/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Cleaning project"
	rm -f EnforcerSyphozia
	rm -f /home/heck/prog/cpp/vulkan/EnforcerScyphozia/common/assert.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/common/config.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/common/discord.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/common/logging/filter.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/common/logging/text_formatter.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/game/game_ability.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/game/game_ai.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/game/game_item.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/game/game_perk.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/game/game_player.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/application.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_buffer.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_camera.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_descriptors.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_device.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_game_object.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_interface.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_model.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_pipeline.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_raycast.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_renderer.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_server.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_sound.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_swap_chain.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_texture.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/enf_window.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/keyboard.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/main.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/systems/enf_ocean_system.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/systems/enf_physics_system.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/systems/enf_point_light_system.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/systems/enf_render_system.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/systems/enf_shadow_map_system.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/src/systems/enf_skybox_system.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/external/imgui/backends/imgui_impl_glfw.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/external/imgui/backends/imgui_impl_vulkan.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/external/imgui/imgui.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/external/imgui/imgui_demo.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/external/imgui/imgui_draw.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/external/imgui/imgui_tables.cpp /home/heck/prog/cpp/vulkan/EnforcerScyphozia/external/imgui/imgui_widgets.cpp/*.o

custom_clean: CMakeFiles/custom_clean
custom_clean: CMakeFiles/custom_clean.dir/build.make
.PHONY : custom_clean

# Rule to build all files generated by this target.
CMakeFiles/custom_clean.dir/build: custom_clean
.PHONY : CMakeFiles/custom_clean.dir/build

CMakeFiles/custom_clean.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/custom_clean.dir/cmake_clean.cmake
.PHONY : CMakeFiles/custom_clean.dir/clean

CMakeFiles/custom_clean.dir/depend:
	cd /home/heck/prog/cpp/vulkan/EnforcerScyphozia && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/heck/prog/cpp/vulkan/EnforcerScyphozia /home/heck/prog/cpp/vulkan/EnforcerScyphozia /home/heck/prog/cpp/vulkan/EnforcerScyphozia /home/heck/prog/cpp/vulkan/EnforcerScyphozia /home/heck/prog/cpp/vulkan/EnforcerScyphozia/CMakeFiles/custom_clean.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/custom_clean.dir/depend


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

# Utility rule file for rebuild.

# Include any custom commands dependencies for this target.
include CMakeFiles/rebuild.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/rebuild.dir/progress.make

CMakeFiles/rebuild:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/heck/prog/cpp/vulkan/EnforcerScyphozia/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Rebuilding project"
	/usr/bin/gmake custom_clean
	/usr/bin/gmake all

rebuild: CMakeFiles/rebuild
rebuild: CMakeFiles/rebuild.dir/build.make
.PHONY : rebuild

# Rule to build all files generated by this target.
CMakeFiles/rebuild.dir/build: rebuild
.PHONY : CMakeFiles/rebuild.dir/build

CMakeFiles/rebuild.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/rebuild.dir/cmake_clean.cmake
.PHONY : CMakeFiles/rebuild.dir/clean

CMakeFiles/rebuild.dir/depend:
	cd /home/heck/prog/cpp/vulkan/EnforcerScyphozia && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/heck/prog/cpp/vulkan/EnforcerScyphozia /home/heck/prog/cpp/vulkan/EnforcerScyphozia /home/heck/prog/cpp/vulkan/EnforcerScyphozia /home/heck/prog/cpp/vulkan/EnforcerScyphozia /home/heck/prog/cpp/vulkan/EnforcerScyphozia/CMakeFiles/rebuild.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/rebuild.dir/depend

# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/exchizz/ROS/Projects/UAV/mavlink/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/exchizz/ROS/Projects/UAV/mavlink/build

# Utility rule file for mavlink_gencfg.

# Include the progress variables for this target.
include CMakeFiles/mavlink_gencfg.dir/progress.make

CMakeFiles/mavlink_gencfg: /home/exchizz/ROS/Projects/UAV/mavlink/devel/include/mavlink/p_adjustConfig.h
CMakeFiles/mavlink_gencfg: /home/exchizz/ROS/Projects/UAV/mavlink/devel/lib/python2.7/dist-packages/mavlink/cfg/p_adjustConfig.py

/home/exchizz/ROS/Projects/UAV/mavlink/devel/include/mavlink/p_adjustConfig.h: /home/exchizz/ROS/Projects/UAV/mavlink/src/cfg/p_adjust.cfg
/home/exchizz/ROS/Projects/UAV/mavlink/devel/include/mavlink/p_adjustConfig.h: /opt/ros/indigo/share/dynamic_reconfigure/cmake/../templates/ConfigType.py.template
/home/exchizz/ROS/Projects/UAV/mavlink/devel/include/mavlink/p_adjustConfig.h: /opt/ros/indigo/share/dynamic_reconfigure/cmake/../templates/ConfigType.h.template
	$(CMAKE_COMMAND) -E cmake_progress_report /home/exchizz/ROS/Projects/UAV/mavlink/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating dynamic reconfigure files from cfg/p_adjust.cfg: /home/exchizz/ROS/Projects/UAV/mavlink/devel/include/mavlink/p_adjustConfig.h /home/exchizz/ROS/Projects/UAV/mavlink/devel/lib/python2.7/dist-packages/mavlink/cfg/p_adjustConfig.py"
	catkin_generated/env_cached.sh /home/exchizz/ROS/Projects/UAV/mavlink/src/cfg/p_adjust.cfg /opt/ros/indigo/share/dynamic_reconfigure/cmake/.. /home/exchizz/ROS/Projects/UAV/mavlink/devel/share/mavlink /home/exchizz/ROS/Projects/UAV/mavlink/devel/include/mavlink /home/exchizz/ROS/Projects/UAV/mavlink/devel/lib/python2.7/dist-packages/mavlink

/home/exchizz/ROS/Projects/UAV/mavlink/devel/share/mavlink/docs/p_adjustConfig.dox: /home/exchizz/ROS/Projects/UAV/mavlink/devel/include/mavlink/p_adjustConfig.h

/home/exchizz/ROS/Projects/UAV/mavlink/devel/share/mavlink/docs/p_adjustConfig-usage.dox: /home/exchizz/ROS/Projects/UAV/mavlink/devel/include/mavlink/p_adjustConfig.h

/home/exchizz/ROS/Projects/UAV/mavlink/devel/lib/python2.7/dist-packages/mavlink/cfg/p_adjustConfig.py: /home/exchizz/ROS/Projects/UAV/mavlink/devel/include/mavlink/p_adjustConfig.h

/home/exchizz/ROS/Projects/UAV/mavlink/devel/share/mavlink/docs/p_adjustConfig.wikidoc: /home/exchizz/ROS/Projects/UAV/mavlink/devel/include/mavlink/p_adjustConfig.h

mavlink_gencfg: CMakeFiles/mavlink_gencfg
mavlink_gencfg: /home/exchizz/ROS/Projects/UAV/mavlink/devel/include/mavlink/p_adjustConfig.h
mavlink_gencfg: /home/exchizz/ROS/Projects/UAV/mavlink/devel/share/mavlink/docs/p_adjustConfig.dox
mavlink_gencfg: /home/exchizz/ROS/Projects/UAV/mavlink/devel/share/mavlink/docs/p_adjustConfig-usage.dox
mavlink_gencfg: /home/exchizz/ROS/Projects/UAV/mavlink/devel/lib/python2.7/dist-packages/mavlink/cfg/p_adjustConfig.py
mavlink_gencfg: /home/exchizz/ROS/Projects/UAV/mavlink/devel/share/mavlink/docs/p_adjustConfig.wikidoc
mavlink_gencfg: CMakeFiles/mavlink_gencfg.dir/build.make
.PHONY : mavlink_gencfg

# Rule to build all files generated by this target.
CMakeFiles/mavlink_gencfg.dir/build: mavlink_gencfg
.PHONY : CMakeFiles/mavlink_gencfg.dir/build

CMakeFiles/mavlink_gencfg.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mavlink_gencfg.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mavlink_gencfg.dir/clean

CMakeFiles/mavlink_gencfg.dir/depend:
	cd /home/exchizz/ROS/Projects/UAV/mavlink/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/exchizz/ROS/Projects/UAV/mavlink/src /home/exchizz/ROS/Projects/UAV/mavlink/src /home/exchizz/ROS/Projects/UAV/mavlink/build /home/exchizz/ROS/Projects/UAV/mavlink/build /home/exchizz/ROS/Projects/UAV/mavlink/build/CMakeFiles/mavlink_gencfg.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mavlink_gencfg.dir/depend


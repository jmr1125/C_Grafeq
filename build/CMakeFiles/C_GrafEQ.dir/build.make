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
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/jiang/desktop/C_GrafEq

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/jiang/Desktop/C_GrafEq/build

# Include any dependencies generated for this target.
include CMakeFiles/C_GrafEQ.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/C_GrafEQ.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/C_GrafEQ.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/C_GrafEQ.dir/flags.make

CMakeFiles/C_GrafEQ.dir/main.cpp.o: CMakeFiles/C_GrafEQ.dir/flags.make
CMakeFiles/C_GrafEQ.dir/main.cpp.o: /Users/jiang/desktop/C_GrafEq/main.cpp
CMakeFiles/C_GrafEQ.dir/main.cpp.o: CMakeFiles/C_GrafEQ.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/jiang/Desktop/C_GrafEq/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/C_GrafEQ.dir/main.cpp.o"
	/opt/homebrew/opt/llvm/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/C_GrafEQ.dir/main.cpp.o -MF CMakeFiles/C_GrafEQ.dir/main.cpp.o.d -o CMakeFiles/C_GrafEQ.dir/main.cpp.o -c /Users/jiang/desktop/C_GrafEq/main.cpp

CMakeFiles/C_GrafEQ.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/C_GrafEQ.dir/main.cpp.i"
	/opt/homebrew/opt/llvm/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/jiang/desktop/C_GrafEq/main.cpp > CMakeFiles/C_GrafEQ.dir/main.cpp.i

CMakeFiles/C_GrafEQ.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/C_GrafEQ.dir/main.cpp.s"
	/opt/homebrew/opt/llvm/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/jiang/desktop/C_GrafEq/main.cpp -o CMakeFiles/C_GrafEQ.dir/main.cpp.s

CMakeFiles/C_GrafEQ.dir/eval.cpp.o: CMakeFiles/C_GrafEQ.dir/flags.make
CMakeFiles/C_GrafEQ.dir/eval.cpp.o: /Users/jiang/desktop/C_GrafEq/eval.cpp
CMakeFiles/C_GrafEQ.dir/eval.cpp.o: CMakeFiles/C_GrafEQ.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/jiang/Desktop/C_GrafEq/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/C_GrafEQ.dir/eval.cpp.o"
	/opt/homebrew/opt/llvm/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/C_GrafEQ.dir/eval.cpp.o -MF CMakeFiles/C_GrafEQ.dir/eval.cpp.o.d -o CMakeFiles/C_GrafEQ.dir/eval.cpp.o -c /Users/jiang/desktop/C_GrafEq/eval.cpp

CMakeFiles/C_GrafEQ.dir/eval.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/C_GrafEQ.dir/eval.cpp.i"
	/opt/homebrew/opt/llvm/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/jiang/desktop/C_GrafEq/eval.cpp > CMakeFiles/C_GrafEQ.dir/eval.cpp.i

CMakeFiles/C_GrafEQ.dir/eval.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/C_GrafEQ.dir/eval.cpp.s"
	/opt/homebrew/opt/llvm/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/jiang/desktop/C_GrafEq/eval.cpp -o CMakeFiles/C_GrafEQ.dir/eval.cpp.s

# Object files for target C_GrafEQ
C_GrafEQ_OBJECTS = \
"CMakeFiles/C_GrafEQ.dir/main.cpp.o" \
"CMakeFiles/C_GrafEQ.dir/eval.cpp.o"

# External object files for target C_GrafEQ
C_GrafEQ_EXTERNAL_OBJECTS =

C_GrafEQ: CMakeFiles/C_GrafEQ.dir/main.cpp.o
C_GrafEQ: CMakeFiles/C_GrafEQ.dir/eval.cpp.o
C_GrafEQ: CMakeFiles/C_GrafEQ.dir/build.make
C_GrafEQ: /Users/jiang/radioconda/lib/libglfw.3.4.dylib
C_GrafEQ: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX15.2.sdk/System/Library/Frameworks/OpenGL.framework
C_GrafEQ: /opt/homebrew/lib/libGLEW.2.2.0.dylib
C_GrafEQ: CMakeFiles/C_GrafEQ.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/jiang/Desktop/C_GrafEq/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable C_GrafEQ"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/C_GrafEQ.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/C_GrafEQ.dir/build: C_GrafEQ
.PHONY : CMakeFiles/C_GrafEQ.dir/build

CMakeFiles/C_GrafEQ.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/C_GrafEQ.dir/cmake_clean.cmake
.PHONY : CMakeFiles/C_GrafEQ.dir/clean

CMakeFiles/C_GrafEQ.dir/depend:
	cd /Users/jiang/Desktop/C_GrafEq/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jiang/desktop/C_GrafEq /Users/jiang/desktop/C_GrafEq /Users/jiang/Desktop/C_GrafEq/build /Users/jiang/Desktop/C_GrafEq/build /Users/jiang/Desktop/C_GrafEq/build/CMakeFiles/C_GrafEQ.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/C_GrafEQ.dir/depend


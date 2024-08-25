cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(C_GrafEQ VERSION 1.0)

find_package(glfw3 3.4 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

#  if(APPLE)
#    execute_process( COMMAND brew --prefix libomp
#      OUTPUT_VARIABLE HOMEBREW_LIBOMP_PREFIX
#      OUTPUT_STRIP_TRAILING_WHITESPACE)
#    set(OpenMP_C_FLAGS
#      "-Xpreprocessor -fopenmp -I${HOMEBREW_LIBOMP_PREFIX}/include/")
#    set(OpenMP_CXX_FLAGS
#      "-Xpreprocessor -fopenmp -I${HOMEBREW_LIBOMP_PREFIX}/include/")
#    set(OpenMP_C_LIB_NAMES omp)
#    set(OpenMP_CXX_LIB_NAMES omp)
#    set(OpenMP_omp_LIBRARY ${HOMEBREW_LIBOMP_PREFIX}/lib/libomp.dylib)
#    set(${OpenMP_C_INCLUDE_DIR} "${HOMEBREW_LIBOMP_PREFIX}/include")
#    set(${OpenMP_CXX_INCLUDE_DIR} "${HOMEBREW_LIBOMP_PREFIX}/include")

    # set(CMAKE_MACOSX_RPATH 1)
#  endif()

#  find_package(OpenMP REQUIRED)
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

#include_directories(${OpenCL_INCLUDE_DIRS})
#  link_libraries(${OpenCL_LIBRARY})


add_executable(${PROJECT_NAME} main.cpp eval.cpp)
target_link_libraries(${PROJECT_NAME} glfw OpenGL::GL GLEW::GLEW)
add_executable(testeval testeval.cpp eval.cpp)
add_executable(genExpr genexpr.cpp)
#target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)

#Require CMake v3.25.2
cmake_minimum_required(VERSION 3.25.2)

#Create project
project(POLAR_PRIMES LANGUAGES CXX C)

#Use the FetchContent Module
include(FetchContent)

#Set GLFW options
set(BUILD_SHARED_LIBS OFF)
set(GLFW_LIBRARY_TYPE "STATIC")
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)

#Get GLFW and GLM
FetchContent_Declare(
   glfw
   GIT_REPOSITORY https://github.com/glfw/glfw.git
   GIT_TAG 3.4
)
FetchContent_Declare(
   glm
   GIT_REPOSITORY https://github.com/g-truc/glm.git
   GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glfw glm)

#Use c 17 and c++ 20
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Set c++ compiler flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
   set(CMAKE_CXX_FLAGS "-Wall -Wextra")
   set(CMAKE_CXX_FLAGS_DEBUG "-g")
   set(CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
   set(CMAKE_CXX_FLAGS "")
   set(CMAKE_CXX_FLAGS_DEBUG "/g")
   set(CMAKE_CXX_FLAGS_RELEASE "/O2")
endif()

#Use all cpp files in the src directory
file(GLOB_RECURSE SOURCES src/**.cpp)
add_executable(POLAR_PRIMES ${SOURCES} deps/glad/src/gl.c)

#Incude GLAD, STB image and FreeType in the build
target_include_directories(POLAR_PRIMES PRIVATE deps/glad/include)

#Use static libraries on windows
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
   target_link_libraries(POLAR_PRIMES -static)
endif()

#Link GLFW, GLM and FreeType
target_link_libraries(POLAR_PRIMES glfw glm)

#Link dwmapi on Windows
if(WIN32)
   target_link_libraries(POLAR_PRIMES dwmapi)
endif()

#Name the output executables
set_target_properties(POLAR_PRIMES PROPERTIES OUTPUT_NAME "Polar-Primes")

#Copy the assets
add_custom_target(COPPY_ASSETS ALL
   COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_SOURCE_DIR}/assets
      ${CMAKE_BINARY_DIR}/assets
   COMMENT "Copying resources into binary directory")

add_dependencies(POLAR_PRIMES COPPY_ASSETS)

install(TARGETS POLAR_PRIMES)
install(DIRECTORY assets TYPE "BIN")
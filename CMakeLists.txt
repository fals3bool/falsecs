cmake_minimum_required(VERSION 3.10)
project(falsecs C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples)
set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE RELEASE)

# Adding Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
FetchContent_Declare(
  raylib
  GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
  GIT_TAG "master"
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(raylib)

# Build falsecs examples option
option(FALSECS_BUILD_EXAMPLES "Build falsecs examples" ON)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
set(FALSECS_HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/")

add_library(${PROJECT_NAME} STATIC ${SRC})
target_include_directories(${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${FALSECS_HEADERS}>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME} raylib)

# Examples
if(FALSECS_BUILD_EXAMPLES)
  add_executable(registry examples/registry.c)
  target_link_libraries(registry PRIVATE ${PROJECT_NAME})
  target_include_directories(registry PRIVATE ${FALSECS_HEADERS})

  add_executable(default_components examples/default_components.c)
  target_link_libraries(default_components PRIVATE ${PROJECT_NAME})
  target_include_directories(default_components PRIVATE ${FALSECS_HEADERS})

  add_executable(scene_manager examples/scene_manager.c)
  target_link_libraries(scene_manager PRIVATE ${PROJECT_NAME})
  target_include_directories(scene_manager PRIVATE ${FALSECS_HEADERS})
endif()



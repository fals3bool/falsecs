cmake_minimum_required(VERSION 3.25) #minumum for raylib
project(gearecs C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

#################################
## MULTIPLATFORM CONFIGURATION ##
#################################

if(EMSCRIPTEN)
  set(PLATFORM Web CACHE STRING "" FORCE)
  set(OPENGL_VERSION "ES 3.0" CACHE STRING "" FORCE)
  # set(CMAKE_EXECUTABLE_SUFFIX ".html")
  add_link_options(
    -sUSE_GLFW=3
    -sMIN_WEBGL_VERSION=2
    -sMAX_WEBGL_VERSION=2
    -sFULL_ES3=1
    -sALLOW_MEMORY_GROWTH=1
  )
else()
  set(PLATFORM Desktop CACHE STRING "" FORCE)
  set(OPENGL_VERSION "3.3" CACHE STRING "" FORCE)
endif()

if(MSVC)
  add_compile_options(/W4)

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(/Zi /Od)
  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(/O2)
  endif()

else() # GCC / Clang
  add_compile_options(-Wall -Wextra -Wpedantic)

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O2)
  endif()
endif()

##########################
## RAYLIB FETCH CONTENT ##
##########################

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES         OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES            OFF CACHE BOOL "" FORCE)
set(SUPPORT_BUSY_WAIT_LOOP OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_GIF OFF CACHE BOOL "" FORCE)
set(USE_EXTERNAL_GLFW      OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
  GIT_TAG "master"
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(raylib)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
set(INLCUDE "${CMAKE_CURRENT_LIST_DIR}/include")

add_library(${PROJECT_NAME} STATIC ${SRC})
target_include_directories(${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${INLCUDE}>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME} raylib)

######################
## GEARECS EXAMPLES ##
######################

option(GEARECS_BUILD_EXAMPLES "Build gearecs examples" ON)

if(GEARECS_BUILD_EXAMPLES)
  file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS examples/*.c)
  foreach(example_src ${EXAMPLE_SOURCES})
    get_filename_component(name ${example_src} NAME_WE)
    add_executable(${name} ${example_src})
    target_link_libraries(${name} PRIVATE ${PROJECT_NAME})
  endforeach()
endif()



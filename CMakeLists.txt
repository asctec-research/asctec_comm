cmake_minimum_required(VERSION 2.8.0)
project(asctec_comm)

if(NOT MSVC)
add_definitions(-std=c++11)
endif()

file(GLOB_RECURSE INCS_H "include/*.h")
file(GLOB_RECURSE INCS_HPP "include/*.hpp")

#set(CMAKE_BUILD_TYPE Debug)

set(asctec_comm_src_lib
  src/lib/cobs.cpp
  src/lib/datalink.cpp
  src/lib/types.cpp
  src/lib/transport.cpp
  src/lib/helper.cpp
  # add further source files for the library here.
)

if(WIN32)
set(asctec_comm_src_lib ${asctec_comm_src_lib} 
  src/lib/uart_win.cpp)
else(WIN32)
set(asctec_comm_src_lib ${asctec_comm_src_lib} 
  src/lib/uart_unix.cpp)
endif(WIN32)

set(asctec_comm_src_example
  src/example/example.cpp
)

if(CATKIN_DEVEL_PREFIX)
  MESSAGE(STATUS "Building as catkin package.")
  find_package(catkin_simple REQUIRED)
  catkin_simple(ALL_DEPS_REQUIRED)
  cs_add_library(${PROJECT_NAME} ${asctec_comm_src_lib} ${INCS_H} ${INCS_HPP})
  cs_add_executable(${PROJECT_NAME}_example ${asctec_comm_src_example} ${INCS_H} ${INCS_HPP})
  target_link_libraries(${PROJECT_NAME}_example ${PROJECT_NAME} pthread)
  cs_install()
  cs_export()
else(CATKIN_DEVEL_PREFIX)
  MESSAGE(STATUS "Building as native cmake package.")
  include_directories(include)
  include_directories(deps/asctec_uav_msgs/include)
  add_library(${PROJECT_NAME} ${asctec_comm_src_lib} ${INCS_H} ${INCS_HPP})
  add_executable(${PROJECT_NAME}_example ${asctec_comm_src_example} ${INCS_H} ${INCS_HPP})
  add_dependencies(${PROJECT_NAME}_example ${PROJECT_NAME})
  if(UNIX)
    target_link_libraries(${PROJECT_NAME}_example ${PROJECT_NAME} pthread)
  else()
    target_link_libraries(${PROJECT_NAME}_example ${PROJECT_NAME})
  endif()

  # TODO: install, pkg-config?
endif(CATKIN_DEVEL_PREFIX)

add_subdirectory(src/test)

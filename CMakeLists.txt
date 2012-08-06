# (almost) everything you might want to know about the CMakeLists syntax 
# http://www.cmake.org/cmake/help/cmake2.6docs.html

cmake_minimum_required (VERSION 2.8)
project(ScrabbleEye) #uzywane przykladowo w nazwie projektu VS

find_package(OpenCV REQUIRED)
include_directories("${PROJECT_SOURCE_DIR}")
#include_directories("${OpenCV_INCLUDE_DIR}")


set(EXECUTABLE_NAME "se")
add_executable(${EXECUTABLE_NAME} ScrabbleEye.cxx) 
#add_executable(${EXECUTABLE_NAME} ScrabbleEye.cpp Constants.cpp) 

target_link_libraries(${EXECUTABLE_NAME} ${OpenCV_LIBS})


##########
## Events
##########
#  add_subdirectory(events)
#  target_link_libraries(${EXECUTABLE_NAME} events)
#
##executable
#install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
##resources
#file(COPY ${PROJECT_SOURCE_DIR}/resources DESTINATION ${PROJECT_BINARY_DIR})
#install(DIRECTORY ${PROJECT_SOURCE_DIR}/resources DESTINATION ${CMAKE_INSTALL_PREFIX})



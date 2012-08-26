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

##############
# Google Test
##############
  ENABLE_TESTING()
  set(GOOGLE_MOCK_RELATIVE google_mock)
  add_subdirectory(${GOOGLE_MOCK_RELATIVE})
  include_directories(${PROJECT_SOURCE_DIR}/${GOOGLE_MOCK_RELATIVE}/include)
  include_directories(${PROJECT_SOURCE_DIR}/${GOOGLE_MOCK_RELATIVE}/gtest/include)

#########
# Utils
#########
  add_subdirectory(utils)
  target_link_libraries(${EXECUTABLE_NAME} utils)



##########
## Install
##########
#executable
install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
#resources
file(COPY ${PROJECT_SOURCE_DIR}/res DESTINATION ${PROJECT_BINARY_DIR})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/res DESTINATION ${CMAKE_INSTALL_PREFIX})

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

########
# TESTS
########

  file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  
  
  # if there is anythink we need for th tests, we put it in the "extras" directory 
  # depending on how we run the tests, it might be needed in different directories 
  file(COPY ${PROJECT_SOURCE_DIR}/tests/extras DESTINATION ${PROJECT_BINARY_DIR}) 
  file(COPY ${PROJECT_SOURCE_DIR}/tests/extras DESTINATION ${PROJECT_BINARY_DIR}/tests) 

  ## this is the list of unit tests in the test/ directory the filename searched for is created by appending
  ## "_test.cxx" to the test name make sure to add all the new tests here

  set(TESTNAMES 
    sample
    lines
    #sth_else
    )
 
  foreach(testname ${TESTNAMES})
          add_executable(tests/${testname}.test tests/${testname}_test.cxx)
          target_link_libraries(tests/${testname}.test utils ${OpenCV_LIBS} gmock_main)
          add_test(${testname} tests/${testname}.test)
  endforeach(testname)

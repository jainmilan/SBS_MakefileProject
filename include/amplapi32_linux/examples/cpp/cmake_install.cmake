# Install script for directory: /home/milan/Workspace/Milan/Research/Project/Waterloo/Simulator/AMPL/amplide.linux32/amplapi/examples/cpp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/cpp" TYPE FILE FILES "/home/milan/Workspace/Milan/Research/Project/Waterloo/Simulator/AMPL/amplide.linux32/amplapi/examples/cpp/dataframeexample.cc")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/cpp" TYPE FILE FILES "/home/milan/Workspace/Milan/Research/Project/Waterloo/Simulator/AMPL/amplide.linux32/amplapi/examples/cpp/dietmodel.cc")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/cpp" TYPE FILE FILES "/home/milan/Workspace/Milan/Research/Project/Waterloo/Simulator/AMPL/amplide.linux32/amplapi/examples/cpp/efficientfrontier.cc")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/cpp" TYPE FILE FILES "/home/milan/Workspace/Milan/Research/Project/Waterloo/Simulator/AMPL/amplide.linux32/amplapi/examples/cpp/firstexample.cc")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/cpp" TYPE FILE FILES "/home/milan/Workspace/Milan/Research/Project/Waterloo/Simulator/AMPL/amplide.linux32/amplapi/examples/cpp/multidimensionalexample.cc")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/cpp" TYPE FILE FILES "/home/milan/Workspace/Milan/Research/Project/Waterloo/Simulator/AMPL/amplide.linux32/amplapi/examples/cpp/optionsexample.cc")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/cpp" TYPE FILE FILES "/home/milan/Workspace/Milan/Research/Project/Waterloo/Simulator/AMPL/amplide.linux32/amplapi/examples/cpp/asyncexample.cc")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/cpp" TYPE FILE FILES "/home/milan/Workspace/Milan/Research/Project/Waterloo/Simulator/AMPL/amplide.linux32/amplapi/examples/cpp/trackingmodel.cc")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/milan/Workspace/Milan/Research/Project/Waterloo/Simulator/AMPL/amplide.linux32/amplapi/examples/cpp/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")

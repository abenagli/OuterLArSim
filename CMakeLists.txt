cmake_minimum_required(VERSION 2.6 FATAL_ERROR)


############ Make your changes here ############ 
 
set(mytarget    OuterLArSim      )  # equivalent to "name := myexample" in G4 GNUmakefiles
set(useROOT     true             )  # use true or false (or comment to set to false)
set(myROOTdict  true             )  # use true or false (or comment to set to false)
#set(myROOTclass TMyRootEvent.hpp)  # comment if none (please see http://root.cern.ch/phpBB3//viewtopic.php?t=6172)

############ What's below should not need to be changed ############ 

# http://www.cmake.org/cmake/help/cmake_tutorial.html
# http://www.cmake.org/cmake/help/cmake2.6docs.html
# http://www.cmake.org/Wiki/CMake_FAQ

set(mymain ${mytarget}.cc)
set(myexe  ${mytarget}   )
set(myproj ${mytarget}_  )  # any name as long as it is different from myexe

project(${myproj})

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
 
#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
if(useROOT)
        if(myROOTclass)
                EXECUTE_PROCESS(COMMAND rootcint -f dict.cc -c ${PROJECT_SOURCE_DIR}/include/${myROOTclass} ${PROJECT_SOURCE_DIR}/include/LinkDef.h)
        endif(myROOTclass)
        if(myROOTdict)
                EXECUTE_PROCESS(COMMAND rootcint -f dict.cc -c ${PROJECT_SOURCE_DIR}/include/STLDict.hh ${PROJECT_SOURCE_DIR}/include/LinkDef.h)
        endif(myROOTdict)
        EXECUTE_PROCESS(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")
endif(useROOT) 
 
#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(${myexe} ${mymain} ${sources} ${headers})
if(useROOT)
        EXECUTE_PROCESS(COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(myROOTclass OR myROOTdict)
                set(CMAKE_EXE_LINKER_FLAGS "${ROOT_LD_FLAGS} dict.cc")
        else(myROOTclass OR myROOTdict)
                set(CMAKE_EXE_LINKER_FLAGS ${ROOT_LD_FLAGS})
        endif(myROOTclass OR myROOTdict)
endif(useROOT)
target_link_libraries(${myexe} ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this example standalone
#
add_custom_target(${myproj} DEPENDS ${myexe})

#----------------------------------------------------------------------------
# Install the executable to 'bin' folder of CMAKE_INSTALL_PREFIX
#
install(TARGETS ${myexe} DESTINATION bin)

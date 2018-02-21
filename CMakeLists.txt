# CMakeLists.txt for event package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Unfold)

set(CMAKE_CXX_COMPILER "gcc")

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
if(EXISTS $ENV{ROOTSYS/cmake/ROOTConfig.cmake})
  list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
else()
  list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/cmake)
endif()

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Tree Net TString)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/interface)
add_definitions(${ROOT_CXX_FLAGS})

ROOT_GENERATE_DICTIONARY(G__MCSelector ../src/MCSelector.h LINKDEF ../interface/LinkDef.h)

#---Create a shared library with geneated dictionary
add_library(MCSelector SHARED ${PROJECT_SOURCE_DIR}/src/MCSelector.C G__MCSelector.cxx)
target_link_libraries(MCSelector ${ROOT_LIBRARIES})




set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)

###############################################################################
## file globbing ##############################################################
###############################################################################

# these instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables 
# `sources` and `data`
file(GLOB_RECURSE sources      src/*.cpp )
file(GLOB_RECURSE headerfiles  interface/*.hpp interface/*.hpp)


# add the data to the target, so it becomes visible in some IDE
add_executable(Unfold ${sources} ${data})
###############################################################################
## dependencies ###############################################################
###############################################################################

# this defines the variables Boost_LIBRARIES that contain all library names
# that we need to link to
find_package(Boost 1.36.0 COMPONENTS filesystem system REQUIRED)

target_link_libraries(Unfold PUBLIC
  ${Boost_LIBRARIES}
  ${ROOT_LIBRARIES}
  MCSelector

  # here you can add any library dependencies
)

#---Set Link and include directories--------------------------------------------------------------
include_directories(${ROOT_INCLUDE_DIRS})
link_directories(${ROOT_LIBRARY_DIR})

#---Set Flags-------------------------------------------------------------------------------------
add_definitions(${ROOT_DEFINITIONS})
set(CMAKE_CXX_FLAGS " -std=c++11 ${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS} -Wextra -g  -std=c++17 -lstdc++ -lTreePlayer -lProof -Iboost_root/boost -Lboost_root/boost/stage/lib/ -l boost_program_options" )


macro(install_symlink filepath sympath)
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${filepath} ${sympath})")
    install(CODE "message(\"-- Created symlink: ${sympath} -> ${filepath}\")")
endmacro(install_symlink)
###############################################################################
## packaging ##################################################################
###############################################################################

# all install commands get the same destination. this allows us to use paths
# relative to the executable.
install(TARGETS Unfold DESTINATION Unfold_destination)
# this is basically a repeat of the file copy instruction that copies the
# resources in the build directory, but here we tell cmake that we want it
# in the package
install(DIRECTORY resources DESTINATION Unfold_destination)


# now comes everything we need, to create a package
# there are a lot more variables you can set, and some
# you need to set for some package types, but we want to
# be minimal here
set(CPACK_PACKAGE_NAME "MyUnfold")
set(CPACK_PACKAGE_VERSION "1.0.0")

# we don't want to split our program up into several things
set(CPACK_MONOLITHIC_INSTALL 1)





# This must be last
include(CPack)
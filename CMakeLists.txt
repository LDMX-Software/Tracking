
# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name and version.
# TODO: The version should be synced up with ldmx-sw.
project(Tracking VERSION 2.1.0
                 DESCRIPTION "Module containing the sim and recon code for the trackers."
                 LANGUAGES CXX
)

# Search for ROOT and add all targets.
find_package(ROOT CONFIG REQUIRED)

# Allow building of event specific classes only.  The most common use case is 
# needing to build the event classes to allow building of the event bus and ROOT
# dictionary prior building the rest of the modules in a package. 
option(BUILD_EVENT_ONLY "Build the event library." ON)
if(BUILD_EVENT_ONLY)

    # Register all of the classes with the event bus.
    register_event_object(module_path "Tracking/Event" 
                          namespace "ldmx" 
                          class "RawSiStripHit" )

    register_event_object(module_path "Tracking/Event" 
                          namespace "ldmx" 
                          class "Track" type "collection")

    register_event_object(module_path "Tracking/Event" 
                          namespace "ldmx" 
                          class "TruthTrack" type "collection")

    register_event_object(module_path "Tracking/Event"
                          namespace "ldmx"
                          class "Measurement" type "collection")

    # Get all of the event sources that will be included in this library
    file(GLOB EVENT_SRC_FILES CONFIGURE_DEPENDS 
        ${PROJECT_SOURCE_DIR}/src/Tracking/Event/[a-zA-z]*.cxx)

    # Generate the files needed to build the event classes.
    setup_library(module Tracking
                  name Event
                  dependencies ROOT::Core
                  SimCore::Event
                  sources ${EVENT_SRC_FILES}
                  register_target)

     return()
endif()

# since ACTS changes frequently enough, we build it as a submodule of Tracking
#   here we 'set' some CMake variables that we would have passed to 'cmake' on
#   the command line with '-D' if we were building ACTS directly.
#set(ACTS_BUILD_PLUGIN_DD4HEP ON) # representing geometry with DD4hep right now
#set(ACTS_BUILD_PLUGIN_TGEO ON) # representing geometry with DD4hep right now
set(ACTS_BUILD_PLUGIN_IDENTIFICATION ON) #This turns on the identiers
set(ACTS_BUILD_EXAMPLES OFF)     # don't waste time building examples
set(CMAKE_CXX_STANDARD 17)       # set C++ standard we use within ldmx-sw
add_subdirectory(acts)           # now build acts
#add_subdirectory(acts-dd4hep)
# luckily for us, adding ACTS as a subdirectory produces the same CMake targets
#   as find_package, so we don't need to do anything else from here on out

find_package(Eigen3 CONFIG REQUIRED)

setup_geant4_target()


file(GLOB SRC_FILES CONFIGURE_DEPENDS 
  ${PROJECT_SOURCE_DIR}/src/Tracking/Sim/[a-zA-z]*.cxx
  ${PROJECT_SOURCE_DIR}/src/Tracking/Reco/[a-zA-z]*.cxx
  ${PROJECT_SOURCE_DIR}/src/Tracking/dqm/[a-zA-z]*.cxx
  ${PROJECT_SOURCE_DIR}/src/Tracking/geo/[a-zA-z]*.cxx
)

setup_library(module Tracking
              dependencies Framework::Configure 
                           Framework::Framework 
                           ActsCore
                           ActsPluginIdentification
                           Geant4::Interface
                           ROOT::Physics
                           Tracking::Event
              sources ${SRC_FILES})


#include_directories(${PROJECT_SOURCE_DIR}/include/Tracking/Reco/)

setup_python(package_name ${PYTHON_PACKAGE_NAME}/Tracking)

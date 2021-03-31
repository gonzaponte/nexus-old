#----------------------------------------------------------------------------
 # Setup the project
 cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

 project(g4bugP)

 find_package(Geant4 REQUIRED ui_all vis_all)
 find_package(Geant4 REQUIRED)
 include(${Geant4_USE_FILE})

 add_subdirectory(source)

 #----------------------------------------------------------------------------
 # Add the executable, and link it to the Geant4 libraries
 #
 add_executable(g4bug g4bug.cc ${sources} ${headers})
 target_link_libraries(g4bug ${Geant4_LIBRARIES})

 add_custom_target(g4bugP DEPENDS g4bug)

 #----------------------------------------------------------------------------
 # Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
 #
 install(TARGETS g4bug DESTINATION bin)

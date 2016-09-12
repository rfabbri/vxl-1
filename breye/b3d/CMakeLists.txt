# breyes/b3d/CMakeLists.txt

include( ${LEMS_CMAKE_DIR}/Modules/UseBoil.cmake )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/basic )

#if (HAS_BGUI3D)

set(b3d_sources

   main.cxx
   Shock3d_Main_Menu.cxx  Shock3d_Main_Menu.h
   Shock3d_Menu.cxx       Shock3d_Menu.h
   Shock3d_Manager.cxx    Shock3d_Manager.h
   Shock3d_Loader.cxx   Shock3d_Loader.h
)

add_executable(b3d ${b3d_sources})

target_link_libraries(b3d boil shock3d vsol bsol shock3d bgui3d)

#endif (HAS_BGUI3D)

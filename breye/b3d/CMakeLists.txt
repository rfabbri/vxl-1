# breyes/b3d/CMakeLists.txt

INCLUDE( ${LEMS_CMAKE_DIR}/Modules/UseBoil.cmake )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

#IF (HAS_BGUI3D)

SET(b3d_sources

   main.cxx
   Shock3d_Main_Menu.cxx  Shock3d_Main_Menu.h
   Shock3d_Menu.cxx       Shock3d_Menu.h
   Shock3d_Manager.cxx    Shock3d_Manager.h
   Shock3d_Loader.cxx   Shock3d_Loader.h
)

ADD_EXECUTABLE(b3d ${b3d_sources})

TARGET_LINK_LIBRARIES(b3d boil shock3d vsol bsol shock3d bgui3d)

#ENDIF (HAS_BGUI3D)

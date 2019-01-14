# dbmsh3dapp Mesh Visualization and Processsing Application
# contrib/dbmsh3dapp/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#include( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

if (HAS_BGUI3D)

set(dbmsh3dapp_sources

   dbmsh3dapp_main.cxx

)


add_executable( dbmsh3dapp ${dbmsh3dapp_sources} )

target_link_libraries( dbmsh3dapp dbmsh3d dbmsh3d_algo 
                       dbmsh3d_pro dbmsh3d_vis dbmsh3d_cmdproc
                       dbmsh3dr dbmsh3dr_pro dbmsh3dr_vis )

#Add the Win32 Application where SoWin is used.
if (WIN32)

set (USE_SOWIN "YES")
include( ${LEMS_CMAKE_DIR}/Modules/UseSoWin.cmake )
include_directories( ${SOWIN_INCLUDE_DIR} )

set(dbmsh3dappw_sources     
   dbmsh3dappw_main.cxx
)

add_executable( dbmsh3dappw ${dbmsh3dappw_sources} )

# add the required libraries into this list
target_link_libraries( dbmsh3dappw 
                       dbmsh3d dbmsh3d_algo dbmsh3d_pro dbmsh3d_vis dbmsh3d_cmdproc
                       dbmsh3dr dbmsh3dr_pro dbmsh3dr_vis               
                       ${COIN_LIBRARY} )
endif (WIN32)

endif (HAS_BGUI3D)

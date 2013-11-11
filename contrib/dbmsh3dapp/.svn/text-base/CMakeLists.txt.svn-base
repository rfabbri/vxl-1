# dbmsh3dapp Mesh Visualization and Processsing Application
# contrib/dbmsh3dapp/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#INCLUDE( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

IF (HAS_BGUI3D)

SET(dbmsh3dapp_sources

   dbmsh3dapp_main.cxx

)


ADD_EXECUTABLE( dbmsh3dapp ${dbmsh3dapp_sources} )

TARGET_LINK_LIBRARIES( dbmsh3dapp dbmsh3d dbmsh3d_algo 
                       dbmsh3d_pro dbmsh3d_vis dbmsh3d_cmdproc
                       dbmsh3dr dbmsh3dr_pro dbmsh3dr_vis )

#Add the Win32 Application where SoWin is used.
IF (WIN32)

SET (USE_SOWIN "YES")
INCLUDE( ${LEMS_CMAKE_DIR}/Modules/UseSoWin.cmake )
INCLUDE_DIRECTORIES( ${SOWIN_INCLUDE_DIR} )

SET(dbmsh3dappw_sources     
   dbmsh3dappw_main.cxx
)

ADD_EXECUTABLE( dbmsh3dappw ${dbmsh3dappw_sources} )

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbmsh3dappw 
                       dbmsh3d dbmsh3d_algo dbmsh3d_pro dbmsh3d_vis dbmsh3d_cmdproc
                       dbmsh3dr dbmsh3dr_pro dbmsh3dr_vis               
                       ${COIN_LIBRARY} )
ENDIF (WIN32)

ENDIF (HAS_BGUI3D)

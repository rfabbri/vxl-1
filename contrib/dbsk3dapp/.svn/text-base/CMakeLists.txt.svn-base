# shock3d applications
# contrib/dbsk3dapp/CMakeLists.txt


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )


# This is to produce a VOX executable.

#Need this because dborl_utilities.h use bpro1
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(dbsk3dappvox_sources
  dborl_shock3d_match_params.h
  dborl_shock3d_match_params_sptr.h
  dbsk3dapp_vox_main.cxx
  vbl_smart_ptr+dborl_shock3d_match_params-.cxx
)


ADD_EXECUTABLE( dbsk3dappvox ${dbsk3dappvox_sources} )

TARGET_LINK_LIBRARIES( dbsk3dappvox 
                       dborl_algo
                       dbmsh3d dbmsh3d_algo dbmsh3d_pro 
                       dbmsh3dr dbmsh3dr_pro 
                       dbgdt3d dbgdt3d_pro 
                       dbsk3d dbsk3d_algo dbsk3d_pro
                       dbasn
                       dbsk3dr dbsk3dr_pro)


INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#INCLUDE( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

# copy the qvoronoi.exe file to the running directory
#CONFIGURE_FILE(
#  ${lems_SOURCE_DIR}/contrib/dbsk3dappw/qvoronoi.exe
#  ${lems_BINARY_DIR}/contrib/dbsk3dappw/qvoronoi.exe COPYONLY
#)

IF (HAS_BGUI3D)

SET(dbsk3dapp_sources

   dbsk3dapp_main.cxx

   dbsk3dapp_view_manager.cxx  dbsk3dapp_view_manager.h
   dbsk3dapp_manager.h       dbsk3dapp_manager.cxx
   dbsk3dapp_menu.h          dbsk3dapp_menu.cxx

   dbsk3dapp_menu_handler.cxx

   dbsk3dapp_action.h        dbsk3dapp_action.cxx
)


ADD_EXECUTABLE( dbsk3dapp ${dbsk3dapp_sources} )

TARGET_LINK_LIBRARIES( dbsk3dapp 
                       dbmsh3d dbmsh3d_algo dbmsh3d_pro dbmsh3d_vis dbmsh3d_cmdproc
                       dbmsh3dr dbmsh3dr_pro dbmsh3dr_vis 
                       dbgdt3d dbgdt3d_pro dbgdt3d_vis dbgdt3d_cmdproc
                       dbsk3d dbsk3d_algo dbsk3d_flowsheet dbsk3d_vis dbsk3d_cmdproc
                       dbasn dbsk3dr dbsk3dr_pro dbsk3dr_vis )


IF (WIN32)

SET (USE_SOWIN "YES")
INCLUDE( ${LEMS_CMAKE_DIR}/Modules/UseSoWin.cmake )
INCLUDE_DIRECTORIES( ${SOWIN_INCLUDE_DIR} )

SET(dbsk3dappw_sources
    dbsk3dappw_main.cxx 
)

ADD_EXECUTABLE( dbsk3dappw ${dbsk3dappw_sources} )

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbsk3dappw 
                       dbmsh3d dbmsh3d_algo  dbmsh3d_vis dbmsh3d_pro
                       dbmsh3dr dbmsh3dr_pro dbmsh3dr_vis dbmsh3d_cmdproc
                       dbgdt3d dbgdt3d_pro dbgdt3d_vis dbgdt3d_cmdproc
                       dbsk3d dbsk3d_algo dbsk3d_pro dbsk3d_vis dbsk3d_cmdproc
                       dbsk3d_flowsheet
                       dbasn dbsk3dr dbsk3dr_pro dbsk3dr_vis 
                       ${COIN_LIBRARY} )    

ENDIF (WIN32)

ENDIF (HAS_BGUI3D)


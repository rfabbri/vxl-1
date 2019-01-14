# shock3d applications
# contrib/dbsk3dapp/CMakeLists.txt


include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )


# This is to produce a VOX executable.

#Need this because dborl_utilities.h use bpro1
include_directories( ${LEMS_SOURCE_DIR}/breye1 )

set(dbsk3dappvox_sources
  dborl_shock3d_match_params.h
  dborl_shock3d_match_params_sptr.h
  dbsk3dapp_vox_main.cxx
  vbl_smart_ptr+dborl_shock3d_match_params-.cxx
)


add_executable( dbsk3dappvox ${dbsk3dappvox_sources} )

target_link_libraries( dbsk3dappvox 
                       dborl_algo
                       dbmsh3d dbmsh3d_algo dbmsh3d_pro 
                       dbmsh3dr dbmsh3dr_pro 
                       dbgdt3d dbgdt3d_pro 
                       dbsk3d dbsk3d_algo dbsk3d_pro
                       dbasn
                       dbsk3dr dbsk3dr_pro)


include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#include( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

# copy the qvoronoi.exe file to the running directory
#configure_file(
#  ${LEMS_SOURCE_DIR}/contrib/dbsk3dappw/qvoronoi.exe
#  ${LEMS_BINARY_DIR}/contrib/dbsk3dappw/qvoronoi.exe COPYONLY
#)

if (HAS_BGUI3D)

set(dbsk3dapp_sources

   dbsk3dapp_main.cxx

   dbsk3dapp_view_manager.cxx  dbsk3dapp_view_manager.h
   dbsk3dapp_manager.h       dbsk3dapp_manager.cxx
   dbsk3dapp_menu.h          dbsk3dapp_menu.cxx

   dbsk3dapp_menu_handler.cxx

   dbsk3dapp_action.h        dbsk3dapp_action.cxx
)


add_executable( dbsk3dapp ${dbsk3dapp_sources} )

target_link_libraries( dbsk3dapp 
                       dbmsh3d dbmsh3d_algo dbmsh3d_pro dbmsh3d_vis dbmsh3d_cmdproc
                       dbmsh3dr dbmsh3dr_pro dbmsh3dr_vis 
                       dbgdt3d dbgdt3d_pro dbgdt3d_vis dbgdt3d_cmdproc
                       dbsk3d dbsk3d_algo dbsk3d_flowsheet dbsk3d_vis dbsk3d_cmdproc
                       dbasn dbsk3dr dbsk3dr_pro dbsk3dr_vis )


if (WIN32)

set (USE_SOWIN "YES")
include( ${LEMS_CMAKE_DIR}/Modules/UseSoWin.cmake )
include_directories( ${SOWIN_INCLUDE_DIR} )

set(dbsk3dappw_sources
    dbsk3dappw_main.cxx 
)

add_executable( dbsk3dappw ${dbsk3dappw_sources} )

# add the required libraries into this list
target_link_libraries( dbsk3dappw 
                       dbmsh3d dbmsh3d_algo  dbmsh3d_vis dbmsh3d_pro
                       dbmsh3dr dbmsh3dr_pro dbmsh3dr_vis dbmsh3d_cmdproc
                       dbgdt3d dbgdt3d_pro dbgdt3d_vis dbgdt3d_cmdproc
                       dbsk3d dbsk3d_algo dbsk3d_pro dbsk3d_vis dbsk3d_cmdproc
                       dbsk3d_flowsheet
                       dbasn dbsk3dr dbsk3dr_pro dbsk3dr_vis 
                       ${COIN_LIBRARY} )    

endif (WIN32)

endif (HAS_BGUI3D)


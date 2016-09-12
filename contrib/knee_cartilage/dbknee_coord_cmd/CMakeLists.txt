# kneecoord_cmd Developing LEMS Shock 3D Command-Line Application
# lemsvxlsrc/contrib/knee_cartilage/kneecoord_cmd/CMakeLists.txt

if (WIN32)

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#include( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

set (USE_SOWIN "YES")
include( ${LEMS_CMAKE_DIR}/Modules/UseSoWin.cmake )
include_directories( ${SOWIN_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

set(dbknee_coord_cmd_sources
  dbknee_coord_cmd_main.cxx
)

add_executable( dbknee_coord_cmd ${dbknee_coord_cmd_sources} )

# add the required libraries into this list
target_link_libraries( dbknee_coord_cmd
                       dbmsh3d dbmsh3d_algo dbmsh3d_pro dbmsh3d_vis
                       dbmsh3d_gdt dbmsh3d_gdtvis
                       dbmsh3dr dbmsh3dr_pro
                       dbsk3d_vis
                       vnl_algo vbl vbl_io vul vsol vsl bsol dbknee
                       ${COIN_LIBRARY} )


set( dbknee_compute_local_coord_cmd_sources
  dbknee_compute_local_coord_cmd_main.cxx
)

add_executable( dbknee_compute_local_coord_cmd ${dbknee_compute_local_coord_cmd_sources} )

# add the required libraries into this list
target_link_libraries( dbknee_compute_local_coord_cmd
                       dbmsh3d dbmsh3d_algo dbmsh3d_pro dbmsh3d_vis dbsk3d_vis
                       vcl vgl vnl vnl_algo vbl vbl_io vul vsol vsl bsol vtol btol
                       dbknee dbknee_vis
                       ${COIN_LIBRARY} )
endif (WIN32)

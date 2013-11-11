# kneecoord_cmd Developing LEMS Shock 3D Command-Line Application
# lemsvxlsrc/contrib/knee_cartilage/kneecoord_cmd/CMakeLists.txt

IF (WIN32)

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
#INCLUDE( ${VXL_BRL_INCLUDE_DIR}/bmods/UseBGUI3D.cmake )

SET (USE_SOWIN "YES")
INCLUDE( ${LEMS_CMAKE_DIR}/Modules/UseSoWin.cmake )
INCLUDE_DIRECTORIES( ${SOWIN_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

SET(dbknee_coord_cmd_sources
  dbknee_coord_cmd_main.cxx
)

ADD_EXECUTABLE( dbknee_coord_cmd ${dbknee_coord_cmd_sources} )

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbknee_coord_cmd
                       dbmsh3d dbmsh3d_algo dbmsh3d_pro dbmsh3d_vis
                       dbmsh3d_gdt dbmsh3d_gdtvis
                       dbmsh3dr dbmsh3dr_pro
                       dbsk3d_vis
                       vnl_algo vbl vbl_io vul vsol vsl bsol dbknee
                       ${COIN_LIBRARY} )


SET( dbknee_compute_local_coord_cmd_sources
  dbknee_compute_local_coord_cmd_main.cxx
)

ADD_EXECUTABLE( dbknee_compute_local_coord_cmd ${dbknee_compute_local_coord_cmd_sources} )

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbknee_compute_local_coord_cmd
                       dbmsh3d dbmsh3d_algo dbmsh3d_pro dbmsh3d_vis dbsk3d_vis
                       vcl vgl vnl vnl_algo vbl vbl_io vul vsol vsl bsol vtol btol
                       dbknee dbknee_vis
                       ${COIN_LIBRARY} )
ENDIF (WIN32)

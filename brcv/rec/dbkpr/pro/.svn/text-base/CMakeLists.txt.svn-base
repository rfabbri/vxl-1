# This is brcv/seg/dbkpr/pro/CMakeLists.txt
# Brown Detection Processing library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

SET(dbkpr_pro_sources
  dbkpr_bbf_match_process.cxx      dbkpr_bbf_match_process.h
  dbkpr_span_match_process.cxx     dbkpr_span_match_process.h
  dbkpr_reconstruct_process.cxx    dbkpr_reconstruct_process.h
  dbkpr_interp_depth_process.cxx   dbkpr_interp_depth_process.h
  dbkpr_corr3d_storage.cxx         dbkpr_corr3d_storage.h          dbkpr_corr3d_storage_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates dbkpr_pro_sources)

ADD_LIBRARY(dbkpr_pro ${dbkpr_pro_sources})

TARGET_LINK_LIBRARIES(dbkpr_pro dbkpr dbdet_pro dvpgl_pro )


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)


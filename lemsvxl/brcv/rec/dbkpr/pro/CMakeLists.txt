# This is brcv/seg/dbkpr/pro/CMakeLists.txt
# Brown Detection Processing library

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

set(dbkpr_pro_sources
  dbkpr_bbf_match_process.cxx      dbkpr_bbf_match_process.h
  dbkpr_span_match_process.cxx     dbkpr_span_match_process.h
  dbkpr_reconstruct_process.cxx    dbkpr_reconstruct_process.h
  dbkpr_interp_depth_process.cxx   dbkpr_interp_depth_process.h
  dbkpr_corr3d_storage.cxx         dbkpr_corr3d_storage.h          dbkpr_corr3d_storage_sptr.h
)

aux_source_directory(Templates dbkpr_pro_sources)

add_library(dbkpr_pro ${dbkpr_pro_sources})

target_link_libraries(dbkpr_pro dbkpr dbdet_pro dvpgl_pro )


if(BUILD_TESTING)
#  add_subdirectory(tests)
endif(BUILD_TESTING)


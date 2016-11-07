# This is contrib/mleotta/modrec/pro/CMakeLists.txt
# Model Based Recognition Processing library


# include directories into here if needed
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )

set(modrec_pro_sources
   modrec_position_process.cxx             modrec_position_process.h
   modrec_classify_process.cxx             modrec_classify_process.h
   modrec_evaluate_process.cxx             modrec_evaluate_process.h
   modrec_pro_utils.cxx                    modrec_pro_utils.h
   modrec_depthmap_process.cxx             modrec_depthmap_process.h
)

aux_source_directory(Templates modrec_pro_sources)

add_library(modrec_pro ${modrec_pro_sources})

target_link_libraries(modrec_pro modrec dbul bbgm bxml)




# This is contrib/mleotta/modrec/pro/CMakeLists.txt
# Model Based Recognition Processing library


# include directories into here if needed
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

SET(modrec_pro_sources
   modrec_position_process.cxx             modrec_position_process.h
   modrec_classify_process.cxx             modrec_classify_process.h
   modrec_evaluate_process.cxx             modrec_evaluate_process.h
   modrec_pro_utils.cxx                    modrec_pro_utils.h
   modrec_depthmap_process.cxx             modrec_depthmap_process.h
)

AUX_SOURCE_DIRECTORY(Templates modrec_pro_sources)

ADD_LIBRARY(modrec_pro ${modrec_pro_sources})

TARGET_LINK_LIBRARIES(modrec_pro modrec dbul bbgm bxml)




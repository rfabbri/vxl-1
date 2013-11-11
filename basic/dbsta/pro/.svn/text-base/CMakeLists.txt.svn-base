# This is basic/dbsta/pro/CMakeLists.txt
# Brown Statistical Modeling Processing library


# include directories into here if needed
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

SET(dbsta_pro_sources
   dbsta_model_image_process.cxx             dbsta_model_image_process.h
   dbsta_model_image_regions_process.cxx     dbsta_model_image_regions_process.h
)

AUX_SOURCE_DIRECTORY(Templates dbsta_pro_sources)

ADD_LIBRARY(dbsta_pro ${dbsta_pro_sources})

TARGET_LINK_LIBRARIES(dbsta_pro dbsta dbsta_algo)




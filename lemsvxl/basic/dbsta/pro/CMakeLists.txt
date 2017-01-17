# This is basic/dbsta/pro/CMakeLists.txt
# Brown Statistical Modeling Processing library


# include directories into here if needed
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXD_BRLD_INCLUDE_DIR})

set(dbsta_pro_sources
   dbsta_model_image_process.cxx             dbsta_model_image_process.h
   dbsta_model_image_regions_process.cxx     dbsta_model_image_regions_process.h
)

aux_source_directory(Templates dbsta_pro_sources)

add_library(dbsta_pro ${dbsta_pro_sources})

target_link_libraries(dbsta_pro dbsta dbsta_algo)




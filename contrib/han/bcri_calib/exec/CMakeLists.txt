# 
# Brown personal testing area.
include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories(${VXL_GEL_INCLUDE_DIR})
include_directories(${VXL_OXL_INCLUDE_DIR})
include_directories(${VXL_BRL_INCLUDE_DIR})
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)
include_directories(${VXL_BRL_INCLUDE_DIR}/bmvl)
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg)

set(bcri_calib_sources
bcri_calib.cxx  bcri_calib.h bcri_calib_menus.cxx bcri_calib_menus.h
)

aux_source_directory(Templates bcri_calib_sources)
add_library(bcri_calib ${bcri_calib_sources})
target_link_libraries(bcri_calib bgui sdet brip btol bdgl mbl mvl  vdgl vtol vsol vul vbl vnl vcl vil1 vil  vsl )



add_executable (bcri_calib_bin  bcri_calib_man.cxx)
target_link_libraries(bcri_calib_bin bcri_calib )

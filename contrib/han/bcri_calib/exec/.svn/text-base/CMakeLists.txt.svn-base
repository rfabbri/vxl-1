# 
# Brown personal testing area.
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bmvl)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg)

SET(bcri_calib_sources
bcri_calib.cxx  bcri_calib.h bcri_calib_menus.cxx bcri_calib_menus.h
)

AUX_SOURCE_DIRECTORY(Templates bcri_calib_sources)
ADD_LIBRARY(bcri_calib ${bcri_calib_sources})
TARGET_LINK_LIBRARIES(bcri_calib bgui sdet brip btol bdgl mbl mvl  vdgl vtol vsol vul vbl vnl vcl vil1 vil  vsl )



ADD_EXECUTABLE (bcri_calib_bin  bcri_calib_man.cxx)
TARGET_LINK_LIBRARIES(bcri_calib_bin bcri_calib )

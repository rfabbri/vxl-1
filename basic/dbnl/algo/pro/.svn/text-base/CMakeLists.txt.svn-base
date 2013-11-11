# basic/dbnl/algo/pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )


SET(dbnl_pro_sources
   dbnl_function_sampling_process.cxx dbnl_function_sampling_process.h
)


#AUX_SOURCE_DIRECTORY(Templates dbnl_pro_sources)

ADD_LIBRARY(dbnl_pro  ${dbnl_pro_sources})

TARGET_LINK_LIBRARIES(dbnl_pro vidpro1 vsol dbnl_algo)

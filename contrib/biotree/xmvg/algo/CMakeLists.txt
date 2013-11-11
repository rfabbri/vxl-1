INCLUDE_DIRECTORIES(${BIOTREE_SOURCE_DIR} ${lems_SOURCE_DIR}/basic)

SET(xmvg_algo_src
    xmvg_dot_product_2d.txx             xmvg_dot_product_2d.h
    xmvg_ramp_compensation.txx             xmvg_ramp_compensation.h
   )

AUX_SOURCE_DIRECTORY(Templates xmvg_algo_src)

ADD_LIBRARY(xmvg_algo ${xmvg_algo_src})

TARGET_LINK_LIBRARIES(xmvg_algo xmvg dbil vnl_algo vnl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

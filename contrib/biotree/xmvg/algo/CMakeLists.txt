include_directories(${BIOTREE_SOURCE_DIR} ${lems_SOURCE_DIR}/basic)

set(xmvg_algo_src
    xmvg_dot_product_2d.txx             xmvg_dot_product_2d.h
    xmvg_ramp_compensation.txx             xmvg_ramp_compensation.h
   )

aux_source_directory(Templates xmvg_algo_src)

add_library(xmvg_algo ${xmvg_algo_src})

target_link_libraries(xmvg_algo xmvg dbil vnl_algo vnl)

if(BUILD_TESTING)
  subdirs(tests)
endif(BUILD_TESTING)

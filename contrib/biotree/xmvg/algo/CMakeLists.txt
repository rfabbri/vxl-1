include_directories(${BIOTREE_SOURCE_DIR} ${lems_SOURCE_DIR}/basic)

set(xmvg_algo_src
    xmvg_dot_product_2d.hxx             xmvg_dot_product_2d.h
    xmvg_ramp_compensation.hxx             xmvg_ramp_compensation.h
   )

aux_source_directory(Templates xmvg_algo_src)

add_library(xmvg_algo ${xmvg_algo_src})

target_link_libraries(xmvg_algo xmvg dbil vnl_algo vnl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

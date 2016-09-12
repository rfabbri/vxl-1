# Calibration of scan by using an artifact model
# blem/contrib/biotree/xscan/cali/CMakeLists.txt

include_directories(
${VXL_GEL_INCLUDE_DIR}/mrc
${biotree_SOURCE_DIR}
${biotree_SOURCE_DIR}/resc
${biotree_SOURCE_DIR}/xscan
${lems_SOURCE_DIR}/brcv/mvg
${lems_SOURCE_DIR}/basic/
${VXL_BRL_INCLUDE_DIR}
${VXL_GEL_INCLUDE_DIR}
${VXL_BRL_INCLUDE_DIR}/bseg 
${VXL_BRL_INCLUDE_DIR}/bbas
)


set(cali_sources
cali_artf_model.h
cali_simulated_scan_resource.h                cali_simulated_scan_resource.cxx
cali_artf_projector.h                         cali_artf_projector.cxx
cali_artf_corresponder.h                      cali_artf_corresponder.cxx
cali_conic_info.h                             cali_conic_info.cxx
cali_vnl_least_squares_function.h             cali_vnl_least_squares_function.cxx
cali_calibrator.h                             cali_calibrator.cxx
cali_cylinder_artifact.h                      cali_cylinder_artifact.cxx
cali_param.h                                  cali_param.cxx 

)

aux_source_directory(Templates cali_sources)

add_library(cali ${cali_sources})


target_link_libraries(cali imgr xmvg xscan vpgl_algo vpgl dbil dbgl_algo vil vgl_algo vgl vnl vbl vul sdet vsl vsol)



if(BUILD_EXAMPLES)
  subdirs(examples)
endif(BUILD_EXAMPLES)

subdirs(tests)

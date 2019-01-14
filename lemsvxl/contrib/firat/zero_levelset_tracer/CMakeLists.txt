include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${LEMS_SOURCE_DIR}/brcv/seg)

#set(zero_levelset_tracer_sources
#       zero_levelset_tracer.cxx
#    )
#add_executable(zero_levelset_tracer  ${zero_levelset_tracer_sources} )
#target_link_libraries (zero_levelset_tracer dbdet_contour_tracer vil dbsol dbsol_algo)

set(zero_levelset_tracer2_sources
       zero_levelset_tracer2.cxx
    )
add_executable(zero_levelset_tracer2  ${zero_levelset_tracer2_sources} )
target_link_libraries (zero_levelset_tracer2 dbdet_contour_tracer vil dbsol dbsol_algo)

#set(zero_levelset_tracer3_sources
#       zero_levelset_tracer3.cxx
#    )
#add_executable(zero_levelset_tracer3  ${zero_levelset_tracer3_sources} )
#target_link_libraries (zero_levelset_tracer3 dbdet_contour_tracer vil dbsol dbsol_algo)

set(compute_zero_level_curve_sources
       compute_zero_level_curve.cxx
    )
add_executable(compute_zero_level_curve  ${compute_zero_level_curve_sources} )
target_link_libraries (compute_zero_level_curve dbdet_contour_tracer vil dbsol dbsol_algo)
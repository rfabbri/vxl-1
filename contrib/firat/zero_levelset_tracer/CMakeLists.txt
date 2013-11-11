INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/seg)

#SET(zero_levelset_tracer_sources
#       zero_levelset_tracer.cxx
#    )
#ADD_EXECUTABLE(zero_levelset_tracer  ${zero_levelset_tracer_sources} )
#TARGET_LINK_LIBRARIES (zero_levelset_tracer dbdet_contour_tracer vil dbsol dbsol_algo)

SET(zero_levelset_tracer2_sources
       zero_levelset_tracer2.cxx
    )
ADD_EXECUTABLE(zero_levelset_tracer2  ${zero_levelset_tracer2_sources} )
TARGET_LINK_LIBRARIES (zero_levelset_tracer2 dbdet_contour_tracer vil dbsol dbsol_algo)

#SET(zero_levelset_tracer3_sources
#       zero_levelset_tracer3.cxx
#    )
#ADD_EXECUTABLE(zero_levelset_tracer3  ${zero_levelset_tracer3_sources} )
#TARGET_LINK_LIBRARIES (zero_levelset_tracer3 dbdet_contour_tracer vil dbsol dbsol_algo)

SET(compute_zero_level_curve_sources
       compute_zero_level_curve.cxx
    )
ADD_EXECUTABLE(compute_zero_level_curve  ${compute_zero_level_curve_sources} )
TARGET_LINK_LIBRARIES (compute_zero_level_curve dbdet_contour_tracer vil dbsol dbsol_algo)
# include directories into here if needed
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_TBL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas/bil/algo )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )

set(dbdet_contour_tracer_sources  

   dbdet_contour_tracer.cxx dbdet_contour_tracer.h 
   dbdet_contour_tracer_sptr.h 
   dbdet_contour_tracer_core.h
   dbdet_contour_tracer_core2.cxx 
   dbdet_contour_tracer_core.cxx 
   dbdet_contour_tracer_curvature_smoothing.cxx 
   dbdet_contour_tracer_grid.cxx dbdet_contour_tracer_grid.h
   dbdet_trace_contours_on_grid.h dbdet_trace_contours_on_grid.cxx

)

add_library(dbdet_contour_tracer ${dbdet_contour_tracer_sources})

# add the required libraries into this list
target_link_libraries( dbdet_contour_tracer vil1 vil bil_algo dbnl_algo osl vepl vepl vdgl vnl_io bdgl dbgl_algo brip pdf1d mbl)


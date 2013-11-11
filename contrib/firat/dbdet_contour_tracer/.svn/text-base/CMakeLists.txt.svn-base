# include directories into here if needed
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_TBL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas/bil/algo )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )

SET(dbdet_contour_tracer_sources  

   dbdet_contour_tracer.cxx dbdet_contour_tracer.h 
   dbdet_contour_tracer_sptr.h 
   dbdet_contour_tracer_core.h
   dbdet_contour_tracer_core2.cxx 
   dbdet_contour_tracer_core.cxx 
   dbdet_contour_tracer_curvature_smoothing.cxx 
   dbdet_contour_tracer_grid.cxx dbdet_contour_tracer_grid.h
   dbdet_trace_contours_on_grid.h dbdet_trace_contours_on_grid.cxx

)

ADD_LIBRARY(dbdet_contour_tracer ${dbdet_contour_tracer_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbdet_contour_tracer vil1 vil bil_algo dbnl_algo osl vepl vepl1 vdgl vnl_io bdgl dbgl_algo brip pdf1d mbl)


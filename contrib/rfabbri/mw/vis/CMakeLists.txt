INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bmvl )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )


SET(mw_vis_sources
  mw_curve_tracing_tool_2.cxx mw_curve_tracing_tool_2.h mw_curve_tracing_tool_common_2.h
  mw_curve_dt_tracing_tool_2.cxx mw_curve_dt_tracing_tool_2.h
  mw_curve_edit_tracing_tool_2.cxx mw_curve_edit_tracing_tool_2.h
#
# The following is old deprecated code
#
#mw_curve_tracing_tool.cxx mw_curve_tracing_tool.h
#mw_curve_tracing_tool_utils.cxx mw_curve_tracing_tool_utils.h
#mw_curve_tracing_tool_rec.cxx 

  mw_curve_appearance_tracing_tool.cxx mw_curve_appearance_tracing_tool.h
  mw_curve_appearance_tracing_tool_utils.cxx mw_curve_appearance_tracing_tool_utils.h
  mw_curve_appearance_tracing_tool_rec.cxx 

  #  mw_curve_dt_tracing_tool.cxx mw_curve_dt_tracing_tool.h
  #  mw_curve_dt_tracing_tool_utils.cxx mw_curve_dt_tracing_tool_utils.h
  #mw_curve_dt_tracing_tool_rec.cxx 

  oldtracer/mw_curve_tracing_tool_utils_3.cxx oldtracer/mw_curve_tracing_tool_utils_3.h
  oldtracer/mw_curve_tracing_tool_3.cxx oldtracer/mw_curve_tracing_tool_3.h
  oldtracer/mw_curve_tracing_tool_rec_3.cxx 
#  oldtracer/mw_curve_tracing_tool_2.cxx oldtracer/mw_curve_tracing_tool_2.h
#  mw_sel_tracing_tool.cxx mw_sel_tracing_tool.h
#  mw_correspond_point_tool.cxx mw_correspond_point_tool.h
  mw_3_correspond_point_tool.cxx mw_3_correspond_point_tool.h
  mw_correspond_point_tool_basic.cxx mw_correspond_point_tool_basic.h
  mw_correspond_point_tool_band.cxx mw_correspond_point_tool_band.h
  mw_sel_geometry_tool.cxx mw_sel_geometry_tool.h
  mw_sel_inliers_to_curve.cxx mw_sel_inliers_to_curve.h
)

ADD_LIBRARY(mw_vis ${mw_vis_sources})

TARGET_LINK_LIBRARIES(mw_vis dbdet_vis bvis1 mw_algo dbcsi vpgl brct )


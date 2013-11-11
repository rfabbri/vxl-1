CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(VXL_FOUND)

  SET(hist_match_sources kullback_liebler_cost.h  kullback_liebler_cost.cxx
                         bhattacharyya_cost.h )

  ADD_LIBRARY(hist_match ${hist_match_sources})
  INSTALL_TARGETS(/lib hist_match)
  TARGET_LINK_LIBRARIES(hist_match vnl vnl_algo)
  
ENDIF(VXL_FOUND)



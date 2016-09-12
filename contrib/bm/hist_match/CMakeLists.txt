cmake_minimum_required(VERSION 2.6)

if(VXL_FOUND)

  set(hist_match_sources kullback_liebler_cost.h  kullback_liebler_cost.cxx
                         bhattacharyya_cost.h )

  add_library(hist_match ${hist_match_sources})
  install_targets(/lib hist_match)
  target_link_libraries(hist_match vnl vnl_algo)
  
endif(VXL_FOUND)



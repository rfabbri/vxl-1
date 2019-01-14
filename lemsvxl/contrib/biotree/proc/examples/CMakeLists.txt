# (c) 2005 Bio-Tree Systems, Inc.  All rights reserved.
# No part of this software may be used, reproduced or transmitted for
# any purpose, in any form or by any means, except as expressly
# permitted in writing by Bio-Tree Systems, Inc.
    
# library for storing / loading 3d image 
if(VXL_VGUI_FOUND)
  include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
endif (VXL_VGUI_FOUND)

# brown Volume Rendering

include_directories( ${VXL_MUL_INCLUDE_DIR})
include_directories( ${LEMS_SOURCE_DIR}/contrib/biotree/xmvg)
include_directories( ${LEMS_SOURCE_DIR}/db3p )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )


add_executable(bioproc_example_xml bioproc_filtering_example_xml.cxx )
target_link_libraries(bioproc_example_xml bioproc bioproc_io expatpp)

# the splr example is now inside bioprc_example_xml
#add_executable(bioproc_splr_example_xml bioproc_splr_filtering_example_xml.cxx )
#target_link_libraries(bioproc_splr_example_xml bioproc bgui3d bioproc_io expatpp)
  
add_executable(bioproc_edge_detector_example bioproc_edge_detector_example.cxx)
target_link_libraries(bioproc_edge_detector_example bioproc)

# (c) 2005 Bio-Tree Systems, Inc.  All rights reserved.
# No part of this software may be used, reproduced or transmitted for
# any purpose, in any form or by any means, except as expressly
# permitted in writing by Bio-Tree Systems, Inc.
    
# library for storing / loading 3d image 
IF(VXL_VGUI_FOUND)
  INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
ENDIF (VXL_VGUI_FOUND)

# brown Volume Rendering

INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/biotree/xmvg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )


ADD_EXECUTABLE(bioproc_example_xml bioproc_filtering_example_xml.cxx )
TARGET_LINK_LIBRARIES(bioproc_example_xml bioproc bioproc_io expatpp)

# the splr example is now inside bioprc_example_xml
#ADD_EXECUTABLE(bioproc_splr_example_xml bioproc_splr_filtering_example_xml.cxx )
#TARGET_LINK_LIBRARIES(bioproc_splr_example_xml bioproc bgui3d bioproc_io expatpp)
  
ADD_EXECUTABLE(bioproc_edge_detector_example bioproc_edge_detector_example.cxx)
TARGET_LINK_LIBRARIES(bioproc_edge_detector_example bioproc)

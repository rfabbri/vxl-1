# (c) 2005 Bio-Tree Systems, Inc.  All rights reserved.
# No part of this software may be used, reproduced or transmitted for
# any purpose, in any form or by any means, except as expressly
# permitted in writing by Bio-Tree Systems, Inc.

INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic ${lems_SOURCE_DIR}/brcv/mvg ${biotree_SOURCE_DIR}/resc)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

SET(bioproc_src
    bioproc_filtering_proc.txx        bioproc_filtering_proc.h
    bioproc_splr_filtering_proc.txx   bioproc_splr_filtering_proc.h
    bioproc_dummy_splat_resource.txx  bioproc_dummy_splat_resource.h
    bioproc_filtering_proc_helper.h
   )

AUX_SOURCE_DIRECTORY(Templates bioproc_src)

ADD_LIBRARY(bioproc ${bioproc_src})

TARGET_LINK_LIBRARIES(bioproc vcl vgl vsl vil3d imgr imgr_file_format xmvg xmvg_algo splr xmvg_io biob )

SUBDIRS( util )

SUBDIRS( io )

SUBDIRS( biocluster )

IF(BUILD_EXAMPLES)
#  IF(SIMVOLEON_FOUND)
    SUBDIRS(examples)
#  ENDIF(SIMVOLEON_FOUND)
ENDIF(BUILD_EXAMPLES)
  
IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

IF(SIMVOLEON_FOUND)
  SUBDIRS(vis)
ENDIF(SIMVOLEON_FOUND)

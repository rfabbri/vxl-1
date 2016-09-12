# (c) 2005 Bio-Tree Systems, Inc.  All rights reserved.
# No part of this software may be used, reproduced or transmitted for
# any purpose, in any form or by any means, except as expressly
# permitted in writing by Bio-Tree Systems, Inc.

include_directories(${lems_SOURCE_DIR}/basic ${lems_SOURCE_DIR}/brcv/mvg ${biotree_SOURCE_DIR}/resc)
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_MUL_INCLUDE_DIR})
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib )

set(bioproc_src
    bioproc_filtering_proc.txx        bioproc_filtering_proc.h
    bioproc_splr_filtering_proc.txx   bioproc_splr_filtering_proc.h
    bioproc_dummy_splat_resource.txx  bioproc_dummy_splat_resource.h
    bioproc_filtering_proc_helper.h
   )

aux_source_directory(Templates bioproc_src)

add_library(bioproc ${bioproc_src})

target_link_libraries(bioproc vcl vgl vsl vil3d imgr imgr_file_format xmvg xmvg_algo splr xmvg_io biob )

subdirs( util )

subdirs( io )

subdirs( biocluster )

if(BUILD_EXAMPLES)
#  if(SIMVOLEON_FOUND)
    subdirs(examples)
#  endif(SIMVOLEON_FOUND)
endif(BUILD_EXAMPLES)
  
if(BUILD_TESTING)
  subdirs(tests)
endif(BUILD_TESTING)

if(SIMVOLEON_FOUND)
  subdirs(vis)
endif(SIMVOLEON_FOUND)

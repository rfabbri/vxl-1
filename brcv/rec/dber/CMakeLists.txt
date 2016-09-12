# Brown Edge-based Recognition Library  
# brcv/rec/dber/CMakeLists.txt

include_directories(${VXL_GEL_INCLUDE_DIR})
include_directories(${VXL_BRL_INCLUDE_DIR})
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg)
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${VXL_MUL_INCLUDE_DIR} )

set(dber_sources
dber_match.h            dber_match.cxx
dber_edgel_similarity.h dber_edgel_similarity.cxx
dber_utilities.h        dber_utilities.cxx
)

aux_source_directory(Templates dber_sources)

add_library(dber  ${dber_sources})

target_link_libraries(dber vnl dbsol dbru)

# algorithms library
#subdirs(algo)

# Processing Library
subdirs(pro)

# Visualization Library
subdirs(vis)

#if(BUILD_TESTING)
#  subdirs(tests)
#endif(BUILD_TESTING)

# Executables
#subdirs(bin)

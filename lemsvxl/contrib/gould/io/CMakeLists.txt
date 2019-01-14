# contrib/gould/io/CMakeLists.txt
# Benjamin Gould


include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/contrib/fine )

set(dbcfg_io_srcs

     dbcfg_io_seg_conversion.h  dbcfg_io_seg_conversion.cxx
     dbcfg_io_amir_conversion.h dbcfg_io_amir_conversion.cxx
     dbcfg_io_cem_conversion.h  dbcfg_io_cem_conversion.cxx
)

add_library(dbcfg_io ${dbcfg_io_srcs})

target_link_libraries(dbcfg_io dbcfg_structure dbseg_structure vcl vnl dbdet)


# contrib/gould/io/CMakeLists.txt
# Benjamin Gould


INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/fine )

SET(dbcfg_io_srcs

     dbcfg_io_seg_conversion.h  dbcfg_io_seg_conversion.cxx
     dbcfg_io_amir_conversion.h dbcfg_io_amir_conversion.cxx
     dbcfg_io_cem_conversion.h  dbcfg_io_cem_conversion.cxx
)

ADD_LIBRARY(dbcfg_io ${dbcfg_io_srcs})

TARGET_LINK_LIBRARIES(dbcfg_io dbcfg_structure dbseg_structure vcl vnl dbdet)


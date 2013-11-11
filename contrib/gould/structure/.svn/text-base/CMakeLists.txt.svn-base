# contrib/gould/structure/CMakeLists.txt
# Benjamin Gould


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

SET(dbcfg_structure_srcs

     dbcfg_curve.h                  dbcfg_curve.txx
                                    dbcfg_junction.txx
     dbcfg_contour_fragment_graph.h dbcfg_contour_fragment_graph.txx
     dbcfg_cfg_constructor.h        dbcfg_cfg_constructor.txx
     dbcfg_line.h                   dbcfg_line.txx
)

AUX_SOURCE_DIRECTORY(Templates dbcfg_structure_srcs)

ADD_LIBRARY(dbcfg_structure ${dbcfg_structure_srcs})

TARGET_LINK_LIBRARIES(dbcfg_structure vcl vnl dbdet)


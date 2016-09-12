# contrib/gould/structure/CMakeLists.txt
# Benjamin Gould


include_directories( ${lems_SOURCE_DIR}/brcv/seg )

set(dbcfg_structure_srcs

     dbcfg_curve.h                  dbcfg_curve.txx
                                    dbcfg_junction.txx
     dbcfg_contour_fragment_graph.h dbcfg_contour_fragment_graph.txx
     dbcfg_cfg_constructor.h        dbcfg_cfg_constructor.txx
     dbcfg_line.h                   dbcfg_line.txx
)

aux_source_directory(Templates dbcfg_structure_srcs)

add_library(dbcfg_structure ${dbcfg_structure_srcs})

target_link_libraries(dbcfg_structure vcl vnl dbdet)


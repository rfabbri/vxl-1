# contrib/seg/vis/CMakeLists.txt

if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )


set(dbseg_vis_sources
    dbseg_seg_displayer.cxx            dbseg_seg_displayer.h
    dbseg_seg_tableau.cxx            dbseg_seg_tableau.h
                        dbseg_seg_tableau_sptr.h
                        dbseg_seg_drawing_tableau_sptr.h
    dbseg_seg_drawing_tableau.cxx        dbseg_seg_drawing_tableau.h
    dbseg_seg_create_region_tool.cxx    dbseg_seg_create_region_tool.h
    dbseg_seg_edit_region_tool.cxx        dbseg_seg_edit_region_tool.h
    dbseg_seed_regions_tool.cxx        dbseg_seed_regions_tool.h                                        
    dbseg_seg_label_region_tool.cxx        dbseg_seg_label_region_tool.h
    
    
)

add_library( dbseg_vis  ${dbseg_vis_sources})

target_link_libraries(dbseg_vis dbseg_structure vidpro1 bvis1)

endif (VXL_VGUI_FOUND)



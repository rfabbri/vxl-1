# contrib/seg/vis/CMakeLists.txt

IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )


SET(dbseg_vis_sources
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

ADD_LIBRARY( dbseg_vis  ${dbseg_vis_sources})

TARGET_LINK_LIBRARIES(dbseg_vis dbseg_structure vidpro1 bvis1)

ENDIF (VXL_VGUI_FOUND)



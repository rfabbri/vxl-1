# contrib/seg/pro/CMakeLists.txt

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


SET(dbseg_pro_sources
                                        
    dbseg_seg_process.cxx                dbseg_seg_process.h
    dbseg_seg_save_process.cxx            dbseg_seg_save_process.h
    dbseg_seg_load_process.cxx            dbseg_seg_load_process.h
    dbseg_mean_shift_process.cxx        dbseg_mean_shift_process.h
    dbseg_seg_save_view_process.cxx        dbseg_seg_save_view_process.h    
    dbseg_seg_save_contour_process.cxx    dbseg_seg_save_contour_process.h
    dbseg_graph_process.cxx                dbseg_graph_process.h
    dbseg_jseg_process.cxx                dbseg_jseg_process.h
    #dbseg_itk_watershed_process.cxx    dbseg_itk_watershed_process.h
    #dbseg_ripple_process.cxx            dbseg_ripple_process.h                                        
    #dbseg_subpixel_process.cxx            dbseg_subpixel_process.h
    dbseg_srgtek_process.cxx            dbseg_srgtek_process.h
    #dbseg_levelset_process.cxx            dbseg_levelset_process.h


    dbseg_seg_load_sequence_process.cxx        dbseg_seg_load_sequence_process.h
    dbseg_seg_save_binimage_process.cxx        dbseg_seg_save_binimage_process.h
    
    dbseg_disk_measurements_process.cxx        dbseg_disk_measurements_process.h
    dbseg_disk_bin_images_process.cxx        dbseg_disk_bin_images_process.h
    
)

ADD_LIBRARY( dbseg_pro  ${dbseg_pro_sources})

TARGET_LINK_LIBRARIES(dbseg_pro dbseg_algos dbseg_structure vidpro1 bvis1)

ENDIF (VXL_VGUI_FOUND)



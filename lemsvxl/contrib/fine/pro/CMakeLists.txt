# contrib/seg/pro/CMakeLists.txt

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


set(dbseg_pro_sources
                                        
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

add_library( dbseg_pro  ${dbseg_pro_sources})

target_link_libraries(dbseg_pro dbseg_algos dbseg_structure vidpro1 bvis1)

endif (VXL_VGUI_FOUND)



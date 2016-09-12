# contrib/seg/structure/CMakeLists.txt

if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )


set(dbseg_structure_sources
   dbseg_seg_object.cxx                        dbseg_seg_object.h
   dbseg_seg_neighborhood.cxx                dbseg_seg_neighborhood.h
   dbseg_seg_spatial_object.hxx                dbseg_seg_spatial_object.h
   dbseg_seg_tree.hxx                        dbseg_seg_tree.h
   dbseg_seg_storage.cxx                    dbseg_seg_storage.h
                                            dbseg_seg_storage_sptr.h
    dbseg_seg_object_base.cxx                dbseg_seg_object_base.h
                                    
    dbseg_seg_object+vxl_byte-.cxx
    dbseg_seg_spatial_object+vxl_byte-.cxx
    dbseg_seg_tree+vxl_byte-.cxx
    #dbseg_seg_storage+vxl_byte-.cxx
    
)
add_library(dbseg_structure ${dbseg_structure_sources})
target_link_libraries(dbseg_structure dbdet)

endif (VXL_VGUI_FOUND)



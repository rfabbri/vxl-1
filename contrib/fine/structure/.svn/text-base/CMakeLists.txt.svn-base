# contrib/seg/structure/CMakeLists.txt

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


SET(dbseg_structure_sources
   dbseg_seg_object.cxx                        dbseg_seg_object.h
   dbseg_seg_neighborhood.cxx                dbseg_seg_neighborhood.h
   dbseg_seg_spatial_object.txx                dbseg_seg_spatial_object.h
   dbseg_seg_tree.txx                        dbseg_seg_tree.h
   dbseg_seg_storage.cxx                    dbseg_seg_storage.h
                                            dbseg_seg_storage_sptr.h
    dbseg_seg_object_base.cxx                dbseg_seg_object_base.h
                                    
    dbseg_seg_object+vxl_byte-.cxx
    dbseg_seg_spatial_object+vxl_byte-.cxx
    dbseg_seg_tree+vxl_byte-.cxx
    #dbseg_seg_storage+vxl_byte-.cxx
    
)
ADD_LIBRARY(dbseg_structure ${dbseg_structure_sources})
TARGET_LINK_LIBRARIES(dbseg_structure dbdet)

ENDIF (VXL_VGUI_FOUND)



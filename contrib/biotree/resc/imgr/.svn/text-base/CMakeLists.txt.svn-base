# Detection for filters
# blem/contrib/biotree/resc/imgr/CMakeLists.txt


INCLUDE_DIRECTORIES(
${VXL_GEL_INCLUDE_DIR}/mrc
${biotree_SOURCE_DIR}
${biotree_SOURCE_DIR}/resc
${lems_SOURCE_DIR}/brcv/mvg
${lems_SOURCE_DIR}/basic/

#pnk added:
/usr/X11R6/include/
)


SET(imgr_sources
imgr_dummy_scan.cxx             imgr_dummy_scan.h
imgr_scan_resource.cxx          imgr_scan_resource.h  imgr_scan_resource_sptr.h
imgr_scan_images_resource.cxx   imgr_scan_images_resource.h
imgr_scan_resource_io.cxx       imgr_scan_resource_io.h
imgr_view_data_base.h           imgr_view_data_base_sptr.h
imgr_image_view_3d_base.h       imgr_image_view_3d_base_sptr.h 
imgr_bounded_image_view_3d.txx  imgr_bounded_image_view_3d.h
imgr_scan_images_resource.h        
)

AUX_SOURCE_DIRECTORY(Templates imgr_sources)

ADD_LIBRARY(imgr ${imgr_sources})

TARGET_LINK_LIBRARIES(imgr xmvg xscan vpgl_algo vpgl dbil dbgl_algo vil vgl_algo vgl vnl vbl vul)

#viewing tool for imgr 
IF (VXL_VGUI_FOUND)
   SUBDIRS(imgrv)
ENDIF (VXL_VGUI_FOUND)

SUBDIRS(file_formats)

SUBDIRS( examples )

SUBDIRS(tests)

# Detection for filters
# blem/contrib/biotree/resc/imgr/CMakeLists.txt


include_directories(
${VXL_GEL_INCLUDE_DIR}/mrc
${biotree_SOURCE_DIR}
${biotree_SOURCE_DIR}/resc
${lems_SOURCE_DIR}/brcv/mvg
${lems_SOURCE_DIR}/basic/

#pnk added:
/usr/X11R6/include/
)


set(imgr_sources
imgr_dummy_scan.cxx             imgr_dummy_scan.h
imgr_scan_resource.cxx          imgr_scan_resource.h  imgr_scan_resource_sptr.h
imgr_scan_images_resource.cxx   imgr_scan_images_resource.h
imgr_scan_resource_io.cxx       imgr_scan_resource_io.h
imgr_view_data_base.h           imgr_view_data_base_sptr.h
imgr_image_view_3d_base.h       imgr_image_view_3d_base_sptr.h 
imgr_bounded_image_view_3d.txx  imgr_bounded_image_view_3d.h
imgr_scan_images_resource.h        
)

aux_source_directory(Templates imgr_sources)

add_library(imgr ${imgr_sources})

target_link_libraries(imgr xmvg xscan vpgl_algo vpgl dbil dbgl_algo vil vgl_algo vgl vnl vbl vul)

#viewing tool for imgr 
if (VXL_VGUI_FOUND)
   subdirs(imgrv)
endif (VXL_VGUI_FOUND)

subdirs(file_formats)

subdirs( examples )

subdirs(tests)

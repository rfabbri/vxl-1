# Detection for filters
# blem/contrib/biotree/resc/imgr/CMakeLists.txt

include_directories(
${VXL_GEL_INCLUDE_DIR}/mrc
${VXLCORE_INCLUDE_DIR}
${VXL_BRL_INCLUDE_DIR}/bbas
${biotree_SOURCE_DIR}
${biotree_SOURCE_DIR}/resc/imgr
)


set(imgrv_sources
imgrv_manager.cxx  imgrv_manager.h
)


add_library(imgrv ${imgrv_sources})


target_link_libraries(imgrv imgr xmvg vpgl_algo vpgl bgui vgui dbil  vil vgl_algo vgl vnl vbl )

# The executable tool
subdirs(bin)

# Detection for filters
# blem/contrib/biotree/resc/imgr/CMakeLists.txt

INCLUDE_DIRECTORIES(
${VXL_GEL_INCLUDE_DIR}/mrc
${VXLCORE_INCLUDE_DIR}
${VXL_BRL_INCLUDE_DIR}/bbas
${biotree_SOURCE_DIR}
${biotree_SOURCE_DIR}/resc/imgr
)


SET(imgrv_sources
imgrv_manager.cxx  imgrv_manager.h
)


ADD_LIBRARY(imgrv ${imgrv_sources})


TARGET_LINK_LIBRARIES(imgrv imgr xmvg vpgl_algo vpgl bgui vgui dbil  vil vgl_algo vgl vnl vbl )

# The executable tool
SUBDIRS(bin)

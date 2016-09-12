include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_TBL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas/bil/algo )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${lems_SOURCE_DIR}/brcv/seg)

set(firat_contour_src
detect_and_export_contours.cxx
)
add_executable(detect_and_export_contours ${firat_contour_src})
target_link_libraries(detect_and_export_contours dbdet_pro)

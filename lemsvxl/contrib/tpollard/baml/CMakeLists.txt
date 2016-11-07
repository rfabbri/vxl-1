include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )

add_library( baml
             baml_appearance.h         baml_appearance.cxx
             baml_mog.h                baml_mog.cxx
             baml_multi_mog.h          baml_multi_mog.cxx
             baml_prob_img.h           baml_prob_img.cxx )


target_link_libraries( baml vcl vgl vnl vil vgl_algo )
set( BAML_INCLUDE_DIR ${baml_SOURCE_DIR} )

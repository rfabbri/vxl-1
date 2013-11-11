INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )

ADD_LIBRARY( baml
             baml_appearance.h         baml_appearance.cxx
             baml_mog.h                baml_mog.cxx
             baml_multi_mog.h          baml_multi_mog.cxx
             baml_prob_img.h           baml_prob_img.cxx )


TARGET_LINK_LIBRARIES( baml vcl vgl vnl vil vgl_algo )
SET( BAML_INCLUDE_DIR ${baml_SOURCE_DIR} )

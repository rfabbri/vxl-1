# This is contrib/dbrec/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas/bsta )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg/ )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

SET(dbrec_sources
    dbrec_part_hierarchy_builder.h    dbrec_part_hierarchy_builder.cxx

    # following files are for the new implementation of the recognition library
    dbrec_part.h dbrec_part.cxx 
    dbrec_part_sptr.h

    dbrec_compositor.h dbrec_compositor.cxx
    dbrec_compositor_sptr.h
    
    dbrec_primitive_factory.h dbrec_primitive_factory.cxx  
    dbrec_primitive_factory_sptr.h
        
    dbrec_type_id_factory.h dbrec_type_id_factory.cxx 

    dbrec_part_context.h dbrec_part_context.cxx 
    dbrec_part_context_sptr.h

    dbrec_visitor.h  dbrec_visitor.cxx
    dbrec_visitor_sptr.h

    # classes related to recognition from 2D images    
    dbrec_gaussian_primitive.h dbrec_gaussian_primitive.cxx 

    dbrec_image_learner_visitors.h dbrec_image_learner_visitors.cxx
    dbrec_image_learner_visitors_sptr.h

    dbrec_image_visitors.h dbrec_image_visitors.cxx

    dbrec_image_models.h dbrec_image_models.cxx
    dbrec_image_models_sptr.h

    dbrec_image_compositor.h dbrec_image_compositor.cxx
    dbrec_image_pairwise_models.h dbrec_image_pairwise_models.cxx
    dbrec_image_pairwise_models_sptr.h

    dbrec_image_hierarchy_factory.h dbrec_image_hierarchy_factory.cxx

    dbrec_part_selector.h dbrec_part_selector.cxx
)

AUX_SOURCE_DIRECTORY(Templates dbrec_sources)

ADD_LIBRARY(dbrec ${dbrec_sources})

TARGET_LINK_LIBRARIES(dbrec brec bsvg dbfs)

SUBDIRS( pro )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )





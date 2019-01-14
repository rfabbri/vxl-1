# This is contrib/dbrec/CMakeLists.txt

include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas/bsta )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg/ )
include_directories( ${LEMS_SOURCE_DIR}/contrib/dbrec_lib )
include_directories( ${LEMS_SOURCE_DIR}/basic )

set(dbrec_sources
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

aux_source_directory(Templates dbrec_sources)

add_library(dbrec ${dbrec_sources})

target_link_libraries(dbrec brec bsvg dbfs)

add_subdirectory( pro )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )





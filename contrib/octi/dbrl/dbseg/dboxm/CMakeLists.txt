# contrib/dbrl/dbseg/dboxm/CMakeLists.txt
include( ${DBRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)

include_directories( ${EXPAT_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR} )
#include_directories( ${BRL_INCLUDE_DIR}/bbas)
include_directories( ${GEL_INCLUDE_DIR}/mrc )
include_directories( ${BRL_INCLUDE_DIR}/b3p/expatpp )

set(dboxm_sources
    boxm_block.txx                         boxm_block.h
    boxm_scene_base.cxx                    boxm_scene_base.h
    boxm_scene.txx                         boxm_scene.h
    boxm_scene_parser.cxx                  boxm_scene_parser.h
    boxm_sample.txx                        boxm_sample.h
    boxm_sample_multi_bin.txx              boxm_sample_multi_bin.h
 
    boxm_apm_traits.cxx                    boxm_apm_traits.h
    boxm_mog_grey_processor.cxx            boxm_mog_grey_processor.h
    boxm_simple_grey_processor.cxx         boxm_simple_grey_processor.h
    boxm_block_vis_graph_node.h
    boxm_block_vis_graph_iterator.txx      boxm_block_vis_graph_iterator.h
    boxm_cell_vis_graph_iterator.txx       boxm_cell_vis_graph_iterator.h
    boxm_neighb_graph_node.h
    boxm_utils.cxx                         boxm_utils.h
    boxm_render_image.h                      boxm_render_image_triangle.h
    boxm_depth_image_triangle.h
    boxm_update_triangle.h
    boxm_refine.h
    boxm_save_block_raw.h
    boxm_save_scene_raw.h
    boxm_quad_scan_iterator.h              boxm_quad_scan_iterator.cxx
    boxm_compute_volume_visibility.h
    boxm_triangle_interpolation_iterator.h boxm_triangle_interpolation_iterator.txx
    boxm_triangle_scan_iterator.h          boxm_triangle_scan_iterator.cxx
    boxm_raytrace_operations.h             boxm_raytrace_operations.cxx

    boxm_rational_camera_utils.h           boxm_rational_camera_utils.cxx
    boxm_upload_mesh.h
    boxm_fill_in_mesh.h
    boxm_vis_implicit_sample.h
    boxm_update_sample.h
    boxm_mob_grey_processor.cxx            boxm_mob_grey_processor.h            

)

aux_source_directory(Templates dboxm_sources)

add_library(dboxm  ${dboxm_sources})

target_link_libraries(dboxm boct vcl vgl vgl_io vgl_xio vbl vsl vpgl vpgl_algo vsol bgeo bsta bsta_algo vul vil imesh expatpp)

#processes
#add_subdirectory(pro)

#io
#add_subdirectory(io)

#tests
#add_subdirectory(tests)

#opt
add_subdirectory(opt)

#basic algorithms involving scenes
#add_subdirectory(algo)

endif(EXPAT_FOUND)

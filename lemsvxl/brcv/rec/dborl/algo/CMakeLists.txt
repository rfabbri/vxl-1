# This is brcv/rec/dborl/algo/CMakeLists.txt
# The Development Brown Object Recognition Library - Specific Algorithms

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/b3p )
include_directories( ${VXL_EXPAT_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/brcv/shp)
include_directories( ${lems_SOURCE_DIR}/brcv/rec)
include_directories( ${lems_SOURCE_DIR}/brcv/seg)
include_directories( ${lems_SOURCE_DIR}/breye1 )


set( dborl_algo_sources
   dborl_utilities.h dborl_utilities.cxx
   dborl_image_desc_parser.h        dborl_image_desc_parser.cxx
   dborl_category_info_parser.h     dborl_category_info_parser.cxx
   dborl_index_parser.h             dborl_index_parser.cxx
   dborl_algo_params.h              dborl_algo_params.cxx
   dborl_xio_image_description.h    dborl_xio_image_description.cxx 
   dborl_search_index_file.h        dborl_search_index_file.cxx
)

aux_source_directory(Templates dborl_algo_sources)

add_library( dborl_algo ${dborl_algo_sources} )

target_link_libraries( dborl_algo dborl buld expatpp bsold bxml bpro1 dbxml vbl) 

# Brown ORL Algo Testing
add_subdirectory( tests )

# Brown ORL Cluster Utilities via MPI
add_subdirectory( mpi )

# Brown ORL Algo Examples
add_subdirectory( examples )

# Brown VOX Edge detection Algos
add_subdirectory( edge )

# Brown VOX Edge Linking Algos to create boundary fragment maps
add_subdirectory( bnd )

# Brown VOX Intrinsic Shock Computation Algorithm to compute intrinsic shock from a boundary map
add_subdirectory( vox_compute_ishock )

# Brown VOX binaries to evaluate various edge detectors and linkers
add_subdirectory( vox_eval_edge_linkers )

# Brown VOX algorithm to detect objects represented by shock graphs in images
add_subdirectory( vox_detect_xgraph )

# Brown VOX algorithm to perform edge detection and edge linking
add_subdirectory( vox_compute_contours )

add_subdirectory( vox_compute_contours_MSEL )

add_subdirectory( vox_compute_contours_release )

add_subdirectory( vox_compute_contours_yuliang)

# Brown VOX algorithm to perform shock patch extraction
add_subdirectory( vox_extract_shock_patches )

# Brown VOX algorithm to perform shock graph matching
add_subdirectory( vox_match_shock_graphs )

# Brown VOX algorithm to perform shock patch detection
add_subdirectory( vox_detect_shock_patches )

# Brown VOX algorithm to perform evaluation of shock patch detection results
add_subdirectory( vox_eval_shock_patches )

# Brown VOX algorithm to perform visualization of shock patch matches
add_subdirectory( vox_visualize_patch_matches )


# Brown VOX algorithm to average two xgraphs
add_subdirectory( vox_average_two_xgraphs )

#Brown VOX algorithm to average n xgraphs
add_subdirectory( vox_average_n_xgraphs )

# Compute edit distance for shock graph which use dbsksp_xshock_graph representation
add_subdirectory( vox_xgraph_edit_distance )

add_subdirectory( vox_convert_esf_to_xgraph )


# Brown VOX algorithm to create an image pyramid
add_subdirectory( vox_octave_create_image_pyramid )

# Brown VOX algorithm to detect pb edges
add_subdirectory( vox_octave_pb_edge_detector )

# Brown VOX algorithm to link edges using Kovesi's algorithm
add_subdirectory( vox_octave_kovesi_edge_linker )

# Brown VOX algorithm to convert edg file to an image and orientation matrix file
add_subdirectory( vox_convert_edg_to_img_and_orient )

# Brown VOX algorithm to detect objects represented by shock graphs in images (pipeline version)
#add_subdirectory( vox_detect_xgraph2 )

# Brown VOX algorithm to train xgraph geometry and ccm models
add_subdirectory(vox_associate_groundtruth_xgraphs)
add_subdirectory(vox_train_xgraph_ccm_model)
add_subdirectory(vox_train_xgraph_geom_model)
add_subdirectory(vox_assoc_geom_and_ccm_model)

# Brown VOX algorithm to perform contour evaluation
add_subdirectory(vox_contour_evaluation)

# Brown VOX algorithm to compute xgraphs from ground truth silhouette
add_subdirectory(vox_compute_xgraphs_from_silhou)

# Brown VOX algorithm to detect objects represented by shock graphs in images
add_subdirectory( vox_detect_xgraph3 )

# foreground edge refine from sequential frames
# disabled for its dependency on gregistration and vidl1
# enable and adapt if you need it
# add_subdirectory(vox_multi_frame_fg_edge_refine )

# generate shapes based on trained geometric model
add_subdirectory(vox_generate_shape )

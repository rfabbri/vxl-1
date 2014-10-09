# This is brcv/rec/dborl/algo/CMakeLists.txt
# The Development Brown Object Recognition Library - Specific Algorithms

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/b3p )
INCLUDE_DIRECTORIES( ${VXL_EXPAT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )


SET( dborl_algo_sources
   dborl_utilities.h dborl_utilities.cxx
   dborl_image_desc_parser.h        dborl_image_desc_parser.cxx
   dborl_category_info_parser.h     dborl_category_info_parser.cxx
   dborl_index_parser.h             dborl_index_parser.cxx
   dborl_algo_params.h              dborl_algo_params.cxx
   dborl_xio_image_description.h    dborl_xio_image_description.cxx 
   dborl_search_index_file.h        dborl_search_index_file.cxx
)

AUX_SOURCE_DIRECTORY(Templates dborl_algo_sources)

ADD_LIBRARY( dborl_algo STATIC ${dborl_algo_sources} )

TARGET_LINK_LIBRARIES( dborl_algo dborl expatpp dbsol bxml bpro1 dbxml) 

# Brown ORL Algo Testing
SUBDIRS( tests )

# Brown ORL Cluster Utilities via MPI
SUBDIRS( mpi )

# Brown ORL Algo Examples
SUBDIRS( examples )

# Brown VOX Edge detection Algos
SUBDIRS( edge )

# Brown VOX Edge Linking Algos to create boundary fragment maps
SUBDIRS( bnd )

# Brown VOX Intrinsic Shock Computation Algorithm to compute intrinsic shock from a boundary map
SUBDIRS ( vox_compute_ishock )

# Brown VOX binaries to evaluate various edge detectors and linkers
SUBDIRS ( vox_eval_edge_linkers )

# Brown VOX algorithm to detect objects represented by shock graphs in images
SUBDIRS( vox_detect_xgraph )

# Brown VOX algorithm to perform edge detection and edge linking
SUBDIRS( vox_compute_contours )

SUBDIRS( vox_compute_contours_release )

SUBDIRS( vox_compute_contours_yuliang)

# Brown VOX algorithm to perform shock patch extraction
SUBDIRS( vox_extract_shock_patches )

# Brown VOX algorithm to perform shock graph matching
SUBDIRS( vox_match_shock_graphs )

# Brown VOX algorithm to perform shock patch detection
SUBDIRS( vox_detect_shock_patches )

# Brown VOX algorithm to perform evaluation of shock patch detection results
SUBDIRS( vox_eval_shock_patches )

# Brown VOX algorithm to perform visualization of shock patch matches
SUBDIRS( vox_visualize_patch_matches )


# Brown VOX algorithm to average two xgraphs
SUBDIRS( vox_average_two_xgraphs )

#Brown VOX algorithm to average n xgraphs
SUBDIRS( vox_average_n_xgraphs )

# Compute edit distance for shock graph which use dbsksp_xshock_graph representation
SUBDIRS( vox_xgraph_edit_distance )

SUBDIRS( vox_convert_esf_to_xgraph )


# Brown VOX algorithm to create an image pyramid
SUBDIRS( vox_octave_create_image_pyramid )

# Brown VOX algorithm to detect pb edges
SUBDIRS( vox_octave_pb_edge_detector )

# Brown VOX algorithm to link edges using Kovesi's algorithm
SUBDIRS( vox_octave_kovesi_edge_linker )

# Brown VOX algorithm to convert edg file to an image and orientation matrix file
SUBDIRS( vox_convert_edg_to_img_and_orient )

# Brown VOX algorithm to detect objects represented by shock graphs in images (pipeline version)
#SUBDIRS( vox_detect_xgraph2 )

# Brown VOX algorithm to train xgraph geometry and ccm models
SUBDIRS(vox_associate_groundtruth_xgraphs)
SUBDIRS(vox_train_xgraph_ccm_model)
SUBDIRS(vox_train_xgraph_geom_model)
SUBDIRS(vox_assoc_geom_and_ccm_model)

# Brown VOX algorithm to perform contour evaluation
SUBDIRS(vox_contour_evaluation)

# Brown VOX algorithm to compute xgraphs from ground truth silhouette
SUBDIRS(vox_compute_xgraphs_from_silhou)

# Brown VOX algorithm to detect objects represented by shock graphs in images
SUBDIRS( vox_detect_xgraph3 )

# foreground edge refine from sequential frames
SUBDIRS(vox_multi_frame_fg_edge_refine )

# generate shapes based on trained geometric model
SUBDIRS(vox_generate_shape )

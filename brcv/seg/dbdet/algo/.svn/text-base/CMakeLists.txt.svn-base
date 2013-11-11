# brcv/seg/dbdet/algo/CMakeLists.txt
# Algorithms for Brown Detection Libraries


SET( dbdet_algo_sources

# I/O functions
  dbdet_load_edg.cxx           dbdet_load_edg.h
  dbdet_cem_file_io.cxx        dbdet_cem_file_io.h
  
# Edge Detection Algorithms 
  dbdet_nms.h                     dbdet_nms.cxx
  dbdet_zc_det.h                  dbdet_zc_det.cxx
  dbdet_subpix_convolution.h
  dbdet_edge_uncertainty_measure.h          dbdet_edge_uncertainty_measure.cxx
  
  dbdet_gen_edge_det.h
  dbdet_interp_edge_det.h 
  dbdet_third_order_edge_det.h              dbdet_third_order_edge_det.cxx
  dbdet_third_order_color_detector.cxx      dbdet_third_order_color_detector.h
  
  dbdet_BS.h                                dbdet_BS.cxx
  dbdet_EMD.h                               dbdet_EMD.cxx
  
  dbdet_savitzky_golay_filter_2d.h
  dbdet_compass_edge_detector.h             dbdet_compass_edge_detector.cxx
  
  dbdet_compass_color_edge_detector.h       dbdet_compass_color_edge_detector.cxx
  
  dbdet_edge_appearance_util.h              dbdet_edge_appearance_util.cxx
  
# Edge Linking Algorithms
  dbdet_generic_linker.cxx     dbdet_generic_linker.h
  
  dbdet_sel_base.cxx           dbdet_sel_base.h 
  dbdet_sel_base_DHT_algo.cxx
  dbdet_sel_knot.h
  dbdet_sel_base_knot_algo.cxx
  dbdet_sel_base_CFTG_algo.cxx
  dbdet_sel.cxx                dbdet_sel.h               dbdet_sel_sptr.h
  dbdet_cvlet_map_io.cxx       dbdet_cvlet_map_io.h
  
  #dbdet_med.cxx               dbdet_med.h
  
  dbdet_postprocess_contours.cxx            dbdet_postprocess_contours.h
  
# IO utilities
  dbdet_convert_edgemap.h                   dbdet_convert_edgemap.cxx
  dbdet_screenshot.h                        dbdet_screenshot.cxx

# Evaluation Algorithms
  dbdet_eval_edge_det.cxx     dbdet_eval_edge_det.h
  
# Utilities
  dbdet_resize_edgemap.h      dbdet_resize_edgemap.cxx
)

IF(OCTAVE_FOUND)
    SET(dbdet_algo_sources
        ${dbdet_algo_sources}
        dbdet_pb_edge_detector.h dbdet_pb_edge_detector_embedded_files.h dbdet_pb_edge_detector.cxx dbdet_pb_edge_detector_embedded_files.cxx
        dbdet_kovesi_edge_linker.h dbdet_kovesi_edge_linker_embedded_files.h dbdet_kovesi_edge_linker.cxx dbdet_kovesi_edge_linker_embedded_files.cxx
    )
ENDIF(OCTAVE_FOUND)

AUX_SOURCE_DIRECTORY(Templates dbdet_algo_sources)

ADD_LIBRARY(dbdet_algo ${dbdet_algo_sources})
TARGET_LINK_LIBRARIES( dbdet_algo dbdet )

IF(OCTAVE_FOUND)
    TARGET_LINK_LIBRARIES( dbdet_algo dbil_algo)
ENDIF(OCTAVE_FOUND)

IF ( Boost_FOUND )
  TARGET_LINK_LIBRARIES( dbdet_algo ${Boost_LIBRARIES} )
ENDIF( Boost_FOUND )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )


# contrib/amir/cmd/CMakeLists.txt
# Command line programs for amir


INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

ADD_EXECUTABLE(sel_cmdline sel_cmdline.cxx)
TARGET_LINK_LIBRARIES(sel_cmdline dbgl_algo dbdet pdf1d)

ADD_EXECUTABLE(sel_evaluate_accuracy sel_evaluate_accuracy.h sel_evaluate_accuracy.cxx)
TARGET_LINK_LIBRARIES(sel_evaluate_accuracy dbgl_algo dbdet pdf1d)

ADD_EXECUTABLE(sel_examine_params sel_examine_params.h sel_examine_params.cxx)
TARGET_LINK_LIBRARIES(sel_examine_params vnl vnl_io vnl_algo vbl vul vil vil_algo vsol vtol vdgl dbgl_algo dbdet pdf1d )

ADD_EXECUTABLE(sel_batch_compute_contourmap sel_batch_compute_contourmap.h sel_batch_compute_contourmap.cxx)
TARGET_LINK_LIBRARIES(sel_batch_compute_contourmap vnl vnl_io vnl_algo vbl vul vil vil_algo vsol vtol vdgl dbgl_algo dbdet)

ADD_EXECUTABLE(amir_3o amir.cxx)
TARGET_LINK_LIBRARIES(amir_3o vnl vnl_io vnl_algo vbl vul vil vil_algo vsol vtol vdgl dbgl_algo dbdet)

ADD_EXECUTABLE(amir_generic amir_generic.cxx)
TARGET_LINK_LIBRARIES(amir_generic vnl vnl_io vnl_algo vbl vul vil vil_algo vsol vtol vdgl dbgl_algo dbdet)

ADD_EXECUTABLE(amir_ms amir_ms.cxx)
TARGET_LINK_LIBRARIES(amir_ms vnl vnl_io vnl_algo vbl vul vil vil_algo vsol vtol vdgl dbgl_algo dbdet)



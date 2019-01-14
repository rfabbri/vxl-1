# contrib/amir/cmd/CMakeLists.txt
# Command line programs for amir


include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )

add_executable(sel_cmdline sel_cmdline.cxx)
target_link_libraries(sel_cmdline dbgl_algo dbdet pdf1d)

add_executable(sel_evaluate_accuracy sel_evaluate_accuracy.h sel_evaluate_accuracy.cxx)
target_link_libraries(sel_evaluate_accuracy dbgl_algo dbdet pdf1d)

add_executable(sel_examine_params sel_examine_params.h sel_examine_params.cxx)
target_link_libraries(sel_examine_params vnl vnl_io vnl_algo vbl vul vil vil_algo vsol vtol vdgl dbgl_algo dbdet pdf1d )

add_executable(sel_batch_compute_contourmap sel_batch_compute_contourmap.h sel_batch_compute_contourmap.cxx)
target_link_libraries(sel_batch_compute_contourmap vnl vnl_io vnl_algo vbl vul vil vil_algo vsol vtol vdgl dbgl_algo dbdet)

add_executable(amir_3o amir.cxx)
target_link_libraries(amir_3o vnl vnl_io vnl_algo vbl vul vil vil_algo vsol vtol vdgl dbgl_algo dbdet)

add_executable(amir_generic amir_generic.cxx)
target_link_libraries(amir_generic vnl vnl_io vnl_algo vbl vul vil vil_algo vsol vtol vdgl dbgl_algo dbdet)

add_executable(amir_ms amir_ms.cxx)
target_link_libraries(amir_ms vnl vnl_io vnl_algo vbl vul vil vil_algo vsol vtol vdgl dbgl_algo dbdet)



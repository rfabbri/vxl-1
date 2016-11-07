include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${BOOST_INCLUDE_PATH} )
include_directories( ${ANN_INCLUDE_PATH} )
link_directories( ${BOOST_LIBRARY_PATH} )


add_executable(test_threadpool test_threadpool.cxx)
target_link_libraries(test_threadpool boost_thread)

link_directories( ${ANN_LIBRARY_PATH} )
add_executable(test_ann_sample ann_sample.cpp)
target_link_libraries(test_ann_sample ANN)

add_executable(test_nn_dist_trans test_nn_dist_trans.cxx)
target_link_libraries(test_nn_dist_trans dbdet_levelset2d_segmentation)

add_executable(test_levelset2d_elbow test_levelset2d.cxx)
target_link_libraries(test_levelset2d_elbow dbdet_levelset2d_segmentation)
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BOOST_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${ANN_INCLUDE_PATH} )
LINK_DIRECTORIES( ${BOOST_LIBRARY_PATH} )


ADD_EXECUTABLE(test_threadpool test_threadpool.cxx)
TARGET_LINK_LIBRARIES(test_threadpool boost_thread)

LINK_DIRECTORIES( ${ANN_LIBRARY_PATH} )
ADD_EXECUTABLE(test_ann_sample ann_sample.cpp)
TARGET_LINK_LIBRARIES(test_ann_sample ANN)

ADD_EXECUTABLE(test_nn_dist_trans test_nn_dist_trans.cxx)
TARGET_LINK_LIBRARIES(test_nn_dist_trans dbdet_levelset2d_segmentation)

ADD_EXECUTABLE(test_levelset2d_elbow test_levelset2d.cxx)
TARGET_LINK_LIBRARIES(test_levelset2d_elbow dbdet_levelset2d_segmentation)
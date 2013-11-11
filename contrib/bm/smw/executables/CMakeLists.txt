#This is contrib/bm/smw/executables/CMakeLists.txt

INCLUDE_DIRECTORIES( ${smw_SOURCE_DIR} )

ADD_EXECUTABLE( westin_experiment_1 westin_experiment_1.cxx )

ADD_EXECUTABLE( bw_1 bw_1.cxx )

TARGET_LINK_LIBRARIES( westin_experiment_1 smw vil vul )

TARGET_LINK_LIBRARIES( bw_1 smw vil vul )

ADD_EXECUTABLE( car_exp car_exp.cxx )

TARGET_LINK_LIBRARIES( car_exp smw vil vul )

ADD_EXECUTABLE( wind_turbine_blue wind_turbine_blue.cxx )

TARGET_LINK_LIBRARIES( wind_turbine_blue smw vil vul )

ADD_EXECUTABLE( wt2 wt2.cxx )

TARGET_LINK_LIBRARIES( wt2 smw vil vul  )

ADD_EXECUTABLE(around_the_block around_the_block.cxx)

TARGET_LINK_LIBRARIES(around_the_block smw vil vul)



#This is contrib/bm/smw/executables/CMakeLists.txt

include_directories( ${smw_SOURCE_DIR} )

add_executable( westin_experiment_1 westin_experiment_1.cxx )

add_executable( bw_1 bw_1.cxx )

target_link_libraries( westin_experiment_1 smw vil vul )

target_link_libraries( bw_1 smw vil vul )

add_executable( car_exp car_exp.cxx )

target_link_libraries( car_exp smw vil vul )

add_executable( wind_turbine_blue wind_turbine_blue.cxx )

target_link_libraries( wind_turbine_blue smw vil vul )

add_executable( wt2 wt2.cxx )

target_link_libraries( wt2 smw vil vul  )

add_executable(around_the_block around_the_block.cxx)

target_link_libraries(around_the_block smw vil vul)



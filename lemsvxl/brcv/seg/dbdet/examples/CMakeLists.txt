

add_executable( contourtracing_command contourtracing_command.cxx )
target_link_libraries( contourtracing_command dbdet bdgl )


add_executable( screenshot_cfg_command screenshot_cfg_command.cxx )
target_link_libraries( screenshot_cfg_command dbdet_algo )

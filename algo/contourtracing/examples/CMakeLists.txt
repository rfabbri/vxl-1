# This is algo/contourtracing/examples/CMakeLists.txt

link_libraries( contourtracing vil bdgl)
add_executable(contourtracing_command contourtracing_command.cxx )

link_libraries( contourtracing dbil_algo vil )
add_executable(cmd cmd.cxx )

#link_libraries( bapl vul )
#add_executable(make_pyramids bapl_make_pyramids.cxx )

#link_libraries( bapl vul ipts )
#add_executable(draw_keypoints bapl_draw_keypoints.cxx )

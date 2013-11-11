# This is algo/contourtracing/examples/CMakeLists.txt

LINK_LIBRARIES( contourtracing vil bdgl)
ADD_EXECUTABLE(contourtracing_command contourtracing_command.cxx )

LINK_LIBRARIES( contourtracing dbil_algo vil )
ADD_EXECUTABLE(cmd cmd.cxx )

#LINK_LIBRARIES( bapl vul )
#ADD_EXECUTABLE(make_pyramids bapl_make_pyramids.cxx )

#LINK_LIBRARIES( bapl vul ipts )
#ADD_EXECUTABLE(draw_keypoints bapl_draw_keypoints.cxx )

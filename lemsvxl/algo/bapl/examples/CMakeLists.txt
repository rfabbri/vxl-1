# This is algo/bapl/examples/CMakeLists.txt

link_libraries( bapl rrel vul )
add_executable(lowe_recognizer bapl_lowe_recognizer.cxx )

link_libraries( bapl vul )
add_executable(make_pyramids bapl_make_pyramids.cxx )

link_libraries( bapl vul ipts )
add_executable(draw_keypoints bapl_draw_keypoints.cxx )

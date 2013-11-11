# This is algo/bapl/examples/CMakeLists.txt

LINK_LIBRARIES( bapl rrel vul )
ADD_EXECUTABLE(lowe_recognizer bapl_lowe_recognizer.cxx )

LINK_LIBRARIES( bapl vul )
ADD_EXECUTABLE(make_pyramids bapl_make_pyramids.cxx )

LINK_LIBRARIES( bapl vul ipts )
ADD_EXECUTABLE(draw_keypoints bapl_draw_keypoints.cxx )

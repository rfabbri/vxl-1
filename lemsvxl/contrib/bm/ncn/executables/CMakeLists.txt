# This is /contrib/bm/ncn/executables/CMakeLists.txt

add_executable( exec1 exec1.cxx )
target_link_libraries( exec1 ncn )

add_executable( test_vidl test_vidl.cxx )
target_link_libraries( test_vidl vidl vil )

add_executable( image_sample_example image_sample_example.cxx )
target_link_libraries( image_sample_example vidl vil )

add_executable( ncn_westin_exp1 ncn_westin_exp1.cxx )
target_link_libraries( ncn_westin_exp1 ncn )
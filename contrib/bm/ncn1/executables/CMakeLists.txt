include_directories( ${BRL_INCLUDE_DIR}/lemsvxlsrc/contrib/bm/dbcl )

add_executable( vidl_exec vidl_exec.cxx )
target_link_libraries( vidl_exec vidl vil )

add_executable( westin1 westin1.cxx )
target_link_libraries( westin1 vidl vil ncn1 dbcl )

add_executable( crop_image_sequence crop_image_sequence.cxx )
target_link_libraries( crop_image_sequence vil vidl vul )

add_executable( temporal_entropy temporal_entropy.cxx )
target_link_libraries( temporal_entropy vil vidl vul ncn1 )

add_executable( westin_full westin_full.cxx )
target_link_libraries( westin_full vil vidl vul ncn1 )

add_executable( expat_test expat_test.cxx )
target_link_libraries( expat_test vil vul ncn1 )
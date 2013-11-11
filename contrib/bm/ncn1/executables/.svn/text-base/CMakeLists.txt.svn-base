INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/lemsvxlsrc/contrib/bm/dbcl )

ADD_EXECUTABLE( vidl_exec vidl_exec.cxx )
TARGET_LINK_LIBRARIES( vidl_exec vidl vil )

ADD_EXECUTABLE( westin1 westin1.cxx )
TARGET_LINK_LIBRARIES( westin1 vidl vil ncn1 dbcl )

ADD_EXECUTABLE( crop_image_sequence crop_image_sequence.cxx )
TARGET_LINK_LIBRARIES( crop_image_sequence vil vidl vul )

ADD_EXECUTABLE( temporal_entropy temporal_entropy.cxx )
TARGET_LINK_LIBRARIES( temporal_entropy vil vidl vul ncn1 )

ADD_EXECUTABLE( westin_full westin_full.cxx )
TARGET_LINK_LIBRARIES( westin_full vil vidl vul ncn1 )

ADD_EXECUTABLE( expat_test expat_test.cxx )
TARGET_LINK_LIBRARIES( expat_test vil vul ncn1 )
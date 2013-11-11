INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/yong )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )


SET(yong_cplusplusclass_sources
 my_rectangle.h
 my_rectangle.cxx
 my_rectangle_sptr.h
 yong_star.h
 yong_star.cxx
 yong_star_sptr.h
 my_rectangle_main.cxx
 vgui_image_display_test1_main.cxx
 ofstream_stringstream_test_main.cxx
 ofstream_fcopy_test_main.cxx
 string_eng_chs_dictionary_main.cxx
 bit_operation_test_main.cpp
 vil_image_test_main.cxx
 vgui_block_pyrimad_image_test_main.cxx
 mygui_yong_star_tableau.h
 mygui_yong_star_tableau.cxx
 mygui_yong_star_tableau_sptr.h 
 mygui_yong_star_tableau2.h
 mygui_yong_star_tableau2.cxx
 mygui_yong_star_tableau2_sptr.h 
 mygui_yong_star_test_main.cxx
 vul_practice_main.cxx
 yong_image_icon_view.h
 io_yong_image_icon_view.h
 yong_image_icon_view.cxx
 yong_image_icon_view_sptr.h 
 yong_image_icon_view_test_main.cxx
 database_value_test.cxx
 database_tuple_test.cxx
 database_relation_test.cxx
 dbpro_database_manager_memoryleak_test_main.cxx
)


AUX_SOURCE_DIRECTORY(Templates yong_cplusplusclass_sources)

ADD_EXECUTABLE(yong_cplusplusclass ${yong_cplusplusclass_sources})

TARGET_LINK_LIBRARIES(yong_cplusplusclass vnl vsol vgl vgui vnl_algo vil_algo vul vsl vil_io dbpro vidpro)


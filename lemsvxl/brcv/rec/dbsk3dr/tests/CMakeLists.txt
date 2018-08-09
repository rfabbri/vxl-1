# brcv/rec/dbsk3dr/tests/CMakeLists.txt


add_executable( dbsk3dr_test_all
      test_driver.cxx
      dbsk3dr_test_str_hypg.cxx
      dbsk3dr_test_dpmatch.cxx
      
      test_simple_graph_match.cxx
      test_sub_graph_match.cxx
      test_gradasn_scaffold_matching.cxx
      test_rigid_trans.cxx
)

target_link_libraries( dbsk3dr_test_all dbsk3dr dbasn dbsk3d dbmsh3d dbmsh3dr testlib )

add_test( NAME  dbsk3dr_test_str_hypg COMMAND $<TARGET_FILE:dbsk3dr_test_all> dbsk3dr_test_str_hypg.cxx )

add_test( NAME  dbsk3dr_test_dpmatch COMMAND $<TARGET_FILE:dbsk3dr_test_all> dbsk3dr_test_dpmatch.cxx )

add_test( NAME  dbsk3dr_simple_graph_match COMMAND $<TARGET_FILE:dbsk3dr_test_all> dbsk3dr_simple_graph_match.cxx )

add_test( NAME  dbsk3dr_sub_graph_match COMMAND $<TARGET_FILE:dbsk3dr_test_all> dbsk3dr_sub_graph_match.cxx ${CMAKE_CURRENT_SOURCE_DIR}/data ) 

add_test( NAME  dbsk3dr_test_rigid_trans COMMAND $<TARGET_FILE:dbsk3dr_test_all> test_rigid_trans.cxx )

add_test( NAME  dbsk3dr_test_shock_match COMMAND $<TARGET_FILE:dbsk3dr_test_all> dbsk3dr_test_shock_match ${CMAKE_CURRENT_SOURCE_DIR}/data ) 


add_executable( dbsk3dr_test_include test_include.cxx )
target_link_libraries( dbsk3dr_test_include dbasn dbmsh3dr dbsk3dr )



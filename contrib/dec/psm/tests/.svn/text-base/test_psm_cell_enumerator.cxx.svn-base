#include <testlib/testlib_test.h>

#include <vul/vul_file.h>

#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_cell_enumerator.h>

static void test_psm_cell_enumerator()
{
  START("psm_cell_enumerator test");

  vcl_string storage_dir("./test_blocks");
  vul_file::make_directory(storage_dir);
  psm_scene<PSM_APM_SIMPLE_GREY> scene(vgl_point_3d<double>(0,0,0), 1.0, storage_dir, bgeo_lvcs_sptr());
  scene.init_block(vgl_point_3d<int>(0,0,0),2);
  scene.init_block(vgl_point_3d<int>(1,0,0),2);
  scene.init_block(vgl_point_3d<int>(0,1,0),2);
  psm_cell_enumerator cell_enum(scene);
  
  TEST_EQUAL("size() returns correct value", cell_enum.size(), 64*3);

  vgl_point_3d<int> block_idx;
  hsds_fd_tree_node_index<3> cell_idx;

  bool found_cell1 = cell_enum.cell_at(100, block_idx, cell_idx);
  TEST("cell_at returns true", found_cell1,true);
  
  unsigned int index;
  bool found_cell2 = cell_enum.index_of(block_idx, cell_idx, index);
  TEST("index_of returns true", found_cell2, true);

  TEST_EQUAL("cell_at and index_of consistency",index,100);


 
  return;
}



TESTMAIN( test_psm_cell_enumerator );

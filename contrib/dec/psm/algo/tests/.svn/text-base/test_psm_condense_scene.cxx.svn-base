#include <testlib/testlib_test.h>

#include <vgl/vgl_point_3d.h>

#include <hsds/hsds_fd_tree_node_index.h>
#include <hsds/hsds_fd_tree.h>

#include <psm/psm_apm_traits.h>
#include <psm/psm_sample.h>
#include <psm/psm_scene.h>

#include "../psm_condense_scene.h"

static void test_psm_condense_scene()
{
  START("psm_condense_scene test");

  // create a scene
  vcl_string storage_dir("./psm_condense_scene_test_data");
  vul_file::make_directory(storage_dir);
  psm_scene<PSM_APM_SIMPLE_GREY> scene(vgl_point_3d<double>(0,0,0), 1.0, storage_dir, bgeo_lvcs_sptr(), 3);
  // set all cell to zero alpha
  scene.init_block(vgl_point_3d<int>(0,0,0),3,0.0f);
  // make first cell occupied
  hsds_fd_tree<psm_sample<PSM_APM_SIMPLE_GREY>,3> &block = scene.get_block(vgl_point_3d<int>(0,0,0));

  hsds_fd_tree_node_index<3> cell0_index(0,3);
  block[cell0_index].alpha = 10000.0;

  unsigned int ncells_begin = block.size();
  psm_condense_scene(scene, 0.01f, 0);

  // examine remaining cells
  unsigned int ncells_end = block.size();

  vcl_cout << "ncells_begin = " << ncells_begin << "  ncells_end = " << ncells_end << vcl_endl;
  TEST_EQUAL("cells condensed to correct number",ncells_end,22);

  return;
}




TESTMAIN( test_psm_condense_scene );


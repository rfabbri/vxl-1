//:
// \file
// \author Isabel Restrepo
// \date 23-Aug-2010

#include <testlib/testlib_test.h>
#include "test_utils.h"

#include <dbrec3d/dbrec3d_models.h>
#include <dbrec3d/dbrec3d_pairwise_compositor.h>

#include <bsta/bsta_gauss_id3.h>

#include <vgl/algo/vgl_rotation_3d.h>

#include <boxm/algo/boxm_init_scene.h>

#include <bvpl/kernels/bvpl_edge3d_kernel_factory.h>

typedef dbrec3d_pairwise_compositor<dbrec3d_pairwise_gaussian_model> T_comp;

void test_find_primitive_pairs()
{
  dbrec3d_test_utils::clean_up();
  
  bvpl_edge3d_kernel_factory kernels_3d(-1,0,-2,1,0, 0);
  kernels_3d.set_rotation_axis(vnl_float_3(0,1,0));
  kernels_3d.set_angle(0.0f);
  bvpl_kernel_sptr kernel1 = new bvpl_kernel(kernels_3d.create());
  kernel1->print();
  vcl_string scene_path = "part1_scene";
  int context_id1 = dbrec3d_test_utils::find_primitive_parts(kernel1,scene_path );
  
  DATABASE->print();
  
  kernels_3d.set_rotation_axis(vnl_float_3(1,0,0));
  kernels_3d.set_angle(0.0f);
  bvpl_kernel_sptr kernel2 = new bvpl_kernel(kernels_3d.create());
  kernel2->print();
  scene_path = "part2_scene";
  int context_id2 = dbrec3d_test_utils::find_primitive_parts(kernel2, scene_path);

  DATABASE->print();

 
  //composition model
  bsta_gauss_id3 dist_model(vnl_vector_fixed<double,3>(2.5, -5, 0),vnl_vector_fixed<double,3>(2.5,2.5,2.5));
  dbrec3d_pairwise_gaussian_model gauss_model(dist_model);
  
  //retrieve the primitive parts associated with each context.
  dbrec3d_context_sptr context1 = CONTEXT_MANAGER->get_context(context_id1);
  dbrec3d_context_sptr context2 = CONTEXT_MANAGER->get_context(context_id2);
  
  //unloads all active blocks
  context1->clean_memory();
  context2->clean_memory();
  
  dbrec3d_part_sptr part1 = context1->part();
  dbrec3d_part_sptr part2 = context2->part();

  //compositor  
  T_comp compositor(gauss_model);
  vnl_float_3 aux_axis;
  vgl_rotation_3d<float> pair_geometry = compositor.compute_rotation(part1->axis(), part1->aux_axis(), part2->axis(), part2->aux_axis(), aux_axis);

  vcl_vector<int> children;
  children.push_back(part1->type_id());
  children.push_back(part2->type_id());
  float max_azimuthal; float min_azimuthal; float max_polar; float min_polar;
  
  //create a composite part
  int composition_id = PARTS_MANAGER->new_composite_part(compositor, pair_geometry.axis(),aux_axis,  pair_geometry.angle(), vnl_float_3(0,0,0), true, false, 
                                                            children, vnl_float_3(1, 2, 2),max_azimuthal, min_azimuthal, max_polar, min_polar);
  
  
  //the search box is defined assuming center cell location is at 0,0,0. this is in unit cell coordinates
  double xmin=-1.0, ymin=0.0, zmin=-1.0, xmax=1.0, ymax =2.0, zmax =1.0;
  vgl_box_3d<double> search_box(xmin, ymin, zmin, xmax, ymax, zmax);
  
  //define the outupt scene
  unsigned max_level = 10;
  unsigned init_level = 1;
  boxm_scene<boct_tree<short, dbrec3d_pair_composite_instance > > *composition_scene = new boxm_scene<boct_tree<short, dbrec3d_pair_composite_instance > >(dbrec3d_test_utils::lvcs_, dbrec3d_test_utils::origin_, 
                                                                                                                                                       dbrec3d_test_utils::block_dim_, dbrec3d_test_utils::world_dim_,
                                                                                                                                                       max_level, init_level);
  vcl_string parts_scene_path("./");
  composition_scene->set_paths("./", "composition_scene");
  composition_scene->write_scene("/dbrec3d_composition_scene.xml");
  boxm_init_scene(*composition_scene);

  
  //detect instances of compositions
  compositor.detect_instances<dbrec3d_part_instance>(composition_id, part1, part2, composition_scene, search_box);

  //explore composition parts scene
  bool result = true;
  {
    boxm_cell_iterator<boct_tree<short, dbrec3d_pair_composite_instance > > iterator =
    composition_scene->cell_iterator(&boxm_scene<boct_tree<short, dbrec3d_pair_composite_instance > >::load_block);
    iterator.begin();
    
    while (!iterator.end()) {
      boct_tree_cell<short,dbrec3d_pair_composite_instance > *cell = *iterator;
      
      if(cell->data().type_id() >=0)
      {
        vgl_point_3d<double> loc = cell->data().location();
        if(vcl_abs(loc.x() - 7.50) > 1.0e-7)
          result = false;
        if(vcl_abs(loc.y() - 8.75) > 1.0e-7)
          result = false;
        if((loc.z()< 6.24) || (loc.z() > 13.76))
          result = false;
        if(vcl_abs(cell->data().posterior() - 0.0161) > 1.0e-3 )
          result = false;
        
        vcl_cout << " At cell located at: " << iterator.global_origin() << " \n Data: " << cell->data() << vcl_endl;
      }
      
      ++iterator;
    }
  }
  TEST("Find primitive_pairs", result, true);
  
  dbrec3d_test_utils::clean_up();

}


TESTMAIN(test_find_primitive_pairs);

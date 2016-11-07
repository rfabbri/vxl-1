//:
// \file
// \author Isabel Restrepo
// \date 13-Aug-2010

#include <testlib/testlib_test.h>
#include "test_utils.h"

#include <bvpl/kernels/bvpl_edge3d_kernel_factory.h>
#include <bvpl/functors/bvpl_positive_gauss_conv_functor.h>
#include <bvpl/bvpl_octree/bvpl_scene_kernel_operator.h>

#include <dbrec3d/dbrec3d_parts_manager.h>
#include <dbrec3d/dbrec3d_context_manager.h>
#include <dbrec3d/dbrec3d_models.h>
#include <dbrec3d/dbrec3d_vrml_visitor.h>

#define DEBUG

void test_non_maxima_suppression()
{
  dbrec3d_test_utils::clean_up();

  //create scene
  boxm_scene<boct_tree<short, bsta_num_obs<bsta_gauss_sf1> > > *scene = dbrec3d_test_utils::create_scene();
  
  // scene->print();
  //explore output scene
#ifdef DEBUG
  {
    boxm_cell_iterator<boct_tree<short, bsta_num_obs<bsta_gauss_sf1 > > > iterator = scene->cell_iterator(&boxm_scene<boct_tree<short, bsta_num_obs<bsta_gauss_sf1 > > >::load_block);
    iterator.begin();
    
    while (!iterator.end()) {
      boct_tree_cell<short,bsta_num_obs<bsta_gauss_sf1> > *cell = *iterator;
      //if(cell->data().mean() > 0.6)
      {
        boct_loc_code<short> code =cell->get_code();
        vcl_cout << " Code: " <<code<< vcl_endl;
        vcl_cout << " At cell located within tree: " <<code.get_point(3)<< vcl_endl;
        vcl_cout << " At cell located at: " << iterator.global_origin() << " Data: " << cell->data() << vcl_endl;

      }
      ++iterator;
    }
  }
#endif
 
  
  //run kernel
  bvpl_edge3d_kernel_factory kernels_3d(-1,0,-2,1,0, 0, scene->finest_cell_length());
  vnl_float_3 axis(1,0,0);
  kernels_3d.set_rotation_axis(axis);
  kernels_3d.set_angle(0.0f);
  bvpl_kernel_sptr kernel_sptr = new bvpl_kernel(kernels_3d.create());
  
  kernel_sptr->print();
  
  //create the output scenes
  boxm_scene<boct_tree<short,bsta_num_obs<bsta_gauss_sf1 > > >  *scene_out =
  new boxm_scene<boct_tree<short,bsta_num_obs<bsta_gauss_sf1> > >(scene->lvcs(), scene->origin(), scene->block_dim(), scene->world_dim(), scene->max_level(), scene->init_level());
  scene_out->set_paths("./", "response_scene");
  scene->clone_blocks(*scene_out, bsta_num_obs<bsta_gauss_sf1>(bsta_gauss_sf1(0.0f, 1.0f)));
  
  //operate on scene
  bvpl_positive_gauss_conv_functor functor;
  bvpl_scene_kernel_operator scene_oper;
  scene_oper.operate(*scene, functor, kernel_sptr, *scene_out);  

#ifdef DEBUG
  //explore  output tree
  {
    boxm_cell_iterator<boct_tree<short, bsta_num_obs<bsta_gauss_sf1 > > > iterator = scene_out->cell_iterator(&boxm_scene<boct_tree<short, bsta_num_obs<bsta_gauss_sf1 > > >::load_block);
    iterator.begin();
    
    while (!iterator.end()) {
      boct_tree_cell<short,bsta_num_obs<bsta_gauss_sf1> > *cell = *iterator;
      if(cell->data().mean() > 1e-7){
        vcl_cout << " At cell located at: " << iterator.global_origin() << " Data: " << cell->data() << vcl_endl;
      }
      ++iterator;
    }
  }
#endif
  
  //convert response scene to parts scene
  boxm_scene<boct_tree<short, dbrec3d_part_instance > > *parts_scene = 
  new boxm_scene<boct_tree<short, dbrec3d_part_instance > >(scene_out->lvcs(), scene_out->origin(), scene_out->block_dim(), scene_out->world_dim(), scene_out->max_level(), scene_out->init_level());
  vcl_string parts_scene_path("./");
  parts_scene->set_paths("./", "parts_scene");
  parts_scene->write_scene("/dbrec3d_parts_scene.xml");
  //convert the scene into a context
  dbrec3d_kernel_model model;
  int context_id = CONTEXT_MANAGER->register_context(scene_out, model, kernel_sptr, parts_scene);

#ifdef DEBUG
  //explore  parts scene
  {
    boxm_cell_iterator<boct_tree<short, dbrec3d_part_instance > > iterator = parts_scene->cell_iterator(&boxm_scene<boct_tree<short, dbrec3d_part_instance > >::load_block);
    iterator.begin();
    
    while (!iterator.end()) {
      boct_tree_cell<short,dbrec3d_part_instance > *cell = *iterator;
      if(cell->data().posterior() > 1e-7){
        vcl_cout << " At cell located at: " << iterator.global_origin() << " \n Data: " << cell->data() << vcl_endl;
      }
      ++iterator;
    }
  }
#endif
  
  //run non-maxima suppression
  dbrec3d_context_sptr context = CONTEXT_MANAGER->get_context(context_id);
  if(context)
    context->local_non_maxima_suppression();
  
  //explore  parts scene 
  vcl_ofstream os("./test.vrml");
  dbrec3d_vrml_visitor vrml_vis(os);
  vgl_box_3d<double> bb = parts_scene->get_world_bbox();
  vrml_vis.write_box(bb);

  bool result = true;
  {
    boxm_cell_iterator<boct_tree<short, dbrec3d_part_instance > > iterator = parts_scene->cell_iterator(&boxm_scene<boct_tree<short, dbrec3d_part_instance > >::load_block);
    iterator.begin();
    
    unsigned num_parts = 0;
    while (!iterator.end()) {
      boct_tree_cell<short,dbrec3d_part_instance > *cell = *iterator;
      if(cell->data().type_id() >=0){
        vcl_cout << " At cell located at: " << iterator.global_origin() << " \n Data: " << cell->data() << vcl_endl;
        if(vcl_abs(cell->data().posterior()- 0.7)>1e-7) result = false;
        //vrml_vis.visit(&cell->data());
        num_parts++;
      }
      ++iterator;
    }
    if(num_parts != 4) result =false;
  }
  TEST("Valid Test", result, true);
  
  dbrec3d_test_utils::clean_up();
  
}


TESTMAIN(test_non_maxima_suppression);

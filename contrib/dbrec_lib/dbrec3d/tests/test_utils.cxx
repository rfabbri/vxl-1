#include "test_utils.h"
//:
// \file
// \author Isabel Restrepo
// \date 15-Aug-2010

#include <bvpl/kernels/bvpl_edge3d_kernel_factory.h>
#include <bvpl/functors/bvpl_positive_gauss_conv_functor.h>
#include <bvpl/bvpl_octree/bvpl_scene_kernel_operator.h>

#include <dbrec3d/dbrec3d_parts_manager.h>
#include <dbrec3d/dbrec3d_context_manager.h>
#include <dbrec3d/dbrec3d_models.h>
#include <dbrec3d/dbrec3d_pairwise_compositor.h>

#include <bsta/bsta_gauss_id3.h>

#include <vgl/algo/vgl_rotation_3d.h>

#include <boxm/algo/boxm_init_scene.h>

bgeo_lvcs dbrec3d_test_utils::lvcs_(33.33,44.44,10.0, bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
vgl_point_3d<double> dbrec3d_test_utils::origin_(0,0,0);
vgl_vector_3d<double> dbrec3d_test_utils::block_dim_(10, 10, 10); //world coordinate dimensions of a block
vgl_vector_3d<unsigned> dbrec3d_test_utils::world_dim_(2,2,2); //number of blocks in a scene


//crete the input scene
void dbrec3d_test_utils::init_tree(boct_tree<short,bsta_num_obs<bsta_gauss_sf1> > *tree, unsigned i)
{
  tree-> split(); //now we have 8 cells
  vcl_vector<boct_tree_cell<short,bsta_num_obs<bsta_gauss_sf1> >*> leaves = tree->leaf_cells();
  leaves[i]->set_data(bsta_num_obs<bsta_gauss_sf1>(bsta_gauss_sf1(0.9f, 0.04f)));
  leaves[i]->split();
}

boxm_scene<boct_tree<short, bsta_num_obs<bsta_gauss_sf1> > >* dbrec3d_test_utils::create_scene()
{
  unsigned int max_tree_level = 8;
  unsigned int init_level = 1;
  boxm_scene<boct_tree<short, bsta_num_obs<bsta_gauss_sf1> > > *scene = 
  new boxm_scene<boct_tree<short, bsta_num_obs<bsta_gauss_sf1> > >(lvcs_, origin_, block_dim_, world_dim_, max_tree_level, init_level );
  vcl_string scene_path("./");
  scene->set_paths(scene_path, "test_scene");
  
  unsigned cell_index = 7;
  boxm_block_iterator<boct_tree<short, bsta_num_obs<bsta_gauss_sf1> > > iter=scene->iterator();
  iter.begin();
  while(!iter.end())
  {
    scene->load_block(iter.index());
    boxm_block<boct_tree<short, bsta_num_obs<bsta_gauss_sf1> > > *block = scene->get_active_block();
    // Construct an empty tree with 3 maximum levels 1 levele initialized to 0.0
    boct_tree<short,bsta_num_obs<bsta_gauss_sf1> > *tree = new boct_tree<short,bsta_num_obs<bsta_gauss_sf1> >(bsta_num_obs<bsta_gauss_sf1>(bsta_gauss_sf1(0.2f, 0.04f)), max_tree_level, init_level);
    //tree->print();
    init_tree(tree, cell_index);
    //tree->print();
    block->init_tree(tree);
    scene->write_active_block();
    if(cell_index == 0) cell_index = 7;
    cell_index--;
    ++iter;
  }
  return scene;
}

void dbrec3d_test_utils::clean_up()
{
  //clean temporary files
  vul_file_iterator file_it("./*.bin");
  for (; file_it; ++file_it)
  {
    vpl_unlink(file_it());
    vul_file::delete_file_glob(file_it());
  }
  
  brdb_database_manager::clear_all();
  
}


//: creates a scene, runs a kernel and returns the id of the context containing non-maximally suppressed parts
int dbrec3d_test_utils::find_primitive_parts(bvpl_kernel_sptr kernel_sptr, vcl_string part_scene_path)
{
  //create scene
  boxm_scene<boct_tree<short, bsta_num_obs<bsta_gauss_sf1> > > *scene = create_scene();
  
  //run kernel
  if(!kernel_sptr)
  {
    bvpl_edge3d_kernel_factory kernels_3d(-1,0,-2,1,0, 0);
    vnl_float_3 axis(1,0,0);
    kernels_3d.set_rotation_axis(axis);
    kernels_3d.set_angle(0.0f);
    kernel_sptr = new bvpl_kernel(kernels_3d.create());
  }
  
  kernel_sptr->set_voxel_length(scene->finest_cell_length());
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

  //convert response scene to parts scene
  boxm_scene<boct_tree<short, dbrec3d_part_instance > > *parts_scene = 
  new boxm_scene<boct_tree<short, dbrec3d_part_instance > >(scene_out->lvcs(), scene_out->origin(), scene_out->block_dim(), scene_out->world_dim(),scene->max_level(), scene->init_level());
  parts_scene->set_paths("./", part_scene_path);
  parts_scene->set_appearance_model(DBREC3D_PART_INSTANCE);
  parts_scene->write_scene(part_scene_path + ".xml");
  
  //convert the scene into a context
  dbrec3d_kernel_model model;
  int context_id = CONTEXT_MANAGER->register_context(scene_out, model, kernel_sptr, parts_scene);

  //run non-maxima suppression
  dbrec3d_context_sptr context = CONTEXT_MANAGER->get_context(context_id);
  if(context)
    context->local_non_maxima_suppression();
  
  //explore  parts scene
  {
    boxm_cell_iterator<boct_tree<short, dbrec3d_part_instance > > iterator = parts_scene->cell_iterator(&boxm_scene<boct_tree<short, dbrec3d_part_instance > >::load_block);
    iterator.begin();
    
    while (!iterator.end()) {
      boct_tree_cell<short,dbrec3d_part_instance > *cell = *iterator;
      
      if(cell->data().type_id() >=0)
        vcl_cout << " At cell located at: " << iterator.global_origin() << " \n Data: " << cell->data() << vcl_endl;

      ++iterator;
    }
  }
  return context_id;
}

int dbrec3d_test_utils::find_primitive_pairs()
{
  typedef dbrec3d_pairwise_compositor<dbrec3d_pairwise_gaussian_model> T_comp;

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
  float max_azimuthal=0; float min_azimuthal=0; float max_polar=0; float min_polar=0;
  
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
  composition_scene->set_appearance_model(DBREC3D_PAIR_COMPOSITE_INSTANCE);
  composition_scene->write_scene("dbrec3d_composition_scene.xml");
  boxm_init_scene(*composition_scene);
  
  
  //detect instances of compositions
  compositor.detect_instances<dbrec3d_part_instance>(composition_id, part1, part2, composition_scene, search_box);
  return 1;
}






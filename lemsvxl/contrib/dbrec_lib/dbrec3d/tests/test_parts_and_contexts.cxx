//:
// \file 
// \brief Testing conversion from a bvpl-response-scene to dbrec3d-parts-scene
// \author Isabel Restrepo
// \date June 1, 2010

#include <testlib/testlib_test.h>

#include <dbrec3d/dbrec3d_parts_manager.h>
#include <dbrec3d/dbrec3d_context_manager.h>
#include <dbrec3d/dbrec3d_models.h>


#include <bvpl/kernels/bvpl_edge3d_kernel_factory.h>
#include <bvpl/kernels/bvpl_create_directions.h>
#include <bvpl/functors/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/bvpl_octree/bvpl_scene_vector_operator.h>
#include <bvpl/bvpl_octree/bvpl_octree_vector_operator_impl.h>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vpl/vpl.h>
#include <vcl_memory.h>


static bool debug = false;

typedef boct_tree_cell<short, float> cell_type;

void fill_edge3d_tree(boct_tree<short,float>* tree)
{
  tree-> split();
  vcl_vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
  for (unsigned i= 0; i<4; i++)
  {
    leaves[i]->set_data(0.2f);
    leaves[i]->split();
    {
      boct_tree_cell<short,float>* cc =leaves[i]->children();
      for (unsigned j1=0; j1<8; j1++)
        cc[j1].split();
    }
    leaves[i+4]->set_data(0.9f);
    leaves[i+4]->split();
    {
      boct_tree_cell<short,float>* cc =leaves[i+4]->children();
      for (unsigned j1=0; j1<8; j1++)
        cc[j1].split();
    }
    //split children
    vcl_vector<boct_tree_cell<short,float>*> leaves = tree->leaf_cells();
  }
}

vcl_map<int,int> check_register_kernels(bvpl_kernel_vector_sptr kernel_vec, vcl_map <int, int> id_map)
{
 
  bool result = true;
  
  //test that parts in the database are the same as kernels
  vcl_map<int,int>::iterator map_it;
  for(map_it=id_map.begin(); map_it!=id_map.end(); map_it++)
  {
    int kernel_id = (*map_it).first;
    int part_id = (*map_it).second;
    bvpl_kernel_sptr kernel = kernel_vec->kernels_[kernel_id];
    dbrec3d_part_sptr part = PARTS_MANAGER->get_part(part_id);
    
    if( !(part->axis()== kernel->axis()) || !(part->angle()== kernel->angle()) )
    {
      result = false;
    }
  }
  
   TEST("Check register Kernels", result, true);
  return id_map;
  
}

bool test_parts_and_contexts()
{
  
  
  /********** 1. create/operate on scene ********************/
  
  
  //create the vector of kernels
  bvpl_edge3d_kernel_factory kernels_3d(-3,2,-3,2,-3,2);
  bvpl_create_directions_a dir;
  bvpl_kernel_vector_sptr kernel_vec = kernels_3d.create_kernel_vector(dir);
  
  
  //create functor
  bvpl_edge_geometric_mean_functor<float> functor;
  
 
  //create tree. this tree is initialized with 4 leaves as a regular grid
  boct_tree<short,float> *init_tree = new boct_tree<short,float>(4);
  fill_edge3d_tree(init_tree);
  
  short level = 0;
  double cell_length = 1.0/(double)(1<<(3 -level));
  
  //crete the input scene
  bgeo_lvcs lvcs(33.33,44.44,10.0, bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
  vgl_point_3d<double> origin(10,10,20);
  
  vgl_vector_3d<double> block_dim(1,1,1); //world coordinate dimensions of a block
  vgl_vector_3d<unsigned> world_dim(1,1,1); //number of blocks in a scene
  
  boxm_scene<boct_tree<short, float> > scene(lvcs, origin, block_dim, world_dim);
  vcl_string scene_path("./");
  scene.set_paths(scene_path, "scene_in");
  
  boxm_block_iterator<boct_tree<short, float> >  iter =scene.iterator();
  iter.begin();
  for (; !iter.end(); iter++) {
    scene.load_block(iter.index());
    boxm_block<boct_tree<short, float> > *block = scene.get_active_block();
    block->init_tree(init_tree->clone());
    scene.write_active_block();
  }
  
  delete init_tree;
  
  //create the output scenes
  boxm_scene<boct_tree<short,bvpl_octree_sample<float> > >  *scene_out = new boxm_scene<boct_tree<short,bvpl_octree_sample<float> > >(lvcs, origin, block_dim, world_dim);
  scene_out->set_paths(scene_path, "response_scene");
  
  //operate on scene
  bvpl_scene_vector_operator scene_oper;
  scene_oper.operate(scene, functor, kernel_vec, *scene_out);
  
/******************* 2. convert scene to a parts scene *******************************/
  
  //create parts scene
  boxm_scene<boct_tree<short, dbrec3d_part_instance > > *parts_scene = new boxm_scene<boct_tree<short, dbrec3d_part_instance > >(lvcs, origin, block_dim, world_dim);
  vcl_string parts_scene_path("./");
  parts_scene->set_paths(parts_scene_path, "parts_scene");
  
  //converte scene
  vcl_map <int, int> id_map;
  dbrec3d_kernel_model model;
  int context_id = CONTEXT_MANAGER->register_context(scene_out,model,kernel_vec,parts_scene,id_map);
  vcl_map<int,int>::iterator map_it;
  for(map_it=id_map.begin(); map_it!=id_map.end(); map_it++)
  {
    vcl_cout << (*map_it).first << ',' << (*map_it).second << vcl_endl;
   
  }
  
  if(context_id <  0)
    return false;
  
  check_register_kernels(kernel_vec, id_map);
  
  //dbrec3d_octree_context_util::dbrec3d_response_to_parts_scene(scene_out, id_map, parts_scene);
  
  /******************* 3. compare that info in the two scenes is the same **************/
  

  boxm_block_iterator<boct_tree<short,bvpl_octree_sample<float> > > iter_bvpl = scene_out->iterator();
  boxm_block_iterator<boct_tree<short,dbrec3d_part_instance > > iter_dbrec3d = parts_scene->iterator();

  iter_bvpl.begin();
  iter_dbrec3d.begin();
  bool result = true;
  for(; !iter_bvpl.end(); iter_bvpl++, iter_dbrec3d++)
  {
    scene_out->load_block(iter_bvpl.index());
    parts_scene->load_block(iter_dbrec3d.index());
    boct_tree<short,bvpl_octree_sample<float> >  *bvpl_tree = (*iter_bvpl)->get_tree();
    boct_tree<short,dbrec3d_part_instance >  *dbrec3d_tree = (*iter_dbrec3d)->get_tree();
    
   // bvpl_tree->print();
    //dbrec3d_tree->print();
    
    vcl_vector<boct_tree_cell<short,bvpl_octree_sample<float> >* > bvpl_cells = bvpl_tree->leaf_cells_at_level(level);  
    vcl_vector<boct_tree_cell<short,dbrec3d_part_instance >* > dbrec3d_cells = dbrec3d_tree->leaf_cells_at_level(level);  
    
    for( unsigned i = 0; i < bvpl_cells.size(); i++)
    {
      boct_tree_cell<short,bvpl_octree_sample<float> >* bvpl_cell = bvpl_cells[i];
      bvpl_octree_sample<float> octree_sample = bvpl_cell->data();
      boct_tree_cell<short,dbrec3d_part_instance >* part_cell = dbrec3d_cells[i];
      dbrec3d_part_instance part_ins = part_cell->data();
      
      if(octree_sample.id_ < 0 ) // probably a border
      {
        //vcl_cerr << "Warning negative id \n"; 
        continue;
      }
      
      dbrec3d_part_sptr part = PARTS_MANAGER->get_part(id_map[octree_sample.id_]);
      bvpl_kernel_sptr kernel= kernel_vec->kernels_[octree_sample.id_];
      
      //vcl_cout << " Octree sample: " << octree_sample << "\n" << "part: " << part << vcl_endl;
      //if( part_ins.location()> dbrec3d_tree->global_origin(part_cell)
      if( !(part->axis()== kernel->axis()) || !(part->angle()== kernel->angle()))
      {
        result = false;
      }
      
    }
  }
  TEST("Intrinsic properties in part scene are correct", result, true);
  
  /*************************** 4. Check responses **********************************/
  boxm_scene<boct_tree<short,float> >  *response_scene = CONTEXT_MANAGER->response_scene<dbrec3d_part_instance>(context_id);
  
  if(!response_scene)
    return false;
  
  
  boxm_block_iterator<boct_tree<short,float > > resp_it = response_scene->iterator();
  
  iter_bvpl.begin();
  resp_it.begin();
  result = true;
  for(; !iter_bvpl.end(); iter_bvpl++, resp_it++)
  {
    scene_out->load_block(iter_bvpl.index());
    response_scene->load_block(resp_it.index());
    boct_tree<short,bvpl_octree_sample<float> >  *bvpl_tree = (*iter_bvpl)->get_tree();
    boct_tree<short,float >  *resp_tree = (*resp_it)->get_tree();
    
    //bvpl_tree->print();
    //dbrec3d_tree->print();
    
    vcl_vector<boct_tree_cell<short,bvpl_octree_sample<float> >* > bvpl_cells = bvpl_tree->leaf_cells_at_level(level);  
    vcl_vector<boct_tree_cell<short,float>* > resp_cells = resp_tree->leaf_cells_at_level(level);  
    
    for( unsigned i = 0; i < bvpl_cells.size(); i++)
    {
      boct_tree_cell<short,bvpl_octree_sample<float> >* bvpl_cell = bvpl_cells[i];
      bvpl_octree_sample<float> octree_sample = bvpl_cell->data();
      boct_tree_cell<short,float >* resp_cell = resp_cells[i];
            
      if(octree_sample.id_ < 0 ) // probably a border
      {
        //vcl_cerr << "Warning negative id \n"; 
        continue;
      }
    
      if(vcl_abs(resp_cell->data() - octree_sample.response_)>1e-7)    
      {
        vcl_cerr << resp_cell->data() << ',' << octree_sample.response_ << vcl_endl;
        result = false;
      }
      
    }
  }
  
  TEST("Responses are equal", result, true);
 
  /*************************** 5. Clean up *****************************************/
  
  //clean temporary files
  vul_file_iterator file_it("./*.bin");
  for (; file_it; ++file_it)
  {
    vpl_unlink(file_it());
    vul_file::delete_file_glob(file_it());
  }
  
  return true;
}




TESTMAIN(test_parts_and_contexts);

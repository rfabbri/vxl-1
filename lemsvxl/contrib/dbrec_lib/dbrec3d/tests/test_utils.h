#ifndef test_utils_h_
#define test_utils_h_
//:
// \brief: A file containing utility functions for other tests
// \file
// \author Isabel Restrepo
// \date 13-Aug-2010

#include <boxm/boxm_scene.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bvpl/kernels/bvpl_kernel.h>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vpl/vpl.h>

#include <brdb/brdb_database_manager.h>

class dbrec3d_test_utils
{
public: 
  //: create a sample tree. refer to dbrec3d/docs for an image of this tree
  static void init_tree(boct_tree<short,bsta_num_obs<bsta_gauss_sf1> > *tree, unsigned i);
  
  //: create a sample scene. refer to dbrec3d/docs for an image of this scene
  static boxm_scene<boct_tree<short, bsta_num_obs<bsta_gauss_sf1> > >* create_scene();
  
  //: creates a scene, runs a kernel and returns the id of the context containing non-maximally suppressed parts
  static int find_primitive_parts(bvpl_kernel_sptr kernel_sptr=NULL, vcl_string part_scene_path = vcl_string("parts_scene"));
  
  //: composes primitives into pairs, returns the id of composition context
  static int find_primitive_pairs();
  
  //: deletes .bin files in the current directory
  static void clean_up();
  
  static bgeo_lvcs lvcs_;
  
  static vgl_point_3d<double> origin_;
  
  static vgl_vector_3d<double> block_dim_; //world coordinate dimensions of a block
  
  //number of blocks in a scene
  static vgl_vector_3d<unsigned> world_dim_; 
  
};





#endif
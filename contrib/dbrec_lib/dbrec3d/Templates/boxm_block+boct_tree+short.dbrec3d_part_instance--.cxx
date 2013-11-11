#include <dbrec3d/dbrec3d_part_instance.h>
#include <boct/boct_tree.h>
#include <boxm/boxm_block.txx>
#include <boxm/boxm_scene.txx>
#include <vbl/vbl_array_3d.txx>
#include <vcl_string.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>

//typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
//typedef boct_tree<short, dbrec3d_part_instance<gauss_type> > tree_type0;
typedef boct_tree<short, dbrec3d_part_instance> tree_type1;
typedef boct_tree<short, dbrec3d_pair_composite_instance> tree_type2;
//BOXM_BLOCK_INSTANTIATE(tree_type0);
//BOXM_BLOCK_ITERATOR_INSTANTIATE(tree_type0);
//BOXM_SCENE_INSTANTIATE(tree_type0);
//VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type0> *);

BOXM_BLOCK_INSTANTIATE(tree_type1);
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree_type1);
BOXM_SCENE_INSTANTIATE(tree_type1);
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type1> *);

BOXM_BLOCK_INSTANTIATE(tree_type2);
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree_type2);
BOXM_SCENE_INSTANTIATE(tree_type2);
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type2> *);
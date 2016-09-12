#include <dbrec3d/dbrec3d_part_instance.h>
#include <bvpl/bvpl_octree/sample/bvpl_octree_sample.h>
#include <boct/boct_tree.hxx>
#include <boct/boct_tree_cell.hxx>

BOCT_TREE_INSTANTIATE(short, dbrec3d_pair_composite_instance);
BOCT_TREE_INSTANTIATE(short, dbrec3d_part_instance);
BOCT_TREE_CELL_INSTANTIATE(short, dbrec3d_part_instance);
BOCT_TREE_CELL_INSTANTIATE(short, dbrec3d_pair_composite_instance);

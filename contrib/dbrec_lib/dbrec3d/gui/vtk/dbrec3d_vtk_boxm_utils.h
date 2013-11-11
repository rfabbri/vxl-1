// This is dbrec3d_vtk_boxm_utils.h
#ifndef dbrec3d_vtk_boxm_utils_h
#define dbrec3d_vtk_boxm_utils_h

//:
// \file
// \brief Function for vtk/boxm utilities
// \author Isabel Restrepo mir@lems.brown.edu
// \date  16-Nov-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vtkHyperOctree.h>

#include <boxm/boxm_scene.h>

//: A function to create a vtkHyperOctree from a boxm scene
vtkHyperOctree* dbrec3d_scene_to_hyperoctree(boxm_scene<boct_tree<short, float> > *scene);


//: A function to recursively clone boxm_cells into vtk octree
void dbrec3d_clone_leaves(vtkHyperOctreeCursor *cursor,
                     boct_tree_cell<short,float> *boxm_cell,
                     vtkHyperOctree *vtk_octree);
#endif

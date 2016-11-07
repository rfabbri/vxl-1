//:
// \file
// \author Isabel Restrepo
// \date 16-Nov-2010

#include "dbrec3d_vtk_boxm_utils.h"

//VTK includes 
#include <vtkDataArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkHyperOctreeCursor.h>

vtkHyperOctree* dbrec3d_scene_to_hyperoctree(boxm_scene<boct_tree<short, float> > *scene)
{
  int x_blocks, y_blocks, z_blocks = 0;
  scene->block_num(x_blocks,y_blocks,z_blocks);
  if(x_blocks*y_blocks*z_blocks != 1)
  {
    vcl_cout << "Only scenes with one block are supported for now" << vcl_endl;
    return NULL;
  }

  vtkHyperOctree* vtk_octree = vtkHyperOctree::New();
  
  //set dimensions
  vtk_octree->SetDimension((int)3);
  double x_lenght, y_length, z_length = 0;
  scene->axes_length(x_lenght, y_length, z_length);
  vtk_octree->SetSize(x_lenght, y_length, z_length);
  vtk_octree->SetOrigin(scene->origin().x(), scene->origin().y(), scene->origin().z());
  
  //fill-in data
  vtkDataArray *scalars=vtkDataArray::CreateDataArray(VTK_DOUBLE);
  scalars->SetNumberOfComponents(1);
  scalars->Allocate(scene->size());
  scalars->SetNumberOfTuples(1); // the root
  vtk_octree->GetLeafData()->SetScalars(scalars);
  //scalars->UnRegister(this); //decrease pointer count
  
  vtkHyperOctreeCursor *cursor=vtk_octree->NewCellCursor();
  cursor->ToRoot();
  
  boxm_block_iterator<boct_tree<short, float> > iter(scene);
  iter.begin();
  scene->load_block(iter.index());
  
  dbrec3d_clone_leaves(cursor,(*iter)->get_tree()->root(),vtk_octree);
  //cursor->UnRegister(this); //decrease pointer count
  
  //free unnecessary memory
  scalars->Squeeze();
  scene->unload_active_blocks();
  //assert("post: valid_levels" && output->GetNumberOfLevels()<=this->GetLevels());
  //assert("post: dataset_and_data_size_match" && output->CheckAttributes()==0);
  
}
void dbrec3d_clone_leaves(vtkHyperOctreeCursor *cursor,
                          boct_tree_cell<short,float> *boxm_cell,
                          vtkHyperOctree *vtk_octree)
{
  //:If reached a leaf cell, insert
  if(boxm_cell->is_leaf())
  {
    float value = boxm_cell->data();
    vtkIdType id=cursor->GetLeafId();
    vtk_octree->GetLeafData()->GetScalars()->InsertTuple1(id,value);
  }
  else{    //subdivide and recurre 
    vtk_octree->SubdivideLeaf(cursor);
    vcl_vector<boct_tree_cell<short,float> * > children;
    boxm_cell->all_children(children);
    if(cursor->GetNumberOfChildren()!=8){
      vcl_cerr << "Error: wrong number of children" << vcl_endl;
      return;
    }
    for (unsigned i=0; i<8; i++) 
    {
      cursor->ToChild(i);
      dbrec3d_clone_leaves(cursor,children[i],vtk_octree);
      cursor->ToParent();
    }
  }
}

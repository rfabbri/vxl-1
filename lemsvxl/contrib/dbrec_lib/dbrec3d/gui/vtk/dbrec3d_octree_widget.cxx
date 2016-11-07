//:
// \file
// \author Isabel Restrepo
// \date 16-Nov-2010

#include "dbrec3d_octree_widget.h"
#include "dbrec3d_vtk_boxm_utils.h"

#include<vtkInteractorStyleTrackballCamera.h>
#include<vtkVolumeRayCastCompositeFunction.h>

dbrec3d_octree_widget::dbrec3d_octree_widget(vtkRenderWindow *window,boxm_scene<boct_tree<short, float> > *scene)
{
  window_ = window;
  renderer_ = vtkRenderer::New();
  window_->AddRenderer(renderer_);
  interactor_ = vtkRenderWindowInteractor::New();
  interactor_->SetRenderWindow(window_);
  renderer_->SetBackground(1.0f, 1.0f, 1.0f);
  vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
  interactor_->SetInteractorStyle(style);
  
  //QT
  this->SetRenderWindow(window_);
  
  //Volume data init
  init_volume_data(scene);
  
}

dbrec3d_octree_widget::dbrec3d_octree_widget()
{
  renderer_->Delete();
  interactor_->Delete();
  opacity_transfun_->Delete();
  volume_property_->Delete();
  volume_->Delete();
}

void dbrec3d_octree_widget::init_volume_data(boxm_scene<boct_tree<short, float> > *scene)
{
  octree_data_ =  dbrec3d_scene_to_hyperoctree(scene);
  
  opacity_transfun_ = vtkPiecewiseFunction::New();
  tf_min_ = 0;
  tf_max_ = 255;
  opacity_transfun_->AddPoint(tf_min_, 1.0);
  opacity_transfun_->AddPoint(tf_max_, 0.0);
  
  
  volume_property_ = vtkVolumeProperty::New();
  volume_property_->SetIndependentComponents(0);
  volume_property_->SetScalarOpacity(opacity_transfun_);
  volume_property_->SetInterpolationTypeToLinear();
  //volume_property_->ShadeOff();
  
  volume_mapper_ = vtkVolumeRayCastMapper::New();
  vtkVolumeRayCastCompositeFunction *rayCastFunction =vtkVolumeRayCastCompositeFunction::New();
  
  volume_mapper_->SetInput(octree_data_);
  volume_mapper_->SetVolumeRayCastFunction(rayCastFunction);
  
  volume_= vtkVolume::New();
  volume_->SetMapper(volume_mapper_);
  volume_->SetProperty(volume_property_);
  
  renderer_->AddVolume(volume_);
  
}

//void vtkHyperOctreeSampleFunction::Subdivide(vtkHyperOctreeCursor *cursor,
//                                             boct_tree_cell<short,float> *boxm_cell,
//                                             int level,
//                                             vtkHyperOctree *vtk_octree)
//{
//  //:If reached a leaf cell, insert
//  if(cell->is_leaf())
//  {
//    float value = boxm_cell->get_data();
//    vtkIdType id=cursor->GetLeafId();
//    vtk_octree->GetLeafData()->GetScalars()->InsertTuple1(id,value);
//  }
//  else{    //subdivide and recurre 
//    vtk_octree->SubdivideLeaf(cursor);
//    vcl_vector<boct_tree_cell<T_loc,T_data>*> children = boxm_cell->children();
//    if(cursor->GetNumberOfChildren()!=8){
//      vcl_cerr << "Error: wrong number of children" << vcl_end;
//      return
//    };
//    for (unsigned i=0; i<8; i++) {
//    {
//      cursor->ToChild(i);
//      this->Subdivide(cursor,children[i],level+1,vtk_octree);
//      cursor->ToParent();
//    }
//    
//  }
//}

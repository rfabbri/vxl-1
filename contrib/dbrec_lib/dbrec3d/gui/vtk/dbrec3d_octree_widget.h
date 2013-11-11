// This is dbrec3d_octree_widget.h
#ifndef dbrec3d_octree_widget_h
#define dbrec3d_octree_widget_h

//:
// \file
// \brief A class that implements the dbrec3d_part concept for composite parts (i.e non-leafs). 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  16-Nov-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkHyperOctree.h>
#include <vtkVolumeProperty.h>
#include <vtkVolume.h>
#include <vtkPiecewiseFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolumeRayCastMapper.h>
#include <QVTKWidget.h>

#include <boxm/boxm_scene.h>

class dbrec3d_octree_widget: public QVTKWidget{
  
  Q_OBJECT
  
public:
  dbrec3d_octree_widget(vtkRenderWindow *window, boxm_scene<boct_tree<short, float> > *scene);
  dbrec3d_octree_widget();

protected:
  
  //: Allocates the volume data - called in the class contructor
  void init_volume_data(boxm_scene<boct_tree<short, float> > *scene);
  //: Volume dimensions
  unsigned dimx_;
  unsigned dimy_;
  unsigned dimz_;
  //: Transfer function limits
  float tf_min_;
  float tf_max_;

public:
  //: VTK components
  vtkRenderer *renderer_;
  vtkRenderWindow* window_;
  vtkRenderWindowInteractor* interactor_;
  vtkVolume* volume_;
  vtkHyperOctree* octree_data_;
  vtkVolumeProperty* volume_property_;
  vtkPiecewiseFunction* opacity_transfun_;
  vtkVolumeRayCastMapper* volume_mapper_;
  
};
#endif

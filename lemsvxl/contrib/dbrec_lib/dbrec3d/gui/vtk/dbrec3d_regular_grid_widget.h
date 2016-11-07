// This is dbrec3d_regular_grid_widget.h
#ifndef dbrec3d_regular_grid_widget_h
#define dbrec3d_regular_grid_widget_h

//:
// \file
// \brief A class that implements the dbrec3d_part concept for composite parts (i.e non-leafs). 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  10-Nov-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>
#include <vtkVolumeProperty.h>
#include <vtkVolume.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolumeRayCastMapper.h>
#include <QVTKWidget.h>

#include <vnl/vnl_vector.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <boct/boct_tree_cell.h>

class dbrec3d_regular_grid_widget: public QVTKWidget
{
  Q_OBJECT
  
public:
  //: Contructor from a vector
  dbrec3d_regular_grid_widget(vtkRenderWindow *window, const vnl_vector<double> &vector, 
                              unsigned dimx, unsigned dimy, unsigned dimz,
                              float min_val, float max_val);
  
  //: Contructor from a bvpl_kernel_sptr
  dbrec3d_regular_grid_widget(vtkRenderWindow *window, bvpl_kernel_sptr kernel, 
                              float min_val, float max_val);
  
  //: Constructor fro 1-d kernels
  dbrec3d_regular_grid_widget(vtkRenderWindow *window, 
                             vnl_vector_fixed<float,5> filter_x, 
                             vnl_vector_fixed<float,5> filter_y, 
                             vnl_vector_fixed<float,5> filter_z,
                              float min_val, float max_val);
  
  //: Constructor from a region and the cells contained in that region
  dbrec3d_regular_grid_widget(vtkRenderWindow *window, 
                              vgl_box_3d<double> const &bbox,
                              double cell_length,
                              vcl_vector<boct_cell_data<short, float> > const &cell_data,
                              float min_val, float max_val, unsigned resolution_level);
  
  
  //:Destructor
  ~dbrec3d_regular_grid_widget();
  
  void update_volume_data(const vnl_vector<double> &vector);
  void update_volume_data(bvpl_kernel_sptr kernel);
  void update_volume_data(vnl_vector_fixed<float,5> filter_x, 
                          vnl_vector_fixed<float,5> filter_y, 
                          vnl_vector_fixed<float,5> filter_z);
  void update_volume_data(vgl_box_3d<double> const &bbox,
                          double cell_length, 
                          vcl_vector<boct_cell_data<short, float> > const &cell_data, 
                          unsigned resolution_level);
    
  //: Size properties
  virtual QSize sizeHint() const { return QSize(400,400); }
  
public slots:
  
  //: Set min point of a linear transfer function
  void set_min_tf_point(float min)
  {
    
    opacity_transfun_->RemovePoint(tf_min_);
    color_transfun_->RemovePoint( tf_min_);
    min = (min-min_val_)/range_val_ *255.0f;
    opacity_transfun_->AddPoint(min, 0.0);
    color_transfun_->AddRGBPoint(min, 0.0, 0.0, 0.0);
    tf_min_ = min;
    update();
  }
  
  //: Set max point of a linear transfer function
  void set_max_tf_point(float max)
  {
    opacity_transfun_->RemovePoint(tf_max_);
    color_transfun_->RemovePoint( tf_max_);

    max = (max-min_val_)/range_val_ *255.0f;
    opacity_transfun_->AddPoint(max, 1.0);
    color_transfun_->AddRGBPoint(tf_max_, 1.0, 1.0, 1.0);

    tf_max_ = max;
    update();
  }
  
signals:
  //: If multiple volumes are in memory, you can move between them using arrow keys
  void request_next_volume();
  void request_prev_volume();
  
public:
  
  //: VTK components
  vtkRenderer *renderer_;
  vtkRenderWindow* window_;
  vtkRenderWindowInteractor* interactor_;
  vtkVolume* volume_;
  vtkImageData* volume_data_;
  vtkVolumeProperty* volume_property_;
  vtkPiecewiseFunction* opacity_transfun_;
  vtkColorTransferFunction* color_transfun_;
  vtkVolumeRayCastMapper* volume_mapper_;
  //vtkFixedPointVolumeRayCastMapper* volume_mapper_;
  //: Transfer function limits
  float tf_min_;
  float tf_max_;
  
  float min_val_;
  float max_val_;
  float range_val_;
  

  //: Volume dimensions
  unsigned dimx_;
  unsigned dimy_;
  unsigned dimz_;
  
protected:
  //: Allocates the volume data - called in the class contructor
  void init_volume_data(const vnl_vector<double> &vector);
  void init_volume_data(bvpl_kernel_sptr kernel);
  void init_volume_data(vnl_vector_fixed<float,5> filter_x, vnl_vector_fixed<float,5> filter_y, vnl_vector_fixed<float,5> filter_z );
  void init_volume_data(vgl_box_3d<double> const &bbox,
                        double cell_length, vcl_vector<boct_cell_data<short, float> > const &cell_data,unsigned resolution_level);
  
  //: Handle key events
  virtual void keyPressEvent(QKeyEvent *e);

  
};
#endif

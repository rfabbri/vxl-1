//:
// \file
// \author Isabel Restrepo
// \date 10-Nov-2010

#include "dbrec3d_regular_grid_widget.h"

#include<vtkInteractorStyleTrackballCamera.h>
#include<vtkVolumeRayCastCompositeFunction.h>
#include <QKeyEvent>

#include <vcl_cassert.h>

#include <vnl/vnl_matrix_fixed.h>

dbrec3d_regular_grid_widget::dbrec3d_regular_grid_widget(vtkRenderWindow *window,const vnl_vector<double> &vector,
                                                         unsigned dimx, unsigned dimy, unsigned dimz,
                                                         float min_val, float max_val)
{

  tf_min_ = 0;
  tf_max_ = 255;
  min_val_ = min_val;
  max_val_ = max_val;
  range_val_ = max_val_-min_val_;
  window_ = window;
  renderer_ = vtkRenderer::New();
  window_->AddRenderer(renderer_);
  interactor_ = vtkRenderWindowInteractor::New();
  interactor_->SetRenderWindow(window_);
  renderer_->SetBackground(0.0f, 1.0f, 0.0f);
  vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
  interactor_->SetInteractorStyle(style);
  
  //QT
  this->SetRenderWindow(window_);
  
  //Volume data init
  dimx_=dimx; dimy_=dimy; dimz_=dimz;
  assert(dimx_*dimy_*dimz_ == vector.size());
  init_volume_data(vector);

}

dbrec3d_regular_grid_widget::dbrec3d_regular_grid_widget(vtkRenderWindow *window, bvpl_kernel_sptr kernel,
                                                         float min_val, float max_val)
{
  tf_min_ = 0;
  tf_max_ = 255;
  min_val_ = min_val;
  max_val_ = max_val;
  range_val_ = max_val_-min_val_;
  window_ = window;
  renderer_ = vtkRenderer::New();
  window_->AddRenderer(renderer_);
  interactor_ = vtkRenderWindowInteractor::New();
  interactor_->SetRenderWindow(window_);
  renderer_->SetBackground(0.0f, 1.0f, 0.0f);
  vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
  interactor_->SetInteractorStyle(style);
  
  //QT
  this->SetRenderWindow(window_);
  
  //Volume data init
  dimx_=kernel->dim().x(); dimy_=kernel->dim().y(); dimz_=kernel->dim().z();
  init_volume_data(kernel);
  
}

dbrec3d_regular_grid_widget::dbrec3d_regular_grid_widget(vtkRenderWindow *window, 
                                                         vnl_vector_fixed<float,5> filter_x, 
                                                         vnl_vector_fixed<float,5> filter_y, 
                                                         vnl_vector_fixed<float,5> filter_z,
                                                         float min_val, float max_val)
{
  tf_min_ = 0;
  tf_max_ = 255;
  min_val_ = min_val;
  max_val_ = max_val;
  range_val_ = max_val_-min_val_;
  window_ = window;
  renderer_ = vtkRenderer::New();
  window_->AddRenderer(renderer_);
  interactor_ = vtkRenderWindowInteractor::New();
  interactor_->SetRenderWindow(window_);
  renderer_->SetBackground(0.0f, 0.0f, 0.0f);
  vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
  interactor_->SetInteractorStyle(style);
  
  //QT
  this->SetRenderWindow(window_);
  
  //Volume data init
  dimx_=5; dimy_=5; dimz_=5;
  init_volume_data(filter_x, filter_y, filter_z);
  
}


//: Constructor from a region and the cells contained in that region
dbrec3d_regular_grid_widget::dbrec3d_regular_grid_widget( vtkRenderWindow *window, 
                                                          vgl_box_3d<double> const &bbox,
                                                          double cell_length,
                                                          vcl_vector<boct_cell_data<short, float> > const &cell_data,
                                                          float min_val, float max_val, unsigned resolution_level)
{
  tf_min_ = 0;
  tf_max_ = 255;
  min_val_ = min_val;
  max_val_ = max_val;
  range_val_ = max_val_-min_val_;
  window_ = window;
  renderer_ = vtkRenderer::New();
  window_->AddRenderer(renderer_);
  interactor_ = vtkRenderWindowInteractor::New();
  interactor_->SetRenderWindow(window_);
  renderer_->SetBackground(0.0f, 0.0f, 0.0f);
  vtkInteractorStyleTrackballCamera *style = vtkInteractorStyleTrackballCamera::New();
  interactor_->SetInteractorStyle(style);
  
  //QT
  this->SetRenderWindow(window_);
  
  //Volume data init
  dimx_=5; dimy_=5; dimz_=5;
  init_volume_data(bbox, cell_length, cell_data, resolution_level);
  
}

dbrec3d_regular_grid_widget::~dbrec3d_regular_grid_widget()
{
  renderer_->Delete();
  interactor_->Delete();
}

void dbrec3d_regular_grid_widget::init_volume_data(const vnl_vector<double> &vector)
{
  volume_data_ = vtkImageData::New();
  volume_data_->SetScalarTypeToUnsignedChar();
  volume_data_->SetNumberOfScalarComponents(1);
  volume_data_->SetExtent(0, dimx_-1, 0, dimy_-1, 0, dimz_-1);
  volume_data_->SetSpacing(10.0, 10.0, 10.0);
  volume_data_->SetOrigin(0, 0, 0);
  
  volume_data_->AllocateScalars();
  
  // Fill in volume_data with the data in the vector
  for(int z = 0; z<dimz_; z++)
    for(int y = 0; y<dimy_; y++)
      for(int x = 0; x<dimx_; x++)
      {
        *((unsigned char *)volume_data_->GetScalarPointer(x, y, z)) = (unsigned char)((vector[x + dimx_*y + dimx_*dimy_*z]-min_val_)/range_val_ *255.0f);
        
      }
    
  opacity_transfun_ = vtkPiecewiseFunction::New();
  opacity_transfun_->AddPoint(tf_min_, 0.0);
  opacity_transfun_->AddPoint(tf_max_, 1.0);
  
  color_transfun_= vtkColorTransferFunction::New();
  color_transfun_->AddRGBPoint( tf_min_, 0.0, 0.0, 0 );
  color_transfun_->AddRGBPoint( tf_max_,1.0, 1.0, 1.0);
  
  volume_property_ = vtkVolumeProperty::New();
  volume_property_->SetIndependentComponents(0);
  volume_property_->SetScalarOpacity(opacity_transfun_);
  volume_property_->SetColor(color_transfun_);
  volume_property_->SetInterpolationTypeToLinear();
  //volume_property_->ShadeOff();
  
  volume_mapper_ = vtkVolumeRayCastMapper::New();
  vtkVolumeRayCastCompositeFunction *rayCastFunction =vtkVolumeRayCastCompositeFunction::New();
  
  volume_mapper_->SetInput(volume_data_);
  volume_mapper_->SetVolumeRayCastFunction(rayCastFunction);
  
  volume_= vtkVolume::New();
  volume_->SetMapper(volume_mapper_);
  volume_->SetProperty(volume_property_);
  
  renderer_->AddVolume(volume_);
  
}

void dbrec3d_regular_grid_widget::init_volume_data(bvpl_kernel_sptr kernel)
{
  volume_data_ = vtkImageData::New();
  volume_data_->SetScalarTypeToUnsignedChar();
  volume_data_->SetNumberOfScalarComponents(1);
  volume_data_->SetExtent(kernel->min_point().x(), kernel->max_point().x(), kernel->min_point().y(), kernel->max_point().y(), kernel->min_point().z(), kernel->max_point().z());
  volume_data_->SetSpacing(10.0, 10.0, 10.0);
  volume_data_->SetOrigin(0, 0, 0);
  
  volume_data_->AllocateScalars();
  
  // Fill in volume_data with the data in the vector
  bvpl_kernel_iterator kernel_iter = kernel->iterator();
  
  kernel_iter.begin(); // reset the kernel iterator
  while (!kernel_iter.isDone())
  {
    vgl_point_3d<int> kernel_idx = kernel_iter.index();
    *((unsigned char *)volume_data_->GetScalarPointer(kernel_idx.x(), kernel_idx.y(), kernel_idx.z())) = (unsigned char)(((*kernel_iter).c_-min_val_)/range_val_ *255.0f);
    ++kernel_iter;
    
  }
  
  opacity_transfun_ = vtkPiecewiseFunction::New();
  opacity_transfun_->AddPoint(tf_min_, 0.0);
  opacity_transfun_->AddPoint(tf_max_, 1.0);
  
  color_transfun_= vtkColorTransferFunction::New();
  color_transfun_->AddRGBPoint( tf_min_, 1.0, 1.0, 0 );
  color_transfun_->AddRGBPoint( tf_max_,1.0, 0.0, 0.0);
  
  volume_property_ = vtkVolumeProperty::New();
  volume_property_->SetIndependentComponents(0);
  volume_property_->SetScalarOpacity(opacity_transfun_);
  volume_property_->SetColor(color_transfun_);
  volume_property_->SetInterpolationTypeToLinear();
  //volume_property_->ShadeOff();
  
  volume_mapper_ = vtkVolumeRayCastMapper::New();
  vtkVolumeRayCastCompositeFunction *rayCastFunction =vtkVolumeRayCastCompositeFunction::New();
  
  volume_mapper_->SetInput(volume_data_);
  volume_mapper_->SetVolumeRayCastFunction(rayCastFunction);
  
  volume_= vtkVolume::New();
  volume_->SetMapper(volume_mapper_);
  volume_->SetProperty(volume_property_);
  
  renderer_->AddVolume(volume_);
  
}

void dbrec3d_regular_grid_widget::init_volume_data(vnl_vector_fixed<float,5> filter_x, vnl_vector_fixed<float,5> filter_y, vnl_vector_fixed<float,5> filter_z )
{
  volume_data_ = vtkImageData::New();
  volume_data_->SetScalarTypeToUnsignedChar();
  volume_data_->SetNumberOfScalarComponents(1);
  volume_data_->SetExtent(-2, 2, -2, 2, -2, 2);
  volume_data_->SetSpacing(10.0, 10.0, 10.0);
  volume_data_->SetOrigin(0, 0, 0);
  
  volume_data_->AllocateScalars();
  
  // Fill in volume_data with the data in the vector
  vnl_matrix_fixed<float, 5, 5>  filter_yx = outer_product(filter_y, filter_x);
  
  //iterate through all columns of the 2-d filter
  for(unsigned i= 0; i < filter_yx.columns(); i++)
  {
    vnl_vector_fixed<float, 5> filter_yx_col = filter_yx.get_column(i);
    vnl_matrix_fixed<float, 5, 5> filter_zyx_2d = outer_product(filter_z, filter_yx_col);
    for (unsigned j = 0; j < filter_zyx_2d.columns() ; j++) {
      for (unsigned k = 0; k < filter_zyx_2d.rows() ; k++)
        *((unsigned char *)volume_data_->GetScalarPointer(i-2, j-2, k-2)) = (unsigned char)((filter_zyx_2d[k][j] -min_val_)/range_val_ *255.0f);
 
    }
  }
    
  opacity_transfun_ = vtkPiecewiseFunction::New();
  opacity_transfun_->AddPoint(tf_min_, 1.0);
  opacity_transfun_->AddPoint(tf_max_ - tf_min_, 0);
  opacity_transfun_->AddPoint(tf_max_, 1.0);
  
  color_transfun_= vtkColorTransferFunction::New();
  color_transfun_->AddRGBPoint( tf_min_, 0.0, 0.0, 0 );
  color_transfun_->AddRGBPoint( tf_max_,1.0, 1.0, 1.0);
  
  volume_property_ = vtkVolumeProperty::New();
  volume_property_->SetIndependentComponents(0);
  volume_property_->SetScalarOpacity(opacity_transfun_);
  volume_property_->SetColor(color_transfun_);
  volume_property_->SetInterpolationTypeToLinear();
  //volume_property_->ShadeOff();
  
  volume_mapper_ = vtkVolumeRayCastMapper::New();
  vtkVolumeRayCastCompositeFunction *rayCastFunction =vtkVolumeRayCastCompositeFunction::New();
  
  volume_mapper_->SetInput(volume_data_);
  volume_mapper_->SetVolumeRayCastFunction(rayCastFunction);
  
  volume_= vtkVolume::New();
  volume_->SetMapper(volume_mapper_);
  volume_->SetProperty(volume_property_);
  
  renderer_->AddVolume(volume_);
  
}

void dbrec3d_regular_grid_widget::init_volume_data(vgl_box_3d<double> const &bbox,
                                                   double cell_length, vcl_vector<boct_cell_data<short, float> > const &cell_data, unsigned resolution_level)
{
  volume_data_ = vtkImageData::New();
  volume_data_->SetScalarTypeToUnsignedChar();
  volume_data_->SetNumberOfScalarComponents(1);

  const int dim_x = (int)((bbox.max_x() - bbox.min_x())/(cell_length));
  const int dim_y = (int)((bbox.max_y() - bbox.min_y())/(cell_length));
  const int dim_z = (int)((bbox.max_z() - bbox.min_z())/(cell_length));
  volume_data_->SetExtent(0, dim_x, 0, dim_y, 0, dim_z);
  
  volume_data_->SetSpacing(cell_length, cell_length, cell_length);
  volume_data_->SetOrigin(0.0,0.0,0.0);
  
  volume_data_->AllocateScalars();
  
  // Fill in volume_data with the data in the vector
  vcl_vector<boct_cell_data<short, float> >::const_iterator it = cell_data.begin();
  
 
  for (unsigned z= 0; z<=dim_z; z++) 
    for (unsigned y= 0; y<=dim_y; y++) 
      for (unsigned x= 0; x<=dim_x; x++) 
        *((unsigned char *)volume_data_->GetScalarPointer(x,y,z)) = 0.0f;

    
  for (; it!=cell_data.end(); it++)
  {   
    vgl_point_3d<double> c = it->centroid_;
    vgl_point_3d<double> node(it->centroid_.x() - bbox.min_point().x(),
                              it->centroid_.y() - bbox.min_point().y(),
                              it->centroid_.z() - bbox.min_point().z());
    
    float cell_val = (unsigned char)(((it->data_)-min_val_)/range_val_ *255.0f);
    
    unsigned int level =it->level_;
    if (level == resolution_level) {
      
      // just copy value to output array
      int x_index = vcl_floor(node.x()/cell_length);
      int y_index = vcl_floor(node.y()/cell_length);
      int z_index = vcl_floor(node.z()/cell_length);
      
      //int out_index=static_cast<int>(ncells-1-(node.z()/step_len) + (node.y()/step_len)*ncells + (node.x()/step_len)*ncells*ncells);
      
      *((unsigned char *)volume_data_->GetScalarPointer(x_index,y_index,z_index)) = cell_val;

    }
//    if (level < resolution_level) {
//      // cell is smaller than output cells. increment output cell value
//      unsigned int ds_factor = 1 << (resolution_level - level);
//      double update_weight = 1.0 / double(ds_factor*ds_factor*ds_factor);
//      const unsigned int node_x = static_cast<unsigned int>(node.x()/cell_length);
//      const unsigned int node_y = static_cast<unsigned int>(node.y()/cell_length);
//      const unsigned int node_z = static_cast<unsigned int>(node.z()/cell_length);
//      
//      int x_index = node_x;
//      int y_index = node_y;
//      int z_index = node_z;
//      *((unsigned char *)volume_data_->GetScalarPointer(x_index,y_index,z_index)) += (cell_val*update_weight);
//      
//    }
#if 0
    else if (level > resolution_level) {
      // cell is bigger than output cells.  copy value to all contained output cells.
      const unsigned int us_factor = 1 << (level - resolution_level);
      const unsigned int node_x_start = static_cast<unsigned int>(node.x()/cell_length);
      const unsigned int node_y_start = static_cast<unsigned int>(node.y()/cell_length);
      const unsigned int node_z_start = static_cast<unsigned int>(node.z()/cell_length);
      for (unsigned int z=node_z_start; z<node_z_start+us_factor; ++z) {
        for (unsigned int y=node_y_start; y<node_y_start+us_factor; ++y) {
          for (unsigned int x=node_x_start; x<node_x_start+us_factor; ++x) {
            
            int x_index = x;
            int y_index = y;
            int z_index = z;
            *((unsigned char *)volume_data_->GetScalarPointer(x_index,y_index,z_index)) = cell_val;

          }
        }
      }
    }
    else {
      // cell is smaller than output cells. increment output cell value
      unsigned int ds_factor = 1 << (resolution_level - level);
      double update_weight = 1.0 / double(ds_factor*ds_factor*ds_factor);
      const unsigned int node_x = static_cast<unsigned int>(node.x()/cell_length);
      const unsigned int node_y = static_cast<unsigned int>(node.y()/cell_length);
      const unsigned int node_z = static_cast<unsigned int>(node.z()/cell_length);
      
      int x_index = node_x;
      int y_index = node_y;
      int z_index = node_z;
      *((unsigned char *)volume_data_->GetScalarPointer(x_index,y_index,z_index)) = cell_val*update_weight;

    }
      
#endif
  }
  
  opacity_transfun_ = vtkPiecewiseFunction::New();
  opacity_transfun_->AddPoint(tf_min_, 0.0);
  opacity_transfun_->AddPoint(tf_max_, 1.0);
  
  color_transfun_= vtkColorTransferFunction::New();
  color_transfun_->AddRGBPoint( tf_min_, 0.0, 0.0, 0.0 );
  color_transfun_->AddRGBPoint( tf_max_,1.0, 1.0, 1.0);
  
  volume_property_ = vtkVolumeProperty::New();
  volume_property_->SetIndependentComponents(0);
  volume_property_->SetScalarOpacity(opacity_transfun_);
  volume_property_->SetColor(color_transfun_);
  volume_property_->SetInterpolationTypeToLinear();
  //volume_property_->ShadeOff();
  
  volume_mapper_ = vtkVolumeRayCastMapper::New();
  vtkVolumeRayCastCompositeFunction *rayCastFunction =vtkVolumeRayCastCompositeFunction::New();
  
  volume_mapper_->SetInput(volume_data_);
  volume_mapper_->SetVolumeRayCastFunction(rayCastFunction);
  
  volume_= vtkVolume::New();
  volume_->SetMapper(volume_mapper_);
  volume_->SetProperty(volume_property_);
  
  renderer_->AddVolume(volume_);
  
}

//: Use this method to update the volume data. Note: It assumes volume data was initiallized and allocated
void dbrec3d_regular_grid_widget::update_volume_data(const vnl_vector<double> &vector)
{
  assert(dimx_*dimy_*dimz_ == vector.size());

  for(int z = 0; z<dimz_; z++)
    for(int y = 0; y<dimy_; y++)
      for(int x = 0; x<dimx_; x++)
      {
        *((unsigned char *)volume_data_->GetScalarPointer(x, y, z)) = (unsigned char)((vector[x + dimx_*y + dimx_*dimy_*z]-min_val_)/range_val_ *255.0f); 
        
      }
  
  
  update();
}

//: Use this method to update the volume data. Note: It assumes volume data was initiallized and allocated
void dbrec3d_regular_grid_widget::update_volume_data(bvpl_kernel_sptr kernel)
{
  bvpl_kernel_iterator kernel_iter = kernel->iterator();
  
  kernel_iter.begin(); // reset the kernel iterator
  while (!kernel_iter.isDone())
  {
    vgl_point_3d<int> kernel_idx = kernel_iter.index();
    *((unsigned char *)volume_data_->GetScalarPointer(kernel_idx.x(), kernel_idx.y(), kernel_idx.z())) = (unsigned char)(((*kernel_iter).c_-min_val_)/range_val_ *255.0f);
    ++kernel_iter;

  } 
  
  update();
}

//: Use this method to update the volume data. Note: It assumes volume data was initiallized and allocated
void dbrec3d_regular_grid_widget::update_volume_data(vnl_vector_fixed<float,5> filter_x, vnl_vector_fixed<float,5> filter_y, vnl_vector_fixed<float,5> filter_z)
{
  // Fill in volume_data with the data in the vector
  vnl_matrix_fixed<float, 5, 5>  filter_yx = outer_product(filter_y, filter_x);
  
  //iterate through all columns of the 2-d filter
  float area= 0.0f;
  for(unsigned i= 0; i < filter_yx.columns(); i++)
  {
    vnl_vector_fixed<float, 5> filter_yx_col = filter_yx.get_column(i);
    vnl_matrix_fixed<float, 5, 5> filter_zyx_2d = outer_product(filter_z, filter_yx_col);
    //vcl_cout << filter_zyx_2d << vcl_endl;
    for (unsigned j = 0; j < filter_zyx_2d.columns() ; j++) {
      for (unsigned k = 0; k < filter_zyx_2d.rows() ; k++){
        *((unsigned char *)volume_data_->GetScalarPointer(i-2, j-2, k-2)) = (unsigned char)((filter_zyx_2d[k][j] -min_val_)/range_val_ *255.0f);
        area= area+(filter_zyx_2d[k][j]*filter_zyx_2d[k][j]);
      }
      
    }
  }
  
  vcl_cout << area << vcl_endl;
  update();
}


void dbrec3d_regular_grid_widget::update_volume_data(vgl_box_3d<double> const &bbox,
                                                    double cell_length, vcl_vector<boct_cell_data<short, float> > const &cell_data, unsigned resolution_level)
{
  const int dim_x = (int)((bbox.max_x() - bbox.min_x())/(cell_length));
  const int dim_y = (int)((bbox.max_y() - bbox.min_y())/(cell_length));
  const int dim_z = (int)((bbox.max_z() - bbox.min_z())/(cell_length));
  volume_data_->SetExtent(0, dim_x, 0, dim_y, 0, dim_z);  
  volume_data_->SetSpacing(cell_length, cell_length, cell_length);
  volume_data_->SetOrigin(0.0, 0.0, 0.0);
  
  volume_data_->AllocateScalars();
  
  //initialize to zero
  for (unsigned z= 0; z<=dim_z; z++) 
    for (unsigned y= 0; y<=dim_y; y++) 
      for (unsigned x= 0; x<=dim_x; x++) 
        *((unsigned char *)volume_data_->GetScalarPointer(x,y,z)) = 0.0f;
  
  
  
  // Fill in volume_data with the data in the vector
  vcl_vector<boct_cell_data<short, float> >::const_iterator it = cell_data.begin();
  
  for (; it!=cell_data.end(); it++)
  {   
    vgl_point_3d<double> node(it->centroid_.x() - bbox.min_point().x(),
                              it->centroid_.y() - bbox.min_point().y(),
                              it->centroid_.z() - bbox.min_point().z());
    
    float cell_val = (unsigned char)(((it->data_)-min_val_)/range_val_ *255.0f);
    
    unsigned int level =it->level_;
    if (level == resolution_level) {
      
      // just copy value to output array
      int x_index = vcl_floor(node.x()/cell_length);
      int y_index = vcl_floor(node.y()/cell_length);
      int z_index = vcl_floor(node.z()/cell_length);
      
      //int out_index=static_cast<int>(ncells-1-(node.z()/step_len) + (node.y()/step_len)*ncells + (node.x()/step_len)*ncells*ncells);
      
      *((unsigned char *)volume_data_->GetScalarPointer(x_index,y_index,z_index)) = cell_val;
      
    }
//    if (level < resolution_level) {
//      // cell is smaller than output cells. increment output cell value
//      unsigned int ds_factor = 1 << (resolution_level - level);
//      double update_weight = 1.0 / double(ds_factor*ds_factor*ds_factor);
//      const unsigned int node_x = static_cast<unsigned int>(node.x()/cell_length);
//      const unsigned int node_y = static_cast<unsigned int>(node.y()/cell_length);
//      const unsigned int node_z = static_cast<unsigned int>(node.z()/cell_length);
//      
//      int x_index = node_x;
//      int y_index = node_y;
//      int z_index = node_z;
//      *((unsigned char *)volume_data_->GetScalarPointer(x_index,y_index,z_index)) += (cell_val*update_weight);
//      
//    }
  }
   
  update();

}

void dbrec3d_regular_grid_widget::keyPressEvent(QKeyEvent *event)
{
  //if undefined, ignore
  if (event->key() == 0)
	{
		event->ignore();
		return;
	}
  
	//const QtKeyboardModifiers modifiers = e->modifiers();
  
  if(event->key() == Qt::Key_Right)
  {
    emit request_next_volume();//update_volume_data();
  }
  if(event->key() == Qt::Key_Left)
  {
    emit request_prev_volume();//update_volume_data();
  }
}
// This is dbrec3d_filter_grid_widget.h
#ifndef dbrec3d_filter_grid_widget_h
#define dbrec3d_filter_grid_widget_h

//:
// \file
// \brief A widget that displays a regular_grid_widget and sliders to control the transfer function 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  11-Nov-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <QWidget>
#include <QLabel>
#include <QSlider>

#include <vcl_string.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <dbrec3d/gui/dbrec3d_double_slider.h>
#include "dbrec3d_regular_grid_widget.h"

class dbrec3d_filter_grid_widget : public QWidget
{
 Q_OBJECT
  
public:
  dbrec3d_filter_grid_widget(const vnl_vector<double> &vector, unsigned dimx, unsigned dimy, unsigned dimz,float tf_min, float tf_max);
  dbrec3d_filter_grid_widget(bvpl_kernel_sptr kernel, float tf_min, float tf_max);
  dbrec3d_filter_grid_widget(vnl_vector_fixed<float,5> filter_x, 
                             vnl_vector_fixed<float,5> filter_y, 
                             vnl_vector_fixed<float,5> filter_z, float tf_min, float tf_max);
  dbrec3d_filter_grid_widget(vgl_box_3d<double> const &bbox,
                             double cell_length,
                             vcl_vector<boct_cell_data<short, float> > const &cell_data,
                             float min_val, float max_val, unsigned resolution_level);
  
  ~dbrec3d_filter_grid_widget();

  friend class dbrec3d_pca_kernels_viewer;
  friend class dbrec3d_kernel_viewer;
  friend class dbrec3d_vector_kernel_viewer;
  friend class dbrec3d_steerable_basis_viewer;
  friend class dbrec3d_region_viewer;
  
protected:
  dbrec3d_double_slider *slider_widget_;
  dbrec3d_regular_grid_widget *grid_widget_;
  
};
#endif

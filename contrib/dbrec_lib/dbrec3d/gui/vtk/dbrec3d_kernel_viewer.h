// This is dbrec3d_kernel_viewer.h
#ifndef dbrec3d_kernel_viewer_h
#define dbrec3d_kernel_viewer_h

//:
// \file
// \brief A volumentic viewer for bvpl_kernels. 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  27-Jan-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <QMainWindow>

#include "dbrec3d_filter_grid_widget.h"
#include <dbrec3d/gui/dbrec3d_histogram_widgets.h>
#include <dbrec3d/gui/dbrec3d_line_plot.h>

#include <vnl/vnl_matrix.h>
#include <vcl_map.h>

class dbrec3d_kernel_viewer : public QMainWindow
{
  Q_OBJECT
  
public:
  dbrec3d_kernel_viewer(vcl_map<vcl_string, bvpl_kernel_sptr> &kernels, float tf_min, float tf_max);
  dbrec3d_kernel_viewer(vcl_vector< vnl_vector<double> > &kernels, float tf_min, float tf_max);

  //~dbrec3d_kernel_viewer();
  
  public slots:
  
  void next_volume();
  void prev_volume();
  
  
protected:
  
  dbrec3d_filter_grid_widget* kernel_widget_;
  QLabel* component_label_;
  dbrec3d_histogram_widget* hist_widget_;
  
  //: The kernels
  vcl_map<vcl_string, bvpl_kernel_sptr> kernels_;
  //: Iterator to current kernel
  vcl_map<vcl_string, bvpl_kernel_sptr>::iterator curr_kernel_it_;
  
};


//: Use this when your kernels are stored as vnl_vectors
class dbrec3d_vector_kernel_viewer : public QMainWindow
{
  Q_OBJECT
  
public:
  dbrec3d_vector_kernel_viewer(vcl_vector< vnl_vector<double> > &kernels, unsigned dimx, unsigned dimy, unsigned dimz,float tf_min, float tf_max);
  
  //~dbrec3d_kernel_viewer();
  
  public slots:
  
  void next_volume();
  void prev_volume();
  
  
protected:
  
  dbrec3d_filter_grid_widget* kernel_widget_;
  QLabel* component_label_;
  dbrec3d_histogram_widget* hist_widget_;

  //: The kernels
  vcl_vector< vnl_vector<double> > kernels_;
  //: Iterator to current kernel
  vcl_vector< vnl_vector<double> >::iterator curr_kernel_it_;
  int curr_kernel_idx_; 
  
};

//: Use this for steerble, separable finters
class dbrec3d_steerable_basis_viewer : public QMainWindow
{
  Q_OBJECT
  
public:
  
  dbrec3d_steerable_basis_viewer(vcl_vector<vcl_vector<vcl_string> > basis, vcl_vector<vcl_string> basis_names,
                        vcl_map<vcl_string, vnl_vector_fixed<float,5> > separable_taps, float tf_min, float tf_max);
  
  public slots:
  
  void next_volume();
  void prev_volume();
  
  
protected:
  
  dbrec3d_filter_grid_widget* kernel_widget_;
  QLabel* component_label_;
  //dbrec3d_histogram_widget* hist_widget_;
  
  //: The kernels
  vcl_vector<vcl_vector<vcl_string> > basis_;
  //: Iterator to current kernel
  vcl_vector<vcl_vector<vcl_string> >::iterator curr_bases_it_;
  
  vcl_vector<vcl_string> basis_names_;
  vcl_vector<vcl_string> ::iterator curr_name_it_;
  
  vcl_map<vcl_string, vnl_vector_fixed<float,5> > separable_taps_;
  
};
#endif

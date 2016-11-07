// This is dbrec3d_pca_kernels_viewer.h
#ifndef dbrec3d_pca_kernels_viewer_h
#define dbrec3d_pca_kernels_viewer_h

//:
// \file
// \brief A viewer that iterates though the component vectors of a PCA matrix and displays them as volumes
// \author Isabel Restrepo mir@lems.brown.edu
// \date  11-Nov-2010.
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


class dbrec3d_pca_kernels_viewer : public QMainWindow
{
  Q_OBJECT
  
public:
  dbrec3d_pca_kernels_viewer(const vnl_matrix<double> &PC, const vnl_vector<double> &weights, 
                             unsigned dimx, unsigned dimy, unsigned dimz, float tf_min, float tf_max);
  //~dbrec3d_pca_kernels_viewer();
  
public slots:
  
  void next_volume();
  void prev_volume();
  
 
protected:

  dbrec3d_filter_grid_widget* pc_widget_;
  QLabel* component_label_;
  dbrec3d_histogram_widget* hist_widget_;
  dbrec3d_line_plot* weights_widget_;
  
  //: The principal components
  vnl_matrix<double> PC_;
  //vnl_vector<double> weights_;
  //: Index to the current principal component
  unsigned long curr_pc_;
  
  
  
};
#endif

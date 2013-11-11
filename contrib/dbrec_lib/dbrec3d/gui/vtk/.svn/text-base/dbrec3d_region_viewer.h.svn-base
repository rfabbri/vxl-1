// This is dbrec3d_region_viewer.h
#ifndef dbrec3d_region_viewer_h
#define dbrec3d_region_viewer_h

//:
// \file
// \brief A class to display a series of re 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  4-Oct-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <QMainWindow>

#include "dbrec3d_filter_grid_widget.h"

#include <boxm/boxm_scene.h>

class dbrec3d_region_viewer : public QMainWindow
{
  Q_OBJECT
  
public:
  dbrec3d_region_viewer(boxm_scene<boct_tree< short, float > > *scene, vcl_vector<vgl_box_3d<double> > bboxes, float tf_min, float tf_max, unsigned resolution_level = 0);
  
  public slots:
  
  void next_volume();
  void prev_volume();
  
protected:
  boxm_scene<boct_tree< short, float > > *scene_;
  dbrec3d_filter_grid_widget* region_widget_;

  //: The regions
  vcl_vector<vgl_box_3d<double> > bboxes_;
  //: Iterator to current kernel
  vcl_vector<vgl_box_3d<double> >::iterator curr_bboxes_it_;
  
  unsigned resolution_level_;
  double cell_length_;
  
};
#endif

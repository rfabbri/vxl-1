// This is brcv/dbsk2d/vis/dbsk2d_bline_view.h
#ifndef dbsk2d_bline_view_h_
#define dbsk2d_bline_view_h_

//:
// \file
// \brief A view of the boundary line including invalid regions
// \author Amir Tamrakar
// \date 03/21/2005
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <vgui/vgui_gl.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>

//: Renders the boundary line in openGL
class dbsk2d_bline_view
{
protected:
  dbsk2d_ishock_bline* _bl;

public:
  dbsk2d_bline_view(dbsk2d_ishock_bline* bl){ _bl = bl; }
  ~dbsk2d_bline_view(){}

  void draw(bool selected=false, bool draw_burnt_region=false) const;
  void draw_filled_rectangle(vgl_point_2d<double> spt, double u, double t0, double t1) const;

};

#endif //dbsk2d_bline_view_h_

// This is brcv/dbsk2d/vis/dbsk2d_bpoint_view.h
#ifndef dbsk2d_bpoint_view_h_
#define dbsk2d_bpoint_view_h_

//:
// \file
// \brief A view of the boundary point including invalid regions
// \author Amir Tamrakar
// \date 03/21/2005
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <vgui/vgui_gl.h>
#include <dbsk2d/dbsk2d_ishock_bpoint.h>

//: Renders the boundary point in openGL
class dbsk2d_bpoint_view
{
protected:
  dbsk2d_ishock_bpoint* _bp;

public:
  dbsk2d_bpoint_view(dbsk2d_ishock_bpoint* bp){ _bp = bp; }
  virtual ~dbsk2d_bpoint_view(){}

  void draw(bool selected=false, bool draw_burnt_region=false) const;
  void draw_filled_arc_sector(double x, double y, double r, double t0, double t1) const;

};

#endif //dbsk2d_bpoint_view_h_

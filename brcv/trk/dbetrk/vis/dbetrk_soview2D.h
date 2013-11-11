// This is brcv/trk/dbetrk/vis/dbetrk_soview2d.h
#ifndef dbetrk_soview2D_h_
#define dbetrk_soview2D_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief vgui_soview2D objects for dbetrk_edge
// \author Vishal Jain (vj@lems.brown.edu)
// \date 09/26/04
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------

#include <vcl_iosfwd.h>

#include <dbetrk/dbetrk_edge_sptr.h>
#include <vgui/vgui_soview2D.h>

//: This is a soview2D for a dbetrk_edge with a smart pointer 
class dbetrk_soview2D : public vgui_soview2D
{
 public:
  //: Constructor - creates a default view
  dbetrk_soview2D( dbetrk_edge_sptr const & edge_node);

  //: Destructor
  ~dbetrk_soview2D() {}

  //: Render on the display.
  virtual void draw() const;

  //: Print details to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('dbetrk_soview2D').
  vcl_string type_name() const { return "dbetrk_soview2D"; }

  //: Returns the distance squared from this ctrk_curve to the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the centroid.
  void get_centroid(float* x, float* y) const;

  //: Translate this soview2D by the given x and y distances.
  virtual void translate(float x, float y);

  //: Returns a smart pointer to the curve
  dbetrk_edge_sptr edgetrk_sptr() const { return dbetrk_edge_sptr_; }

protected:
  //: Smart pointer to a ctrk_tracker_curve
  dbetrk_edge_sptr dbetrk_edge_sptr_;
};


#endif // bgui_ctrk_soview2D_h_

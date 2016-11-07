// This is brl/bbas/bgui/dbctrk_soview2D.h
#ifndef dbctrk_soview2D_h_
#define dbctrk_soview2D_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief vgui_soview2D objects for dbctrk classes
// \author Vishal Jain (vj@lems.brown.edu)
// \date 5/26/04
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------

#include <vcl_iosfwd.h>

#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <vgui/vgui_soview2D.h>

//: This is a soview2D for a dbctrk_tracker_curve with a smart pointer 
class dbctrk_soview2D : public vgui_soview2D
{
 public:
  //: Constructor - creates a default view
  dbctrk_soview2D( dbctrk_tracker_curve_sptr const & dbctrk_curve);

  //: Destructor
  ~dbctrk_soview2D() {}

  //: Render on the display.
  virtual void draw() const;

  //: Print details to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('dbctrk_soview2D').
  vcl_string type_name() const { return "dbctrk_soview2D"; }

  //: Returns the distance squared from this dbctrk_curve to the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the centroid.
  void get_centroid(float* x, float* y) const;

  //: Translate this soview2D by the given x and y distances.
  virtual void translate(float x, float y);

  //: Returns a smart pointer to the curve
  dbctrk_tracker_curve_sptr dbctrk_sptr() const { return dbctrk_sptr_; }

protected:
  //: Smart pointer to a dbctrk_tracker_curve
  dbctrk_tracker_curve_sptr dbctrk_sptr_;
};


#endif // dbctrk_soview2D_h_

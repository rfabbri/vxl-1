// This is brcv/seg/dbdet/vis/dbdet_keypoint_soview2D.h
#ifndef dbdet_keypoint_soview2D_h_
#define dbdet_keypoint_soview2D_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A linked Lowe keypoint vgui_soview2D object
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 11/16/03
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------

#include <vcl_iosfwd.h>

#include <dbdet/dbdet_keypoint_sptr.h>

#include <vgui/vgui_soview2D.h>


class dbdet_keypoint_soview2D : public vgui_soview2D //_linestrip
{
 public:
  //: Constructor - creates a default view
  dbdet_keypoint_soview2D( dbdet_keypoint_sptr const & kp, bool box=false);

  ~dbdet_keypoint_soview2D() {}

  //: Render the keypoint on the display.
  virtual void draw() const;

  void set_point_style(const vgui_style_sptr& newstyle){ point_style_ = newstyle; }

  //: Print details about this keypoint to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('dbdet_keypoint_soview2D').
  vcl_string type_name() const { return "dbdet_keypoint_soview2D"; }

  //: Returns the distance squared from this keypoint to the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the centroid of this keypoint.
  void get_centroid(float* x, float* y) const;

  //: Translate this keypoint by the given x and y distances.
  void translate(float x, float y);

  //: Smart pointer to a dbdet_keypoint
  dbdet_keypoint_sptr sptr;

private:
  //: Determines whether a box is draw around the keypoint or not
  bool draw_box_;

  //: The style of the points
  vgui_style_sptr point_style_;
};


#endif // dbdet_keypoint_soview2D_h_

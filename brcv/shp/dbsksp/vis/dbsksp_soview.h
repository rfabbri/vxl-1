// This is brl/bbas/bgui/dbsksp_soview.h
#ifndef dbsksp_soview_h_
#define dbsksp_soview_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief soview for dbsksp objects
// \author Nhon Trinh
// \date Oct 3, 2006
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------


#include <vgui/vgui_soview2D.h>


class dbsksp_soview : public vgui_soview2D
{
 public:
  //: Constructor
   dbsksp_soview(): vgui_soview2D(){};

  //: Destructor
  virtual ~dbsksp_soview() {}

  //: Print details about this vsol object to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class
  virtual vcl_string type_name() const=0;

  //: Render this 2D digital_curve on the display.
  virtual void draw() const=0;

  //: Translate this 2D digital_curve by the given x and y distances.
  virtual void translate(float x, float y)=0;

  //: Returns the distance squared of this soview2D from the given position.
  virtual float distance_squared(float x, float y) const = 0;
 
  //: Returns the centroid of this soview2D.
  virtual void get_centroid(float* x, float* y) const = 0;
 
protected:

};


#endif // dbsksp_soview_h_

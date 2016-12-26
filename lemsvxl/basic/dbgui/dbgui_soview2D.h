// This is dbgui/dbgui_soview2D.h
#ifndef dbgui_soview2D_h_
#define dbgui_soview2D_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A collection of vgui_soview2D objects
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date April 24, 2008
// \verbatim
//  Modifications:
//
// \endverbatim
//--------------------------------------------------------------------------------


#include <vgui/vgui_soview2D.h>
#include <dbgl/algo/dbgl_circ_arc.h>


//: display a circular arc object
class dbgui_soview2D_circ_arc : public vgui_soview2D
{
 public:
   //: Constructor
   dbgui_soview2D_circ_arc(const dbgl_circ_arc& circ_arc) : circ_arc_(circ_arc){};

   //: Constructor
   dbgui_soview2D_circ_arc(float start_x, float start_y, float end_x, float end_y, float k);

  //: Destructor
  virtual ~dbgui_soview2D_circ_arc() {}

  //: Returns the type of this class
  virtual vcl_string type_name() const {return "dbgui_soview_circ_arc"; }

  //: Render this node on the display.
  virtual void draw() const;

  //: Translate this node by the given x and y distances.
  virtual void translate(float x, float y);

  //: Returns the distance squared of this soview2D from the given position.
  virtual float distance_squared(float x, float y) const;
 
  //: Returns the centroid of this soview2D.
  virtual void get_centroid(float* x, float* y) const;

  //: Print details about this object to the given stream.
  virtual vcl_ostream& print(vcl_ostream& os) const;

  //: Return a smart pointer to the node
  dbgl_circ_arc circ_arc() const {return this->circ_arc_; }
 
protected:
  dbgl_circ_arc circ_arc_;

};


#endif // dbgui_soview2D_h_

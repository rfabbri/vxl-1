// This is dbgui/dbgui_shockshape_tableau.h
#ifndef dbgui_shockshape_tableau_h_
#define dbgui_shockshape_tableau_h_
//:
// \file
// \brief  Tableau for shape modelling
// \author Nhon Trinh (ntrinh@lems.brown.edu
// \date 10/30/04
//
//  Contains classes: dbgui_shockshape_tableau  dbgui_shockshape_tableau_new


#include "dbgui_shockshape_tableau_sptr.h"
#include <vgui/vgui_tableau.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <vgui/vgui_text_tableau.h>
//#include <vgl/vgl_point_2d.h>
//#include <bvrl/bvrl_region_sptr.h>
//#include <bvrl/bvrl_transform_2d_sptr.h>


class dbgui_shockshape_tableau : public bgui_vsol2D_tableau //vgui_tableau
{
 public:
  //: Constructor - don't use this, use dbgui_shockshape_tableau_new.
  //  The child tableau is added later using add_child.
  dbgui_shockshape_tableau();

  //: Returns the type of tableau ('dbgui_shockshape_tableau').
  vcl_string type_name() const;

  //: Handle all events sent to this tableau.
  // bool handle(vgui_event const &);


 protected:
  // Destructor - called by dbgui_shockshape_tableau_sptr.
  ~dbgui_shockshape_tableau();
};

//: Create a smart-pointer to a dbgui_shockshape_tableau.
struct dbgui_shockshape_tableau_new : public dbgui_shockshape_tableau_sptr {
  typedef dbgui_shockshape_tableau_sptr base;

  //: Constructor - creates a default dbgui_shockshape_tableau.
  dbgui_shockshape_tableau_new() : base(new dbgui_shockshape_tableau()) { }
};

#endif // dbgui_shockshape_tableau_h_

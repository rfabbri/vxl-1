// This is brcv/mvg/dbvrl/vis/dbvrl_region_tableau.h
#ifndef dbvrl_region_tableau_h_
#define dbvrl_region_tableau_h_
//:
// \file
// \brief  Tableau that performs a 2d tranformation 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 4/22/04
//
//  Contains classes: dbvrl_region_tableau  dbvrl_region_tableau_new


#include "dbvrl_region_tableau_sptr.h"
#include <vcl_vector.h>
#include <vgui/vgui_tableau.h>
#include <vgl/vgl_point_2d.h>
#include <dbvrl/dbvrl_region_sptr.h>
#include <dbvrl/dbvrl_transform_2d_sptr.h>


class dbvrl_region_tableau : public vgui_tableau
{
 public:
  //: Constructor - don't use this, use dbvrl_region_tableau_new.
  //  The child tableau is added later using add_child.
  dbvrl_region_tableau();

  //: Returns the type of tableau ('dbvrl_region_tableau').
  vcl_string type_name() const;

  //: draw the grid for the given transformation
  void draw_grid( const vcl_vector<vgl_point_2d<double> >& verts, 
                  unsigned int nx, unsigned int ny );

  //: Handle all events sent to this tableau.
  bool handle(vgui_event const &);

  //: Set the 2d homography
  void set_transform(const dbvrl_transform_2d_sptr& xform);

  //: Set the region
  void set_region(const dbvrl_region_sptr& region);

  //: Get the bounding box for this tableau.
  bool get_bounding_box(float low[3], float high[3]) const;

 protected:
  // Destructor - called by dbvrl_region_tableau_sptr.
  ~dbvrl_region_tableau();

  dbvrl_transform_2d_sptr xform_;

  dbvrl_region_sptr region_;
};

//: Create a smart-pointer to a dbvrl_region_tableau.
struct dbvrl_region_tableau_new : public dbvrl_region_tableau_sptr {
  typedef dbvrl_region_tableau_sptr base;

  //: Constructor - creates a default dbvrl_region_tableau.
  dbvrl_region_tableau_new() : base(new dbvrl_region_tableau()) { }
};

#endif // dbvrl_region_tableau_h_

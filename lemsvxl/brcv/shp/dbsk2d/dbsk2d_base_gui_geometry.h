// This is brcv/shp/dbsk2d/dbsk2d_base_gui_geometry.h
#ifndef dbsk2d_base_gui_geometry_h_
#define dbsk2d_base_gui_geometry_h_
//:
// \file
// \brief Base class for all geometry in this library that
// need to be displayed
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
// \endverbatim

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>

//: This class does not exist in vxl nor is it needed
// It is only for backward compatibility with a previous
// GUI application
class GraphicsNode;

//: Common base class for all boundary and shock elements
// It exists only to store extrinsic points for rendering purposes.
// In the future, view classes for the objects will 
// render this class obsolete 
// \todo Define view classes for all the elements.
class dbsk2d_base_gui_geometry
{
  //: pointer to the graphics element (Not for VXL)
  GraphicsNode *GUIItem;

public :

  //: constructor
  dbsk2d_base_gui_geometry(): GUIItem (0) {ex_pts_.clear();}

  //: destructor (delete all the stored extrinsic points}
  virtual ~dbsk2d_base_gui_geometry(){ex_pts_.clear();}

  //: return the extrinsic points for rendering this geometry
  virtual vcl_vector<vgl_point_2d<double> >& ex_pts() { return ex_pts_; }

  //: compute the extrinsic locus of this element for easier rendering
  virtual void compute_extrinsic_locus()=0;

  virtual GraphicsNode* getGUIItem() { return GUIItem; }  

  //: Return some information about the element
  virtual void getInfo (vcl_ostream& ostrm=vcl_cout){};

protected:

  //: extrinsic points for drawing purposes
  vcl_vector<vgl_point_2d<double> > ex_pts_;

};

#endif //dbsk2d_base_gui_geometry_h_

// This is dbdet_sel_knot.h
#ifndef dbdet_sel_knot_h
#define dbdet_sel_knot_h
//:
//\file
//\brief A data structure for representing knot points
//\author Amir Tamrakar
//\date 08/08/08
//
//\verbatim
//  Modifications
//
//\endverbatim

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_deque.h>
#include <vnl/vnl_math.h>
#include <vbl/vbl_array_2d.h>

#include <vgl/vgl_point_2d.h>
#include <dbdet/sel/dbdet_curvelet.h>
#include <dbdet/sel/dbdet_curve_model.h>

class dbdet_sel_knot
{
public:
  //dbdet_curvelet L_cvlet;
  //dbdet_curvelet R_cvlet;

  unsigned nB; ///< # of edgels before anchor
  unsigned nA; ///< # of edgels after anchor

  //: default constructor
  dbdet_sel_knot(unsigned nb=0, unsigned na=0): nB(nb), nA(na) {}

  //: destructor
  virtual ~dbdet_sel_knot(){}

};

#endif // dbdet_sel_knot_h

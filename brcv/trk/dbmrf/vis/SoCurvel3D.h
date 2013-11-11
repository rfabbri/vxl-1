// This is brcv/trk/dbmrf/vis/SoCurvel3D.h
#ifndef SoCurvel3D_h_
#define SoCurvel3D_h_

//:
// \file
// \brief This file defines new Coin3D nodes with backward pointers to bmrf objects
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/7/04 
//
// \verbatim
//  Modifications
// \endverbatim


#include <Inventor/nodes/SoLineSet.h>
#include <bmrf/bmrf_curve_3d_sptr.h>


//: The Customized SoLineSet with backward pointer
class SoCurvel3D : public SoLineSet
{

  SO_NODE_HEADER(SoCurvel3D);

public:
  //: Constructor
  SoCurvel3D(const bmrf_curve_3d_sptr& curve=NULL);

  //: Accessor function 
  bmrf_curve_3d_sptr curve() const { return curve_; }

  void set_highlight(bool value) { highlight_ = value; this->touch(); }

  bool highlight() const { return highlight_; }

  //: Initializes this class
  static void initClass();

  virtual void GLRender(SoGLRenderAction *action);
  
private:
  //: back pointer to the curve object
  bmrf_curve_3d_sptr curve_;

  //: a flag to highlight the curvel
  bool highlight_;

  virtual ~SoCurvel3D(){}
};


#endif // SoCurvel3D_h_

// This is brcv/trk/dbmrf/vis/SoCurvel3D.cxx
//:
// \file


#include "SoCurvel3D.h"
#include <Inventor/elements/SoLineWidthElement.h>
#include <Inventor/actions/SoGLRenderAction.h>


SO_NODE_SOURCE(SoCurvel3D);


//: Constructor
SoCurvel3D::SoCurvel3D(const bmrf_curve_3d_sptr& curve)
 : curve_(curve), highlight_(false)
{
  SO_NODE_CONSTRUCTOR(SoCurvel3D);
}


void
SoCurvel3D::initClass()
{
  //Initialize type id variables
  SO_NODE_INIT_CLASS(SoCurvel3D, SoLineSet, "SoLineSet");
}


void 
SoCurvel3D::GLRender(SoGLRenderAction *action)
{
  if(highlight_){
    float width = SoLineWidthElement::get(action->getState()); 
    SoLineWidthElement::set(action->getState(),this,3);
    SoLineSet::GLRender(action);
    SoLineWidthElement::set(action->getState(),this,width);
    highlight_ = false;
  }
  else
    SoLineSet::GLRender(action);
}

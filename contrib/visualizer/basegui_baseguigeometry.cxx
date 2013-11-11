#include "basegui_baseguigeometry.h"

//==============================================
//THIS IS THE ONLY GUI WE HAVE TO INCLUDE
//We need this method here because deleteself function is
//only defined in nodes.h

#include "basegui_nodes.h"

BaseGUIGeometry::~BaseGUIGeometry ()
{
  if (GUIItem) {
    //delete GUIItem;
    GUIItem->deleteSelf();
  }

  //delete all the stored extrinsic information
  ExPts.clear();
}
//==============================================

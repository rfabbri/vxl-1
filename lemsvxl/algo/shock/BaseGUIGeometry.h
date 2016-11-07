#ifndef _BASE_GUI_GEOMETRY_H_
#define _BASE_GUI_GEOMETRY_H_

#include <extrautils/points.h>
#include <vcl_vector.h>

//==============================================
//THIS IS THE ONLY GUI WE HAVE TO INCLUDE
class GraphicsNode;
class BaseGUIGeometry  //common base for elements
{
public :
  GraphicsNode *GUIItem;
  vcl_vector<Point2D<double> > ExPts;  //extrinsic points for drawing purposes

      BaseGUIGeometry() { 
          GUIItem = 0; 
        ExPts.clear();
      }
      virtual ~BaseGUIGeometry(){
        //delete all the stored extrinsic information
        ExPts.clear();
      }
  virtual void compute_extrinsic_locus(){}

  virtual GraphicsNode* getGUIItem() { return GUIItem; }
};
//==============================================

#endif

// This is brcv/shp/dbsk2d/dbsk2d_visual_fragment.h
#ifndef dbsk2d_visual_fragment_h_
#define dbsk2d_visual_fragment_h_
//:
// \file
// \brief Visual Fragment class
// \author Amir Tamrakar
// \date 06/21/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 06/21/2005    Initial version.
// \endverbatim

#include <vcl_list.h>
#include <dbsk2d/dbsk2d_base_gui_geometry.h>

#include <dbsk2d/dbsk2d_shock_node.h>
#include <dbsk2d/dbsk2d_shock_edge.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

#include <dbsk2d/dbsk2d_shock_fragment.h>

//: Visual Fragment class
class dbsk2d_visual_fragment : public dbsk2d_base_gui_geometry
{
public:

  //: Constructor
  dbsk2d_visual_fragment() {}

  //: Destructor
  virtual ~dbsk2d_visual_fragment() {}

  int id(){return id_;}
  void set_id(int id){id_ = id;}

  //: compute the extrinsic locus of this element for easier rendering
  virtual void compute_extrinsic_locus(){}

protected:

  int id_;

  //dbsk2d_bnd_contour* _left_contour;    ///< the left boundary curve
  //dbsk2d_bnd_contour* _right_contour;   ///< the right boundary curve

  //: visual fragments are gropued shock fragments and therefore may need
  //  to store an entire subgraph
  dbsk2d_shock_graph_sptr shock_graph_;    

};

#endif // dbsk2d_visual_fragment_h_

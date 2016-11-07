// This is brcv/shp/dbsk2d/dbsk2d_shock_edge.cxx

//:
// \file

#include <dbsk2d/dbsk2d_shock_edge.h>

//: Constructor
dbsk2d_shock_edge::dbsk2d_shock_edge(dbsk2d_shock_node_sptr src_node, 
                                     dbsk2d_shock_node_sptr tgt_node) : 
  dbsk2d_base_gui_geometry(), dbgrl_edge<dbsk2d_shock_node>(src_node, tgt_node), 
  id_(-1), left_contour_(0), right_contour_(0),
  LsEta_(-1), LeEta_(-1), RsEta_(-1), ReEta_(-1), fragment_(0)
{
}

//: set the left contour
void dbsk2d_shock_edge::set_left_contour(dbsk2d_bnd_contour_sptr contour, 
                                         double lseta, double leeta) 
{ 
  left_contour_ = contour;
  LsEta_ = lseta;
  LeEta_ = leeta;
}


//: set the right contour
void dbsk2d_shock_edge::set_right_contour(dbsk2d_bnd_contour_sptr contour, 
                                          double rseta, double reeta) 
{ 
  right_contour_ = contour;
  RsEta_ = rseta;
  ReEta_ = reeta;
}

//: map from intrinsic to extrinsic coordinates
vgl_point_2d<double> 
dbsk2d_shock_edge::get_ex_coords(double psi, double t)
{
  if (t>=0)
    return _translatePoint(pt(psi), tangent(psi)+phi(psi), t);
  else
    return _translatePoint(pt(psi), tangent(psi)-phi(psi), -t);
}

//: Return some information about the element
void dbsk2d_shock_edge::getInfo (vcl_ostream& ostrm) 
{ 
  ostrm << "E: [" << id_ << "]" << vcl_endl; 
  ostrm << "[ pN: " << source()->id() << ", cN: " << (target()?target()->id():-1) << " ]\n" << vcl_endl;
}


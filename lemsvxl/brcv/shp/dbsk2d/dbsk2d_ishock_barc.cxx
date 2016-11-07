// This is brcv/shp/dbsk2d/dbsk2d_ishock_barc.cxx

//:
// \file

#include "dbsk2d_ishock_barc.h"

#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>
#include <vcl_cstdio.h>

#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_bnd_edge.h>

//-------------------------------------------------------------------
//: Constructor
dbsk2d_ishock_barc::
dbsk2d_ishock_barc (dbsk2d_ishock_bpoint* startpt, 
                    dbsk2d_ishock_bpoint* endpt, 
                    int id, 
                    bool bGUI, 
                    vgl_point_2d<double> center, 
                    double r, 
                    ARC_NUD nud): 
dbsk2d_ishock_bcurve(startpt, endpt, id, bGUI),
_center(center),
_R(r),
_nud(nud)
{
  _type = BARC;
  this->compute_cached_params();

  //now we need to link the BPOINTs to the line
  s_pt()->connectTo(this);
  e_pt()->connectTo(this);

  // for gui purpose
  compute_extrinsic_locus();
}


//: compute local copies of commonly used parameters
void dbsk2d_ishock_barc::
compute_cached_params()
{
  _StartVector = angle0To2Pi(_vPointPoint(_center, this->start()));
  _EndVector = angle0To2Pi(_vPointPoint(_center, this->end()));

  if (_nud==ARC_NUD_CCW) {
    _CCWStartVector  = _StartVector;
    _CCWEndVector    = _EndVector;
    _InTangent = angle0To2Pi (_StartVector + vnl_math::pi/2);
    _OutTangent = angle0To2Pi (_EndVector + vnl_math::pi/2);
  }
  else {
    _CCWStartVector  = _EndVector;
    _CCWEndVector    = _StartVector;
    _InTangent = angle0To2Pi (_StartVector - vnl_math::pi/2);
    _OutTangent = angle0To2Pi (_EndVector - vnl_math::pi/2);
  }
  _bAcross = (_CCWStartVector>_CCWEndVector);

  return;
}


//-------------------------------------------------------------------
//: Compute the bounding box of this arc
void dbsk2d_ishock_barc::
compute_bounding_box(vbl_bounding_box<double, 2 >& box ) const
{
  // initialize the bounding box with end points of the arc
  double min_x = vcl_min(this->start().x(), this->end().x());
  double min_y = vcl_min(this->start().y(), this->end().y());
  double max_x = vcl_max(this->start().x(), this->end().x());
  double max_y = vcl_max(this->start().y(), this->end().y());
  
  // sweeping angle of the arc
  double arc_angle = CCW(this->CCWStartVector(), this->CCWEndVector());
  
  // check if 4 extreme points of the circle are inside `this' arc.
  // this will determine the bounding box of the arc
  // right point
  if (CCW(this->CCWStartVector(), 0) < arc_angle)
    max_x = this->center().x() + this->R();

  // top point
  if (CCW(this->CCWStartVector(), vnl_math::pi_over_2) < arc_angle)
    max_y = this->center().y() + this->R();

  // left point
  if (CCW(this->CCWStartVector(), vnl_math::pi) < arc_angle)
    min_x = this->center().x() - this->R();

  // bottom point
  if (CCW(this->CCWStartVector(), -vnl_math::pi_over_2) < arc_angle)
    min_y = this->center().y() - this->R();

  // set range of bounding box
  box.update(min_x, min_y);
  box.update(max_x, max_y);
  return;
}

//: convert a vector to an eta value
double dbsk2d_ishock_barc::vec_to_eta(VECTOR_TYPE vec)
{
  //Note: depending the direction of the half arc, the eta can be CW or CCW
  double eta;

  if (_nud>0) //CW arc
    eta = CCW(vec, _StartVector);
  else
    eta = CCW(_StartVector, vec);

  //However, if the etas are not in the eta range, they need to be made negative
  //   .        
  //                  
  //     .--->--       (eta<0 to mirror the convention for the line)
  //
  //
  //            .        
  //                  
  //   --->--.        (eta > max_eta)
  //

  //Note: assuming that all arc segments are smaller than a half circle
  if (eta>vnl_math::pi)
    eta = eta - 2*vnl_math::pi;

  return eta;
}

//: convert an eta value to a vector
VECTOR_TYPE dbsk2d_ishock_barc::eta_to_vec(double eta)
{
  return angle0To2Pi(_StartVector - _nud*eta);
}

//-------------------------------------------------------------------
//: Return tangent angle [0, 2pi) of curve given arc-length
VECTOR_TYPE dbsk2d_ishock_barc::
tangent_at(double s) const
{
  return angle0To2Pi(this->InTangentAtStartPt() + s*this->curvature());
}

// -------------------------------------------------------------------
//: Return reverse tangent angle [0, 2pi) of curve given arc-length
VECTOR_TYPE dbsk2d_ishock_barc::
reverse_tangent_at(double s) const
{
  return angle0To2Pi(this->InTangentAtStartPt()+s*this->curvature()+ 
    vnl_math::pi);
}

//-------------------------------------------------------------------
//EPSILONISSUE 21
//Remember to recompute _center and _R for this dbsk2d_ishock_barc
//_StartVectors, _EndVectors, too
//Here we fix _R, recompute the new _center
void dbsk2d_ishock_barc::
reconnect(dbsk2d_ishock_bpoint* oldPt, dbsk2d_ishock_bpoint* newPt)
{
  if(oldPt==s_pt()){

    //startPt = newPt;
    this->set_s_pt(newPt);

    _StartVector = _vPointPoint(_center, this->start());
  }
  else {
    dbsk2d_assert (oldPt==e_pt());
    //endPt = newPt;
    this->set_e_pt(newPt);

    _EndVector = _vPointPoint(_center, this->end());
  }
  _center = getCenterOfArc (this->start().x(), this->start().y(), this->end().x(), this->end().y(), _R, _nud, ARC_NUS_SMALL);

  if (_nud==ARC_NUD_CCW) {
    _CCWStartVector  = _StartVector;
    _CCWEndVector    = _EndVector;
    _InTangent = angle0To2Pi (_StartVector + vnl_math::pi/2);
    _OutTangent = angle0To2Pi (_EndVector + vnl_math::pi/2);
  }
  else {
    _CCWStartVector  = _EndVector;
    _CCWEndVector    = _StartVector;
    _InTangent = angle0To2Pi (_StartVector - vnl_math::pi/2);
    _OutTangent = angle0To2Pi (_EndVector - vnl_math::pi/2);
  }
  _bAcross = (_CCWStartVector>_CCWEndVector);
}


void dbsk2d_ishock_barc::getInfo (vcl_ostream& ostrm)
{
  char s[1024];

  ostrm << "\n==============================\n";
  ostrm << "BA: [" << _id << "] Twin: [" << this->twinArc()->id() << "] s_pt: [";
  ostrm << this->s_pt()->id() << "] e_pt: [" << this->e_pt()->id() << "]\n";
  vcl_sprintf (s, 
    "S-E:(%.3f, %.3f)-(%.3f, %.3f)\n", 
    this->start().x(), this->start().y(), 
    this->end().x(),   this->end().y()); 
  ostrm << s;

  vcl_sprintf (s, 
    "C:(%.3f, %.3f)\n", 
    this->center().x(), this->center().y()); 
  ostrm << s;

  vcl_sprintf (s, "R: %.5f, k: %.5f\n", _R, 1/_R); ostrm<<s;
  vcl_sprintf (s, "nud: %s, ", (_nud==ARC_NUD_CW) ? "1 (CW)" : "-1 (CCW)"); ostrm<<s;

  vcl_sprintf (s, 
    "Sv-Ev: (%.5f) - (%.5f), Degrees: (%.5f) - (%.5f)\n", 
    _StartVector, _EndVector, _StartVector*180/vnl_math::pi, _EndVector*180/vnl_math::pi); 
  ostrm << s;

  vcl_sprintf (s, "length: %.5f\n", this->l()); ostrm<<s;
  vcl_sprintf (s, "bGUIElm: %s\n\n", _bGUIElm ? "yes" : "no"); ostrm<<s;

  //bnd_ishock_map
  bnd_ishock_map_iter curS = shock_map_.begin();
  ostrm << "ShockMap: [" << id() << "]" << vcl_endl;
  for (; curS!=shock_map_.end(); ++curS){
    vcl_sprintf (s, "%.5f -> %d (%s)\n", 
      curS->first.s_eta, curS->second->id(), 
      (curS->first).type_string().c_str()); 
    ostrm<<s;
  }
  ostrm << "\n";

  dbsk2d_ishock_belm* twL = twinArc();
  ostrm << "Twin ShockMap: [" << twL->id() << "]" << vcl_endl;
  for (curS = twL->shock_map().begin(); curS!=twL->shock_map().end(); ++curS){
    vcl_sprintf (s, "%.5f -> %d (%s)\n", 
      curS->first.s_eta, curS->second->id(), 
      (curS->first).type_string().c_str()); 
    ostrm<<s;
  }
  ostrm << "\n";

  dbsk2d_ishock_belm* sp = s_pt();
  ostrm << "SPT ShockMap [" << sp->id() << "]" << vcl_endl;
  for (curS = sp->shock_map().begin(); curS!=sp->shock_map().end(); ++curS){
    vcl_sprintf (s, "%.5f -> %d (%s)\n", 
      curS->first.s_eta, curS->second->id(), 
      (curS->first).type_string().c_str()); 
    ostrm<<s;
  }
  ostrm << "\n";

  dbsk2d_ishock_belm* ep = e_pt();
  ostrm << "EPT ShockMap [" << ep->id() << "]" << vcl_endl;
  for (curS = ep->shock_map().begin(); curS!=ep->shock_map().end(); ++curS){
    vcl_sprintf (s, "%.5f -> %d (%s)\n", 
      curS->first.s_eta, curS->second->id(), 
      (curS->first).type_string().c_str());
    ostrm<<s;
  }
  ostrm << "\n";
}


void dbsk2d_ishock_barc::
compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ex_pts_.clear();

  double startVector = this->_CCWStartVector;//_StartVector;
  double endVector = this->_CCWEndVector; //_EndVector;

  //bridge across discontinuity
  if (endVector < startVector) endVector += 2*vnl_math::pi;

  const int NUM_ELLIPSE_SUBDIVISIONS = 100;
  int n_line_segs = int(NUM_ELLIPSE_SUBDIVISIONS*vcl_fabs(endVector-startVector)/(2*vnl_math::pi));
  if(n_line_segs < 4) n_line_segs = 4;

  for(int i = 0; i < n_line_segs; ++i) {
    double v = startVector + (endVector-startVector)*i/double(n_line_segs-1);
    ex_pts_.push_back(vgl_point_2d<double>(_center.x()+_R*vcl_cos(v), _center.y()+_R*vcl_sin(v)));
  }
}


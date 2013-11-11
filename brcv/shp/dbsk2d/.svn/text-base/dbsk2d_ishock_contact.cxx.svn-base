// This is brcv/shp/dbsk2d/dbsk2d_ishock_contact.cxx

//:
// \file

#include <vcl_cstdio.h>
#include <dbsk2d/dbsk2d_ishock_contact.h>
#include <dbsk2d/dbsk2d_lagrangian_cell_bnd.h>

dbsk2d_ishock_contact::dbsk2d_ishock_contact (int newid, dbsk2d_ishock_belm* lbe, 
                                              dbsk2d_ishock_belm* rbe,
                                              vgl_point_2d<double> origin, double n,
                                              double lstau, double rstau,
                                              double lsEta, double rsEta) :
    dbsk2d_ishock_edge (newid, 0, NULL, lbe, rbe) 
{
  _type = CONTACTSHOCK;
  _H = 0;
  _n = n;
  _origin = origin;

  _LsEta = lsEta;
  _RsEta = rsEta;

  //this might have to be updated due to the fact that lineline contacts can be a wedge
  _LsTau = lstau;
  _RsTau = rstau;

  _LeTau=_LsTau; //for contact shock, tau is constant
  _ReTau=_RsTau; //for contact shock, tau is constant
  
  compute_tau_ranges();
}

//: compute the range of the intrinsic parameters
void dbsk2d_ishock_contact::compute_tau_ranges()
{
  _minLTau = _LsTau;
  _maxLTau = _LsTau; 
  _minRTau = _RsTau; 
  _maxRTau = _RsTau; 
  
  dbsk2d_assert(_minLTau<=_maxLTau && _minRTau<=_maxRTau);
}

//tau and eta conversion functions
double dbsk2d_ishock_contact::EtaToTau(double eta, DIRECTION dir, bool constrained)
{
  //always return the tau no matter what the eta (can this come back to haunt me??)
  if (dir==LEFT)
    return _LsTau;
  else
    return _RsTau;
}

void dbsk2d_ishock_contact::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ex_pts_.clear();

  vgl_point_2d<double> start = getStartPt ();
  vgl_point_2d<double> end;

  //if (_simTime == _startTime)
  //  end = getStartPt();
  //else 
    if (endTime() > MAX_RADIUS) {
    //compute projected End point
    end = start + LARGE_DRAWLENGTH*vgl_vector_2d<double>(vcl_cos(_n), vcl_sin(_n));
  } 
  else
    end = getEndPt();
  
  ex_pts_.push_back(start);
  ex_pts_.push_back(end);
}

void dbsk2d_ishock_contact::getInfo (vcl_ostream& ostrm)
{
  char s[1024];
  char endx[32], endy[32], simtime[32], endtime[32];

  ostrm << "\n==============================\n";
  ostrm << "C: [" << _id << "] " << "{ "  << (_bActive ? "A" : "nA" ) << ", ";
  ostrm << (_bPropagated ? "Prop" : "nProp" ) << ", (C)  }" << vcl_endl;

  vgl_point_2d<double> start = getStartPt ();
  vgl_point_2d<double> end = getEndPt ();

  if (_endTime==ISHOCK_DIST_HUGE) {
    vcl_sprintf(endtime, "INF");
    vcl_sprintf(endx, "INF");
    vcl_sprintf(endy, "INF");
  }
  else {
    vcl_sprintf(endtime, "%.7f", _endTime);
    vcl_sprintf(endx, "%.3f", end.x());
    vcl_sprintf(endy, "%.3f", end.y());
  }

  if (_simTime==ISHOCK_DIST_HUGE) 
    vcl_sprintf(simtime, "INF");
  else 
    vcl_sprintf(simtime, "%.7f", _simTime);

  vcl_sprintf(s, "Origin : (%.3f, %.3f)\n", _origin.x(), _origin.y()); ostrm << s;
  vcl_sprintf(s, "Sta-End: (%.3f, %.3f)-(%s, %s)\n", start.x(), start.y(), endx, endy); ostrm << s;
  vcl_sprintf(s, "{ ts=%.7f, te=%s, tsim=%s }\n", _startTime, endtime, simtime); ostrm << s <<vcl_endl;

  ostrm << "lB: " << _lBElement->id(); vcl_sprintf(s, " (%f, %f)\n", LsEta(), LeEta()); ostrm <<s;
  ostrm << "rB: " << _rBElement->id(); vcl_sprintf(s, " (%f, %f)\n", RsEta(), ReEta()); ostrm <<s;
  ostrm << "[ lS: " << (_lShock?_lShock->id():-1)  << ", rS: " << (_rShock?_rShock->id():-1) << "]\n";
  ostrm << "[ lN: " << (_lNeighbor?_lNeighbor->id():-1)  << ", rN: " << (_rNeighbor?_rNeighbor->id():-1);
  ostrm << ", pN: -1" << ", cN: " << (_cSNode?_cSNode->id():-1) << " ]\n" << vcl_endl;

  vcl_sprintf(s, "LTau: %f - %f (Tau range: %f - %f)\n", _LsTau, _LeTau, minLTau(), maxLTau()); ostrm << s;
  vcl_sprintf(s, "RTau: %f - %f (Tau range: %f - %f)\n\n", _RsTau, _ReTau, minRTau(), maxRTau()); ostrm << s;

  vcl_sprintf(s, "LEta: %f - %f (Eta range: %f - %f)\n", LsEta(), LeEta(), _lBElement->min_eta(), _lBElement->max_eta()); ostrm << s;
  vcl_sprintf(s, "REta: %f - %f (Eta range: %f - %f)\n\n", RsEta(), ReEta(), _rBElement->min_eta(), _rBElement->max_eta()); ostrm << s;

  vcl_sprintf(s, "H: %f\n", _H); ostrm << s;
  vcl_sprintf(s, "n: %f\n", _n); ostrm << s;

  //boundary intersection
  ostrm << "Termination: ";
  if (_cSNode)
    ostrm << "at intersection." << vcl_endl;
  else {
    if (this->cell_bnd()){
      if (this->cell_bnd()->is_vert())
        ostrm << "at vert. bnd; y=" << this->bnd_intersect_pos() << vcl_endl;
      else        
        ostrm << "at horiz. bnd; x=" << this->bnd_intersect_pos() << vcl_endl;
    }
    else
      ostrm << "in mid air." << vcl_endl;
  }
}


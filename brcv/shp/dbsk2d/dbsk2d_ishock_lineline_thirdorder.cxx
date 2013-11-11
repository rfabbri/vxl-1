// This is brcv/shp/dbsk2d/dbsk2d_ishock_lineline_thirdorder.cxx

//:
// \file

#include <vcl_cstdio.h>
#include <dbsk2d/dbsk2d_ishock_lineline_thirdorder.h>
#include <dbsk2d/dbsk2d_lagrangian_cell_bnd.h>

dbsk2d_ishock_lineline_thirdorder::dbsk2d_ishock_lineline_thirdorder (
  int newid, double stime, dbsk2d_ishock_node* pse,
  dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,
  double lsEta, double rsEta) :
  dbsk2d_ishock_edge (newid, stime, pse, lbe, rbe) 
{
  _type = LINELINE_THIRDORDER;
  _endTime = _startTime;

  _Al = ((dbsk2d_ishock_bline *)lbe)->start();
  _Bl = ((dbsk2d_ishock_bline *)lbe)->end();
  _Ar = ((dbsk2d_ishock_bline *)rbe)->start();
  _Br = ((dbsk2d_ishock_bline *)rbe)->end();

  _lL = ((dbsk2d_ishock_bline *)lbe)->l();
  _lR = ((dbsk2d_ishock_bline *)rbe)->l();

  _nl = vcl_atan2(_Bl.y() - _Al.y(), _Bl.x() - _Al.x());
  _ul = angle0To2Pi (_nl- vnl_math::pi_over_2);

  //distance between the lines
  //_H = (_distPointLine(_Al, _Ar, _Br) + _distPointLine(_Bl, _Ar, _Br) )/2.0 ;
  _H = 2.0*_startTime;

  _origin = _Al - 0.5*_H*vgl_vector_2d<double>(vcl_cos(_ul), vcl_sin(_ul));

  _LsTau = lsEta;
  _RsTau = rsEta;

  //now bring the left and right parameter to agreement
  correct_intrinsic_parameters_at_init();

  //compute tau (value) range
  compute_tau_ranges();

  //set the end taus to the tau limits
  set_end_taus_at_init();
}

bool dbsk2d_ishock_lineline_thirdorder::isLSTauValid () 
{
  bool result = LisGEq(_LsTau,0) && LisLEq(_LsTau,_lL);
  //dbsk2d_assert (result);
  return result;
}

bool dbsk2d_ishock_lineline_thirdorder::isRSTauValid () 
{
  bool result = LisGEq(_RsTau,0) && LisLEq(_RsTau,_lR);
  //dbsk2d_assert (result);
  return result;
}

void dbsk2d_ishock_lineline_thirdorder::compute_tau_ranges()
{
  _minLTau = vnl_math_max (0.0, LTau(_lR));
  _maxLTau = _LsTau;
  _minRTau = _RsTau;
  _maxRTau = vnl_math_min (_lR, RTau(0));
  
  dbsk2d_assert(_minLTau<=_maxLTau && _minRTau<=_maxRTau);
}

void dbsk2d_ishock_lineline_thirdorder::correct_intrinsic_parameters_at_init()
{
  //need to complete this
}

void dbsk2d_ishock_lineline_thirdorder::set_end_taus_at_init()
{
  _LeTau = _minLTau;
  _ReTau = _maxRTau;
}

//-----------------------------------------------------------------------------
// Tau conversion functions
//-----------------------------------------------------------------------------

double dbsk2d_ishock_lineline_thirdorder::RTau (double ltau) 
{ 
  return (_LsTau - ltau) +_RsTau ; //lstau>=ltau
} 

double dbsk2d_ishock_lineline_thirdorder::LTau (double rtau) 
{ 
  return _LsTau - (rtau - _RsTau); //rstau<=rtau
} 

double dbsk2d_ishock_lineline_thirdorder::EtaToTau(double eta, DIRECTION dir, bool constrained)
{
  //always output left tau
  if (dir == LEFT)
    return eta; //ltau=leta
  else
    return LTau(eta); //rtau=reta, but need to return ltau(default)
}

//-----------------------------------------------------------------
// DYNAMICS DEFINITIONS
//-----------------------------------------------------------------

double dbsk2d_ishock_lineline_thirdorder::r  (double tau)
{
   return _H/2;
}
 
double dbsk2d_ishock_lineline_thirdorder::rp (double tau)
{
   return 0;
}

double dbsk2d_ishock_lineline_thirdorder::rpp(double tau)
{
   return 0;
}

double dbsk2d_ishock_lineline_thirdorder::g  (double tau)
{
   return 0;
}

double dbsk2d_ishock_lineline_thirdorder::tangent (double tau)
{
   //tangent vector
   return angle0To2Pi(_nl+vnl_math::pi);
}

double dbsk2d_ishock_lineline_thirdorder::k  (double tau)
{
   return 0;
}

double dbsk2d_ishock_lineline_thirdorder::v  (double tau)
{
   return 100000;
}

double dbsk2d_ishock_lineline_thirdorder::a  (double tau)
{
   return 0;
}

double dbsk2d_ishock_lineline_thirdorder::phi (double tau)
{
  return 0;
}

vgl_point_2d<double> dbsk2d_ishock_lineline_thirdorder::getPtFromLTau (double ltau) 
{ 
  vgl_point_2d<double> lstartpt = _origin;
  return _translatePoint (lstartpt, _nl, ltau); 
}

void dbsk2d_ishock_lineline_thirdorder::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ex_pts_.clear();

  vgl_point_2d<double> start = getStartPt ();
  vgl_point_2d<double> end =   getEndPtWithinRange ();

  ex_pts_.push_back(start);
  ex_pts_.push_back(end);
}

void dbsk2d_ishock_lineline_thirdorder::getInfo (vcl_ostream& ostrm)
{
  char s[1024];
  char endx[32], endy[32], simtime[32], endtime[32];

  ostrm << "\n==============================\n";
  ostrm << "LL3O: [" << _id << "] " << "{ "  << (_bActive ? "A" : "nA" ) << ", ";
  ostrm << (_bPropagated ? "Prop" : "nProp" ) << "}" << vcl_endl;

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

  ostrm << "lB: " << _lBElement->id(); vcl_sprintf(s, " (%f, %f)\n", LeEta(), LsEta()); ostrm <<s;
  ostrm << "rB: " << _rBElement->id(); vcl_sprintf(s, " (%f, %f)\n", RsEta(), ReEta()); ostrm <<s;
  ostrm << "[ lS: " << (_lShock?_lShock->id():-1)  << ", rS: " << (_rShock?_rShock->id():-1) << "]\n";
  ostrm << "[ lN: " << (_lNeighbor?_lNeighbor->id():-1)  << ", rN: " << (_rNeighbor?_rNeighbor->id():-1);
  ostrm << ", pN: " << _pSNode->id() << ", cN: " << (_cSNode?_cSNode->id():-1) << " ]\n" << vcl_endl;
  
  vcl_sprintf(s, "LTau: %f - %f (Tau range: %f - %f)\n", _LsTau, _LeTau, minLTau(), maxLTau()); ostrm << s;
  vcl_sprintf(s, "RTau: %f - %f (Tau range: %f - %f)\n\n", _RsTau, _ReTau, minRTau(), maxRTau()); ostrm << s;

  vcl_sprintf(s, "LEta: %f - %f (Eta range: %f - %f)\n", LsEta(), LeEta(), _lBElement->min_eta(), _lBElement->max_eta()); ostrm << s;
  vcl_sprintf(s, "REta: %f - %f (Eta range: %f - %f)\n\n", RsEta(), ReEta(), _rBElement->min_eta(), _rBElement->max_eta()); ostrm << s;

  vcl_sprintf(s, "H: %f\n", _H); ostrm << s;
  vcl_sprintf(s, "nl: %f (%.2f deg) CW from X-axis\n", _nl, _nl*180/vnl_math::pi); ostrm << s;
  vcl_sprintf(s, "ul: %f (%.2f deg) CW from X-axis\n", _ul, _ul*180/vnl_math::pi); ostrm << s;
  vcl_sprintf(s, "lL: %f\n", _lL); ostrm << s;
  vcl_sprintf(s, "lR: %f\n \n", _lR); ostrm << s;
  
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
 /*
 if (MessageOption >= MSG_TERSE) {
 s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
 s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
 s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
 s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

 s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
 s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
 s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
 }
 */

}



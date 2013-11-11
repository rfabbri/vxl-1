#include "sielements.h"

void SISource::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();
  ExPts.push_back(Point2D<double>(_origin.x, _origin.y));
}

void SIA3Source::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();
  ExPts.push_back(Point2D<double>(_origin.x, _origin.y));
}


void SIJunct::compute_extrinsic_locus()
{  
  //clear existing points and recompute in case it was modified
  ExPts.clear();
  ExPts.push_back(Point2D<double>(_origin.x, _origin.y));
}

void SISink::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();
  ExPts.push_back(Point2D<double>(_origin.x, _origin.y));
}

void SIContact::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();

  Point start = getStartPt ();
  Point end;

  if (_simTime == _startTime)
    end = getStartPt();
  else if (endTime() > MAX_RADIUS) {
    //compute projected EngPoint
    end.x = start.x + LARGE_DRAWLENGTH*vcl_cos(_n);
    end.y = start.y + LARGE_DRAWLENGTH*vcl_sin(_n);
  } 
  else
    end = getEndPt();
  
  ExPts.push_back(Point2D<double>(start.x, start.y));
  ExPts.push_back(Point2D<double>(end.x, end.y));
}

void SIPointPoint::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();

  Point start = getStartPt ();
  Point end;
  if (_simTime == _startTime) {
    end = start;
  }
  else if (_simTime > MAX_RADIUS) {
    //compute projected EngPoint
    end.x = start.x + LARGE_DRAWLENGTH*vcl_cos(_n);
    end.y = start.y + LARGE_DRAWLENGTH*vcl_sin(_n);
  } 
  else {
    end = getEndPt();
  }

  ExPts.push_back(Point2D<double>(start.x, start.y));
  ExPts.push_back(Point2D<double>(end.x, end.y));
}

void SIPointLine::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();

  //1)Here the stau and etau is from 0 to 2_M_PI CCW
  //2)We have to first avoid the Extreme Condition that the parabola goes to infinity
  //3)And then make sure etau is always greater than stau for the loop
  //4)Finally draw it

  double stau = sTau();
  double etau = eTau();
  Point2D<double> origin(_origin.x, _origin.y);
  Point2D<double> pt, last_pt;
   double d;

  if (etau == M_PI) //Extreme Condition
    etau = M_PI + 0.001*( stau>M_PI ? +1 : -1 );

  if (stau > etau) 
    swap (stau, etau);

  // increments of the tau while drawing the intrinsic parabola
  const int NUM_SUBDIVISIONS = 100;
  double DELTA_TAU = 2*M_PI/NUM_SUBDIVISIONS; 

  d = _H/(1+vcl_cos(stau));
   pt = origin + rotateCCWPoint2D(d*vcl_cos(stau), d*vcl_sin(stau), _u);
  ExPts.push_back(pt);

  last_pt = pt;

   for (double tau=stau; tau<=etau; tau+=DELTA_TAU) {
      d = _H/(1+vcl_cos(tau));
    pt = origin + rotateCCWPoint2D(d*vcl_cos(tau), d*vcl_sin(tau), _u);

    ExPts.push_back(pt);
    last_pt = pt;
   }

  d = _H/(1+vcl_cos(etau));
   Point2D<double> e_pt = origin + rotateCCWPoint2D(d*vcl_cos(etau), d*vcl_sin(etau), _u);
  ExPts.push_back(e_pt);
}

void SILineLine::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();

  Point start = getStartPt ();
  Point end =   getEndPtWithinRange ();

  ExPts.push_back(Point2D<double>(start.x, start.y));
  ExPts.push_back(Point2D<double>(end.x, end.y));

}

void SIPointArc::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();

  Point2D<double> origin(_origin.x, _origin.y);
  Point2D<double> pt, last_pt;

  double stau = _LsTau;
  double etau = _LeTau;

  if (stau>etau)
    swap(stau, etau);

  // increments of the tau while drawing the intrinsic parabola
  const int NUM_SUBDIVISIONS = 100;
  double DELTA_TAU = 2*M_PI/NUM_SUBDIVISIONS;

  //starting point
   pt = origin + rotateCCWPoint2D(d(stau)*vcl_cos(stau), d(stau)*vcl_sin(stau), _u);
  ExPts.push_back(pt);

  last_pt = pt;

   for (double tau=stau; tau<=etau; tau+=DELTA_TAU) {
    pt = origin + rotateCCWPoint2D(d(tau)*vcl_cos(tau), d(tau)*vcl_sin(tau), _u);

    ExPts.push_back(pt);
    last_pt = pt;
   }

   Point2D<double> e_pt = origin + rotateCCWPoint2D(d(etau)*vcl_cos(etau), d(etau)*vcl_sin(etau), _u);
  ExPts.push_back(e_pt);
}

void SILineArc::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();

  Point2D<double> origin(_origin.x, _origin.y);
  Point2D<double> pt, last_pt;
   double d;
  double u = _u;
  double H = _R + _s*_nud*_H;

  double stau = sTau();
  double etau = eTau();
  
  //CONVERT TAUS... Case 3,4,5,6,9,10:
  if (_s*_nud==-1) {
    //for drawing this parabola rotate the axis and the
    //taus by M_PI
    u += M_PI;
    stau = angle0To2Pi (stau+M_PI);
    etau = angle0To2Pi (etau+M_PI);

    //SPECIAL CASE
    //because of the discontinuity at 2*M_PI
    if (_nu==1 && etau==2*M_PI)
      etau = 0;
  }

  if (etau == M_PI) //Extreme Condition
    etau = M_PI + 0.001*( stau>M_PI ? +1 : -1 );

  if (stau > etau) 
    swap (stau, etau);

  // increments of the tau while drawing the intrinsic parabola
  const int NUM_SUBDIVISIONS = 100;
  double DELTA_TAU = 2*M_PI/NUM_SUBDIVISIONS; 

  d = H/(1+vcl_cos(stau));
   pt = origin + rotateCCWPoint2D(d*vcl_cos(stau), d*vcl_sin(stau), u);
  ExPts.push_back(pt);

  last_pt = pt;

   for (double tau=stau; tau<=etau; tau+=DELTA_TAU) {
      d = H/(1+vcl_cos(tau));
    pt = origin + rotateCCWPoint2D(d*vcl_cos(tau), d*vcl_sin(tau), u);

    ExPts.push_back(pt);
    last_pt = pt;
   }

  d = H/(1+vcl_cos(etau));
   Point2D<double> e_pt = origin + rotateCCWPoint2D(d*vcl_cos(etau), d*vcl_sin(etau), u);
  ExPts.push_back(e_pt);
}

void SIArcArc::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();

  Point2D<double> origin(_origin.x, _origin.y);
  Point2D<double> pt, last_pt;

  double stau = _LsTau;
  double etau = _LeTau;

  if (stau>etau)
    swap(stau, etau);

  // increments of the tau while drawing the intrinsic parabola
  const int NUM_SUBDIVISIONS = 100;
  double DELTA_TAU = 2*M_PI/NUM_SUBDIVISIONS;

  //starting point
   pt = origin + rotateCCWPoint2D(d(stau)*vcl_cos(stau), d(stau)*vcl_sin(stau), _u);
  ExPts.push_back(pt);

  last_pt = pt;

   for (double tau=stau; tau<=etau; tau+=DELTA_TAU) {
    pt = origin + rotateCCWPoint2D(d(tau)*vcl_cos(tau), d(tau)*vcl_sin(tau), _u);

    ExPts.push_back(pt);
    last_pt = pt;
   }

   Point2D<double> e_pt = origin + rotateCCWPoint2D(d(etau)*vcl_cos(etau), d(etau)*vcl_sin(etau), _u);
  ExPts.push_back(e_pt);
}

void SIThirdOrder::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();

  Point start = getStartPt ();
  Point end =   getEndPtWithinRange ();

  ExPts.push_back(Point2D<double>(start.x, start.y));
  ExPts.push_back(Point2D<double>(end.x, end.y));
}

void SIArcThirdOrder::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();

  double startVector, endVector;

  //Intrinsic to Extrinsic...
  if (_nudl == ARC_NUD_CW) {
    startVector = angle0To2Pi ( _ul - _LsTau);
    endVector   = angle0To2Pi ( _ul - _LeTau);
  }
  else {
    startVector = angle0To2Pi ( _ul + _LsTau);
    endVector   = angle0To2Pi ( _ul + _LeTau);
  }

  //bridge across discontinuity
  if (endVector < startVector) endVector += 2*M_PI;

  double R = (_Rl+_Rr)/2;

  const int NUM_ELLIPSE_SUBDIVISIONS = 100;
  int n_line_segs = int(NUM_ELLIPSE_SUBDIVISIONS*vcl_fabs(endVector-startVector)/(2*M_PI));
  if(n_line_segs < 4) n_line_segs = 4;

  for(int i = 0; i < n_line_segs; ++i) {
    double v = startVector + (endVector-startVector)*i/double(n_line_segs-1);
    ExPts.push_back(Point2D<double>(_origin.x+R*vcl_cos(v), _origin.y+R*vcl_sin(v)));
  }
}

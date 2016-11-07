#include "belements.h"

void BPoint::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();
  ExPts.push_back(Point2D<double>(_pt.x, _pt.y));
}

void BLine::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();
  ExPts.push_back(Point2D<double>(_start.x, _start.y));
  ExPts.push_back(Point2D<double>(_end.x, _end.y));
}

void BArc::compute_extrinsic_locus()
{
  //clear existing points and recompute in case it was modified
  ExPts.clear();

  double startVector = _StartVector;
  double endVector = _EndVector;

  //bridge across discontinuity
  if (endVector < startVector) endVector += 2*M_PI;

  const int NUM_ELLIPSE_SUBDIVISIONS = 100;
  int n_line_segs = int(NUM_ELLIPSE_SUBDIVISIONS*vcl_fabs(endVector-startVector)/(2*M_PI));
  if(n_line_segs < 4) n_line_segs = 4;

  for(int i = 0; i < n_line_segs; ++i) {
    double v = startVector + (endVector-startVector)*i/double(n_line_segs-1);
    ExPts.push_back(Point2D<double>(_center.x+_R*vcl_cos(v), _center.y+_R*vcl_sin(v)));
  }
}


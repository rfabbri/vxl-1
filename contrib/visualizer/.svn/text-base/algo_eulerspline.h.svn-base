#ifndef EULERSPLINE_H_INCLUDED
#define EULERSPLINE_H_INCLUDED

#include "basegui_nodes.h"
#include "algo_euler.h"

class EulerSplineGeom : public GeometryNode {
public:
  typedef std::vector<Point2D<double> > points_t;
  typedef std::vector<EulerSpiralParams> spirals_t;
protected:
  points_t _points;
  spirals_t _spirals;
  bool _inhibit_computation;

  // the way we compute the tangents, each point affects the 4 spirals 
  // closest to them. recomputeNearbySpirals computes the results
  // for all of them. This can be inefficient if many points are
  // changed at once: see beginTransaction and endTransaction for a remedy.
  void recomputeNearbySpirals(int changed);
  void computeSpiral(int i);
public:
  EulerSplineGeom();

  // if you're going to make any large changes to the spline, call
  // beginTransaction, then make the changes, then call endTransaction.
  // This could concievably speed things up by a factor of 4.
  void beginTransaction();
  void endTransaction();

  void push_back(double x, double y) { push_back(Point2D<double>(x, y)); }
  void push_back(const Point2D<double> &d);
  void pop_back();
  void erase();

  virtual void draw(SceneGraphVisitor *);
};

#endif

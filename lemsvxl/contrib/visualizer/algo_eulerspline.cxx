#include "algo_eulerspline.h"
#include "algo_euler.h"
#include "basegui_visitor.h"

EulerSplineGeom::EulerSplineGeom()
  : GeometryNode(), _inhibit_computation(false) {}

void EulerSplineGeom::computeSpiral(int i) {
  if(_inhibit_computation) return;
  if(i < 0 || i >= _spirals.size()) return;

  double x0, y0, x1, y1;

  x0 = _points[i  ].getX();
  y0 = _points[i  ].getY();
   x1 = _points[i+1].getX();
   y1 = _points[i+1].getY();

  double theta0, theta1;

  if(i == 0) {
    theta0 = atan2(_points[1].getY()-_points[0].getY(), _points[1].getX()-_points[0].getX());
    theta1 = atan2(_points[2].getY()-_points[0].getY(), _points[2].getX()-_points[0].getX());
  } else if(i == _spirals.size()-1) {
    theta0 = atan2(_points[i+1].getY()-_points[i-1].getY(), _points[i+1].getX()-_points[i-1].getX());
    theta1 = atan2(_points[i+1].getY()-_points[i].getY(), _points[i+1].getX()-_points[i].getX());
  } else {
    theta0 = atan2(_points[i+1].getY()-_points[i-1].getY(), _points[i+1].getX()-_points[i-1].getX());
    theta1 = atan2(_points[i+2].getY()-_points[i].getY(), _points[i+2].getX()-_points[i].getX());
  }

  EulerSpiral ES;
  ES.set_params(Point2D<double>(x0, y0), theta0, Point2D<double>(x1, y1), theta1);
  ES.get_euler_spiral_params(_spirals[i]);
}

void EulerSplineGeom::recomputeNearbySpirals(int changed) {
  if(_inhibit_computation) return;

  computeSpiral(changed-1);
  computeSpiral(changed);
  computeSpiral(changed+1);
  computeSpiral(changed+2);
}

void EulerSplineGeom::draw(SceneGraphVisitor *gfx) {
  assert(!_inhibit_computation); // did you do a beginTransaction() without an endTransaction() ?

  gfx->beginPoints();
  for(points_t::iterator i = _points.begin(); i != _points.end(); ++i)
    gfx->vertex(i->getX(), i->getY());
  gfx->end();

  EulerSpiral ES;
  for(spirals_t::iterator j = _spirals.begin(); j != _spirals.end(); ++j) {
    gfx->beginPolyline();
    for(double s = 0; s < 1.01*j->getLength(); s += j->getLength()/10) {
      Point2D<double> p = ES.compute_es_point(*j, s);
      gfx->vertex(p.getX(), p.getY());
    }
    gfx->end();
  }
}

void EulerSplineGeom::push_back(const Point2D<double> &d) {
  _points.push_back(d);
  if(_points.size() >= 2)
    _spirals.push_back(EulerSpiralParams());
  recomputeNearbySpirals(int(_spirals.size())-1);
}

void EulerSplineGeom::pop_back() {
  _points.pop_back();
  if(!_spirals.empty())
    _spirals.pop_back();
  recomputeNearbySpirals(int(_spirals.size())-1);
}

void EulerSplineGeom::erase() {
  _points.erase(_points.begin(), _points.end());
  _spirals.erase(_spirals.begin(), _spirals.end());
}

void EulerSplineGeom::beginTransaction() {
  assert(!_inhibit_computation); // no nested begin/end pairs!
  _inhibit_computation = true;
}

void EulerSplineGeom::endTransaction() {
  assert(_inhibit_computation); // no nested begin/end pairs!
  _inhibit_computation = false;
  for(int i = 0; i < _spirals.size(); ++i)
    computeSpiral(i);
}

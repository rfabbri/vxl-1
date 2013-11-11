#include "algo_eulergeom.h"
#include "basegui_visitor.h"

void EulerSpiralGeom::set(const Point2D<double> &p0, double t0,
      const Point2D<double> &p1, double t1) {
  EulerSpiral ES;
  ES.set_params(p0, t0, p1, t1);
  ES.get_euler_spiral_params(params);
}

void EulerSpiralGeom::draw(SceneGraphVisitor *geom) {
  EulerSpiral ES;
  geom->beginPolyline();
    double temp = params.getLength()/70.0;
    Point2D<double> p;
            
    for(double s = 0; s < (params.getLength()+temp); s += temp)
      {
    p = ES.compute_es_point(params, s);
    geom->vertex(p.getX(), p.getY());
       }
    geom->end();
}

#ifndef OFFSET_INTERVAL_H_INCLUDED
#define OFFSET_INTERVAL_H_INCLUDED

#include "geomnodes.h"
#include <vcl_vector>

class OffsetIntervalGeom : public GeometryNode {
  double start_length, end_length;
  double offset_pixels;
  LineGeom *line;

public:
  OffsetIntervalGeom(LineGeom *l, double off_pix=5) :
    line(l), offset_pixels(off_pix) {}

  void setInterval(double sl, double el) {
    start_length = sl;
    end_length = el;
  }

  virtual void draw(SceneGraphVisitor *v) {
    if(euc_distance(line->end(), line->start()) < 1e-10)
      return;

    double perp_offset = offset_pixels/v->getScaleFactor();
    Point2D<double>
      direction = (line->end() - line->start())/euc_distance(line->end(), line->start()),
      perp = Point2D<double>(direction.getY(), -direction.getX());

    v->beginPolyline();
    v->vertex(line->start() + direction*start_length + perp_offset*perp);
    v->vertex(line->start() + direction*end_length + perp_offset*perp);
    v->end();
  }
};

#endif

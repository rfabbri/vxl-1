#ifndef EULERGEOM_H_INCLUDED
#define EULERGEOM_H_INCLUDED

#include "basegui_nodes.h"
#include "algo_euler.h"

class EulerSpiralGeom : public GeometryNode {
protected:
  EulerSpiralParams params;

public:
  EulerSpiralGeom(const Point2D<double> &p0, double t0,
      const Point2D<double> &p1, double t1) : GeometryNode() 
              {
                set(p0, t0, p1, t1);
               }


            void set(const Point2D<double> &p0, double t0, const Point2D<double> &p1, double t1);
            virtual void draw(SceneGraphVisitor *);

            EulerSpiralGeom(EulerSpiralParams params_)
              {
                params = params_;
               }

            virtual    ~EulerSpiralGeom(){};
 };

#endif   /* EULERGEOM_H_INCLUDED */

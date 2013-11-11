#ifndef VISITOR_H_INCLUDED
#define VISITOR_H_INCLUDED

#include "base_points.h"
#include "basegui_style.h"
#include "basegui_xform.h"

#include <iosfwd>

class GraphicsView;
class SceneGraph;
class GraphicsNode;
class StyleNode;
class XFormNode;
class Group;
class Image;

enum VisitState { VISIT_PRE, VISIT_POST };
enum VisitResult { VISIT_CONTINUE, VISIT_SKIP, VISIT_ABORT };

class SceneGraphVisitor {
public:
  virtual VisitResult visitView(GraphicsView *s, VisitState vs) = 0;
  // visitSceneGraph is called once at the start, and once at the end, of the visit
  virtual VisitResult visitSceneGraph(SceneGraph *s, VisitState vs) = 0;
  virtual VisitResult visitGeometry(GraphicsNode *n, VisitState vs) = 0;
  virtual VisitResult visitStyle(StyleNode *p, VisitState vs) = 0;
  virtual VisitResult visitGroup(Group *g, VisitState vs) = 0;
  virtual VisitResult visitXForm(XFormNode *n, VisitState vs) = 0;

  virtual void pushMatrix() = 0;
  virtual void multXForm(const XForm &xf) = 0;
  virtual void popMatrix() = 0;

  virtual void beginPolyline() = 0;
  virtual void beginFilledPolygon() = 0; // assumed to be convex
  virtual void beginUnfilledPolygon() = 0;
  virtual void beginPoints() = 0;
  virtual void end() = 0;

    // these can be used to create an object which doesn't zoom as 
    // the canvas is zoomed. calling unscale will set up a coordinate
    // system where 1 unit is one screen width
  virtual void unscale(double origin_x, double origin_y) = 0;
  virtual void rescale() = 0;
    // returns the unit scale factor. so, if you draw something n units long,
    // it will show up as getScaleFactor()*n times the size of the screen.
  virtual double getScaleFactor() = 0;

  void vertex(const Point2D<double> &a) { vertex(a.getX(), a.getY()); }
  virtual void vertex(double x, double y) = 0;
  virtual void infiniteVertex(double vx, double vy) = 0; // this is a vertex at infinity, in the direction (vx, vy)

  // TODO the api here is inconsistent... perhaps it'd be better to have
  // end pairs for each primitive type
  virtual void beginConcaveFilledPolygon() = 0;
  virtual void concaveVertex(double x, double y) = 0;
  virtual void endConcaveFilledPolygon() = 0;

  virtual void text(double x, double y, const char *t) = 0;
  virtual void affineText(double x, double y, const char *t) = 0; // text that rotates, zooms, etc.
  virtual void image(double x1, double y1, double x2, double y2, Image *img) = 0;

  // these shouldn't be called directly by node draw()
  // functions... they're all managed by the
  // style nodes and style.apply()
  virtual void setLineWidth(double w) = 0;
  virtual void setHidden(bool h) = 0;
  virtual void setPointSize(double s) = 0;
  virtual void setColor(float r, float g, float b, float a = 1) = 0;
  // note: color components here go from 0 to 1 as in OpenGL, not
  // 0 to 255.
  virtual void setFont(GLFont *font) = 0;

protected:
  StyleSet _current_style;

public:
  virtual const StyleSet &currentStyle() const;

  // mergeStyle merges the styles (i.e. add whatever properties
  // that "add_style" contains, and then returns the style
  // set which will undo whatever changes the merge made.
  // this has the effect of allowing "style inheritance":
  // any styles which aren't set are inherited from the
  // parent node
  virtual StyleSet mergeStyle(const StyleSet &add_style);

  // replaces _all_ style properties with new_style
  virtual void loadStyle(const StyleSet &new_style);
};

#endif

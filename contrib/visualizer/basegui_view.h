#ifndef VIEW_H_INCLUDED
#define VIEW_H_INCLUDED

#include <vector>
#include "base_points.h"
#include "basegui_xform.h"
#include "basegui_color.h"

class SceneGraph;
class Group;
class GraphicsNode;
class SceneGraphVisitor;
class RGBImage;

// Each hit record returns an object, as well as all the groups
// it belongs to. group_stack is the list of groups, sorted from the
// highest level (the root), down to the lowest level.
struct HitRecord {
  std::vector<Group*> group_stack;
  GraphicsNode *hit_object;
};

class GraphicsView {
public:
  virtual int width() const = 0;
  virtual int height() const = 0;

  virtual void translate(const Point2D<double> &screen_delta) = 0;
  virtual void zoom(const Point2D<double> &screen_center, float amount_to_zoom) = 0;
  virtual void rotate(const Point2D<double> &screen_center, float radians) = 0;
  // flips origin from bottom left to top left (or vice versa). it defaults
  // to the top left.
  virtual void flip_y_axis() = 0;

  virtual void zoomToScreenRect(double x1, double y1, double x2, double y2) = 0;
  virtual void zoomToWorldRect(double x1, double y1, double x2, double y2) = 0;
  virtual void zoomToBounds(GraphicsNode *g) = 0;

  virtual SceneGraph *sceneGraph() = 0;
  virtual const SceneGraph *sceneGraph() const = 0;

  virtual Point2D<double> unproject(const Point2D<double> &screenPoint) = 0;
  virtual Point2D<double> project(const Point2D<double> &world_point) = 0;

  // returns number of hits added to the hit record vector.
  // hits is cleared before the hit results are added.
  // hit_radius is the diameter of the hit region in pixels
  virtual int hitTest(std::vector<HitRecord> &hits,
      const Point2D<double> &screen_point,
      int hit_diameter = 2) = 0;

  virtual int hitTest(std::vector<HitRecord> &hits,
      const Point2D<double> topLeft, const Point2D<double>bottomRight) = 0;

    // returns the unit scale factor. so, if you draw something n units long,
    // it will show up as getScaleFactor()*n pixels.
  virtual double getScaleFactor() const = 0;

  // returns a matrix which represents the transformation from world space
  // to screen space. in screen space, the screen is an axis aligned rectangle
  // which goes from 0 to 1 in y, and 0 to aspect in x. aspect is width/height
  // the origin is the bottom left, although by default this projection matrix
  // is initialized to flip that.
  virtual XForm getProjectionMatrix() const = 0;
  virtual void setProjectionMatrix(const XForm &x) = 0;

  virtual void traverse(SceneGraphVisitor *c) = 0;
  virtual RGBImage *screenCapture() = 0;

  virtual void setBackgroundColor(float r, float g, float b) = 0;
  virtual void setBackgroundColor(StandardColor c) = 0;
};

#endif

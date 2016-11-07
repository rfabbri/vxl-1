#ifndef GL_GRAPHICS_H_INCLUDED
#define GL_GRAPHICS_H_INCLUDED

#include "basegui_view.h"
#include "basegui_scenegraph.h"
#include "basegui_visitor.h"
#include "basegui_glvisitors.h"
#include "basegui_xform.h"

class RGBImage;

class GLGraphicsView : public GraphicsView {
private:
  int _width, _height;
  XForm view_transform;

  SceneGraph *_scene_graph;
  GLFontLibrary *_fonts;
  GLGraphicsOut _gfx;
  GLPicker _picker;

  float _bgr, _bgg, _bgb; // background color
  bool _newbgcolor;

public:
  GLGraphicsView(SceneGraph *c);
  virtual ~GLGraphicsView();

  void translate(const Point2D<double> &screen_delta);
  void zoom(const Point2D<double> &screen_center, float amount_to_zoom);
  void rotate(const Point2D<double> &screen_center, float radians);
  // flips y axis from bottom left to top right (or vice versa)
  void flip_y_axis();

  virtual void zoomToScreenRect(double x1, double y1, double x2, double y2);
  virtual void zoomToWorldRect(double x1, double y1, double x2, double y2);
  virtual void zoomToBounds(GraphicsNode *g);

  SceneGraph *sceneGraph();
  const SceneGraph *sceneGraph() const;

  Point2D<double> unproject(const Point2D<double> &screenPoint);
  Point2D<double> project(const Point2D<double> &world_point);

  int width() const;
  int height() const;

  int hitTest(std::vector<HitRecord> &hits,
      const Point2D<double> &screen_point, int diameter);

  int hitTest(std::vector<HitRecord> &hits,
      const Point2D<double> topLeft, const Point2D<double>bottomRight);

  double getScaleFactor() const;

  XForm getProjectionMatrix() const;
  void setProjectionMatrix(const XForm &x);

  void traverse(SceneGraphVisitor *c);

  RGBImage *screenCapture();

  void setBackgroundColor(float r, float g, float b);
  void setBackgroundColor(StandardColor c);
public:
  // these are pretty much the only operations you need to call
  // in order to get GL to work. Call init() to initialize the screen.
  // resize() should also be called once when the widget is created
  // to set the widget size, BEFORE draw() is called at all, but after
  // init().
  void init();
  void resize(int w, int h);
  void draw();
};

#endif

#include "basegui_xpgl.h"
#include "basegui_glgraphics.h"
#include "basegui_scenegraph.h"
#include "basegui_visitor.h"
#include "basegui_xform.h"
#include "basegui_glfont.h"
#include "basegui_rgbimage.h"

#include <vcl_cassert.h>

GLGraphicsView::GLGraphicsView(SceneGraph *c) 
  : _width(-1), _height(-1), _scene_graph(c),
  _fonts(GLFont::newLibrary()), _gfx(_fonts), _picker(_fonts) {

  setBackgroundColor(1,1,1);

  view_transform =
    XForm::translation(Vect3(0,.5,0)) *
    XForm::reflect_y_axis() *
    XForm::translation(Vect3(0,-.5,0)) *
    XForm::uniform_scale(.01);
}

GLGraphicsView::~GLGraphicsView() {
  GLFont::deleteLibrary(_fonts);
}

int GLGraphicsView::width() const { return _width; }
int GLGraphicsView::height() const { return _height; }

void GLGraphicsView::init() {
  glShadeModel(GL_FLAT);
  glDisable(GL_DITHER);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//GL_ONE
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
}

void GLGraphicsView::resize(int w, int h) {
  _width = w; _height = h;
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w/float(h), 0, 1,  0, 1);
  //glOrtho(0, w, 0, h,  -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

double GLGraphicsView::getScaleFactor() const {
  return sqrt(fabs(determinant(view_transform)))*height();
}

XForm GLGraphicsView::getProjectionMatrix() const {
  return view_transform;
}

void GLGraphicsView::setProjectionMatrix(const XForm &x) {
  view_transform = x;
}

void GLGraphicsView::setBackgroundColor(float r, float g, float b) {
  _bgr = r;
  _bgg = g;
  _bgb = b;
  _newbgcolor = true;
}

void GLGraphicsView::setBackgroundColor(StandardColor c) {
  float r, g, b;
  lookup_standard_color(c, r, g, b);
  setBackgroundColor(r, g, b);
}

void GLGraphicsView::draw() {
  assert(_width >= 0); // are you calling resize() at least once before drawing the widget?

  if(_newbgcolor) {
    glClearColor(_bgr, _bgg, _bgb, 0);
    _newbgcolor = false;
  }

  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  traverse(&_gfx);
  glFlush();
}

RGBImage *GLGraphicsView::screenCapture() {
  draw();
  unsigned char
    *data_upsidedown = new unsigned char[3*_width*_height],
    *data = new unsigned char[3*_width*_height];
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
  glPixelStorei(GL_PACK_ALIGNMENT,1); // tight packing

  glReadPixels(0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, data_upsidedown);

  glPopClientAttrib();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  // reverse image
  for(int y = 0; y < _height; ++y)
    memcpy(&data[_width*3*y], &data_upsidedown[_width*3*(_height-1-y)], _width*3);

  RGBImage *r = new RGBImage(_width, _height, data, RGBImage::NoCopy);
  return r;
}

int GLGraphicsView::hitTest(std::vector<HitRecord> &hits,
      const Point2D<double> &screen_point,
      int hit_diameter) {
  glLoadIdentity();

  hits.clear();
  _picker.setHitTestParameters(&hits,
      (int)screen_point.getX(),
      (int)screen_point.getY(),
      _width, _height, hit_diameter);

  traverse(&_picker);

  _picker.setHitTestParameters(NULL,0,0,0,0,0); // make sure no stale pointers
  return hits.size();
}

int GLGraphicsView::hitTest(std::vector<HitRecord> &hits,
      const Point2D<double> topLeft, const Point2D<double>bottomRight) {
  glLoadIdentity();

  hits.clear();
  _picker.setHitTestParameters(&hits,
      (int)topLeft.getX(),
      (int)topLeft.getY(),
      _width, _height, bottomRight.getX()-topLeft.getX());//hack

  traverse(&_picker);

  _picker.setHitTestParameters(NULL,0,0,0,0,0); // make sure no stale pointers
  return hits.size();
}

void GLGraphicsView::traverse(SceneGraphVisitor *g) {
  VisitResult res = g->visitView(this, VISIT_PRE);
  if(res == VISIT_ABORT || res == VISIT_SKIP) return;

  _scene_graph->traverse(g);
  g->visitView(this, VISIT_POST);
}

void GLGraphicsView::translate(const Point2D<double> &screen_delta) {
  Point2D<double> world_tx = 
    unproject(screen_delta) - unproject(Point2D<double>(0, 0));

  view_transform = view_transform * 
    XForm::translation(
        Vect3(world_tx[0], world_tx[1], 0));
}

void GLGraphicsView::zoom(const Point2D<double> &screen_center,
    float amount_to_zoom) {
  Point2D<double> world_center_p2d = unproject(screen_center);
  Vect3 world_center_offset =
    Vect3(world_center_p2d[0], world_center_p2d[1], 0);

  view_transform =
    view_transform *
    XForm::translation(world_center_offset) *
    XForm::uniform_scale(amount_to_zoom) *
    XForm::translation(-world_center_offset);
}

void GLGraphicsView::rotate(const Point2D<double> &screen_center, float radians) {
  Point2D<double> world_center_p2d = unproject(screen_center);
  Vect3 world_center_offset =
    Vect3(world_center_p2d[0], world_center_p2d[1], 0);

  view_transform =
    view_transform *
    XForm::translation(world_center_offset) *
    XForm::rotation(radians) *
    XForm::translation(-world_center_offset);
}

void GLGraphicsView::flip_y_axis() {
  Point2D<double> world_center = unproject(Point2D<double>(_width/2., _height/2.));
  Vect3 world_center_offset = Vect3(world_center[0], world_center[1], 0);

  view_transform =
    view_transform *
    XForm::translation(world_center_offset) *
    XForm::reflect_y_axis() *
    XForm::translation(-world_center_offset);
}

void GLGraphicsView::zoomToWorldRect(double x1, double y1, double x2, double y2) {
  if(x1 >= x2 || y1 >= y2) return;

  double sx1 = HUGE_VAL, sy1 = HUGE_VAL, sx2 = -HUGE_VAL, sy2 = -HUGE_VAL; // screen coords

  Point2D<double> tmp;
  for(int i = 0; i < 4; ++i) {
    switch(i){
    case 0: tmp = project(Point2D<double>(x1, y1)); break;
    case 1: tmp = project(Point2D<double>(x1, y2)); break;
    case 2: tmp = project(Point2D<double>(x2, y2)); break;
    case 3: tmp = project(Point2D<double>(x2, y1)); break;
    }

    if(tmp.getX() < sx1) sx1 = tmp.getX();
    if(tmp.getX() > sx2) sx2 = tmp.getX();
    if(tmp.getY() < sy1) sy1 = tmp.getY();
    if(tmp.getY() > sy2) sy2 = tmp.getY();
  }

  zoomToScreenRect(sx1, sy1, sx2, sy2);
}

void GLGraphicsView::zoomToScreenRect(double x1, double y1, double x2, double y2) {
  assert(width() >= 0 && height() >= 0);
  if(x1 >= x2 || y1 >= y2) return;

  Point2D<double> real_x1y1, real_x2y2, real_x1y2, real_x2y1;
  real_x1y1 = unproject(Point2D<double>(x1, y1));
  real_x1y2 = unproject(Point2D<double>(x1, y2));
  real_x2y2 = unproject(Point2D<double>(x2, y2));
  real_x2y1 = unproject(Point2D<double>(x2, y1));

  Point2D<double> screenTopLeft = unproject(Point2D<double>(0,0)),
    screenTopRight = unproject(Point2D<double>(width(), 0)),
    screenBotLeft = unproject(Point2D<double>(0, height()));

  double scale_factor1 = 
        euc_distance(screenTopLeft, screenTopRight) /
        euc_distance(real_x1y1, real_x2y1);
  double scale_factor2 =
        euc_distance(screenTopLeft, screenBotLeft) /
        euc_distance(real_x1y1, real_x1y2);
  double scale_factor;

  if(scale_factor1 < scale_factor2)
    scale_factor = scale_factor1;
  else
    scale_factor = scale_factor2;

  // step 2: scale
  view_transform =
    view_transform *
    XForm::uniform_scale(scale_factor);

  Vect3 cur_rect_center = Vect3((.5*(real_x1y1 + real_x2y2)).getX(), (.5*(real_x1y1 + real_x2y2)).getY(), 1);
  Vect3 desired_projected_center = Vect3(width()/double(2*height()),.5,1);

  // step 1: translate to the center of the screen
  Vect3 trans1 = invert(view_transform) * desired_projected_center - cur_rect_center;
  view_transform =
    view_transform *
    XForm::translation(trans1);
}

void GLGraphicsView::zoomToBounds(GraphicsNode *g) {
  double x1, y1, x2, y2;
  sceneGraph()->getBoundingRect(getProjectionMatrix(), g, x1, y1, x2, y2);
  double proj_x1, proj_y1, proj_x2, proj_y2;

  proj_x1  = height()*x1;
  proj_y1  = height()*(1.-y2);
  proj_x2  = height()*x2;
  proj_y2  = height()*(1.-y1);

  zoomToScreenRect(proj_x1, proj_y1, proj_x2, proj_y2);
}

SceneGraph *GLGraphicsView::sceneGraph() { return _scene_graph; }
const SceneGraph *GLGraphicsView::sceneGraph() const { return _scene_graph; }

Point2D<double> GLGraphicsView::unproject(const Point2D<double> &screenPoint) {
  // we could also do this using gluUnProject ...

  float x, y;
  y = (_height - screenPoint[1]) / _height;
  x = screenPoint[0] / _width * (_width/float(_height));
  Vect3 result = invert(view_transform) * Vect3(x,y,1);

  return Point2D<double>(result[0], result[1]);
}

Point2D<double> GLGraphicsView::project(const Point2D<double> &world_point) {
  Vect3 eye_point = view_transform * Vect3(world_point[0], world_point[1], 1);
  float x, y;
  x = (_height/float(_width)) * _width * eye_point[0];
  y = _height*(1-eye_point[1]);
  return Point2D<double>(x, y);
}

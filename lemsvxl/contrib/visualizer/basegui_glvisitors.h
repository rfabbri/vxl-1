#ifndef GLVISITORS_H_INCLUDED
#define GLVISITORS_H_INCLUDED

#include <vector>
#include <map>
#include "basegui_visitor.h"
#include "basegui_xpglu.h"

struct HitRecord;
class GLFont;
class GLFontLibrary;

class GLGraphicsOut: public SceneGraphVisitor {
private:
  GLUtesselator *tesselator;

  typedef std::vector<std::pair<double*, int> > tesselatorVertices_t;
  tesselatorVertices_t tesselatorVertices; // for concave polygons, see beginConcaveFilledPolygon
  double *addTessVertex(double x, double y, double z);
  void clearTessVertices();

  GLFont *_cur_font;
  GLFontLibrary *_fonts;

  std::map<Image *, unsigned int> texture_names;
  std::vector<double> scale_factors; // for unscale

protected:
  bool _hidden;

public:
  GLGraphicsOut(GLFontLibrary *fnts);
  virtual ~GLGraphicsOut();

  virtual VisitResult visitView(GraphicsView *s, VisitState vs);
  virtual VisitResult visitSceneGraph(SceneGraph *g, VisitState vs);
  virtual VisitResult visitGeometry(GraphicsNode *n, VisitState vs);
  virtual VisitResult visitStyle(StyleNode *p, VisitState vs);
  virtual VisitResult visitGroup(Group *g, VisitState vs);
  virtual VisitResult visitXForm(XFormNode *n, VisitState vs);

  virtual void pushMatrix();
  virtual void multXForm(const XForm &xform);
  virtual void popMatrix();

  virtual void unscale(double origin_x, double origin_y);
  virtual void rescale();
  virtual double getScaleFactor();

  virtual void beginPolyline();
  virtual void beginFilledPolygon();
  virtual void beginUnfilledPolygon();
  virtual void beginPoints();
  virtual void end();

  virtual void beginConcaveFilledPolygon();
  virtual void concaveVertex(double x, double y);
  virtual void endConcaveFilledPolygon();
  virtual void text(double x, double y, const char *t);
  virtual void affineText(double x, double y, const char *t);
  virtual void image(double x1, double y1, double x2, double y2, Image *img);

  virtual void vertex(double x, double y);
  virtual void infiniteVertex(double vx, double vy);
  virtual void setColor(float r, float g, float b, float a = 1);
  virtual void setLineWidth(double w);
  virtual void setPointSize(double s);
  virtual void setHidden(bool h);
  virtual void setFont(GLFont *font);

private:
  static void gluTessCombineStaticCB(GLdouble coords[3], void *vertex_data[4],
    GLfloat weight[4], void **outData, void *data);
  void gluTessCombineCB(GLdouble coords[3], void *vertex_data[4],
    GLfloat weight[4], void **outData);
};

class GLPicker: public GLGraphicsOut {
private:
  enum { GL_SELECT_BUFFER_SIZE = 4096 };
  unsigned int select_buffer[GL_SELECT_BUFFER_SIZE];
  std::vector<int> cur_names;
  int _window_w, _window_h, _hit_x, _hit_y, _diameter;
  std::vector<HitRecord> *_hit_records;

public:
  GLPicker(GLFontLibrary *fonts);
    ~GLPicker(){};
    
  void setHitTestParameters(std::vector<HitRecord> *hits,
      int x, int y, int w, int h, int diameter);

  virtual VisitResult visitSceneGraph(SceneGraph *g, VisitState vs);
  virtual VisitResult visitGeometry(GraphicsNode *n, VisitState vs);
  virtual VisitResult visitGroup(Group *g, VisitState vs);
};

#endif

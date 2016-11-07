#include <basegui_xpgl.h>
#include "basegui_printer.h"
#include "basegui_xform.h"
#include "basegui_glfont.h"
#include "basegui_scenegraph.h"
#include "basegui_view.h"
#include "basegui_visitor.h"
#include <vcl_cstdio.h>
#include <iostream>
#include <assert.h>

class PSPrinterVisitor;
void setup_view(PSPrinterVisitor &printer, double width, XForm viewmat, GraphicsNode *g,
    double x1, double y1, double x2, double y2,
    double border, bool clip);

class PSPrinterVisitor : public SceneGraphVisitor {
private:
  double window_height_pixels;
  FILE *f;
  std::vector<XForm> xform_stack;
  enum MODE { NONE, PLINE, POLYGON, UPOLYGON, POINTS };
  MODE mode;
  int vertno;
  double x1, y1, x2, y2;
  double thicken, pointsize;
  bool clip;

  GraphicsNode *the_object_to_show;
  bool now_hidden;

public:
  PSPrinterVisitor(const char *file, double thickenn) :  mode(NONE), thicken(thickenn), clip(false){
    f = fopen(file, "w");
    if(!f)
      std::cerr << "error opening file " << file << " for printing" << vcl_endl;
    now_hidden = true;
  }
  virtual ~PSPrinterVisitor() {
    fclose(f);
  }
  bool valid() { return f; }

  void setView(XForm viewmat, GraphicsNode *g, double xx1, double yy1, double xx2, double yy2, bool cclip) {
    xform_stack.push_back(viewmat);
    x1 = xx1;
    y1 = yy1;
    x2 = xx2;
    y2 = yy2;
    clip = cclip;
    the_object_to_show = g;
  }

  virtual VisitResult visitView(GraphicsView *s, VisitState vs) {
    window_height_pixels = s->height();
    return VISIT_CONTINUE;
  }

  virtual VisitResult visitSceneGraph(SceneGraph *s, VisitState vs) {
    if(vs == VISIT_PRE) {
      fprintf(f, "%%!PS-Adobe-2.0 EPSF-3.0\n");
      fprintf(f, "%%%%Creator: LEMS Visualizer 1.0\n");
      fprintf(f, "%%%%Pages: 1\n");
      fprintf(f, "%%%%BoundingBox: %d %d %d %d\n", int(floor(x1)), int(floor(y1)), int(ceil(x2)), int(ceil(y2)));
            fprintf(f, "gsave\n");
      if(clip) {
        fprintf(f, "gsave\n");
        fprintf(f, "newpath\n");
        fprintf(f, "%g %g moveto\n", x1, y1);
        fprintf(f, "%g %g lineto\n", x1, y2);
        fprintf(f, "%g %g lineto\n", x2, y2);
        fprintf(f, "%g %g lineto\n", x2, y1);
        fprintf(f, "closepath\n");
        fprintf(f, "clip\n");
        fprintf(f, "newpath\n");
                fprintf(f, "grestore\n");
      }
    } else {
      fprintf(f, "grestore\n");
      fprintf(f, "%%%%EOF\n");
    }
    return VISIT_CONTINUE;
  }

  virtual VisitResult visitGeometry(GraphicsNode *n, VisitState vs) {
    if(vs == VISIT_PRE) {
      if(the_object_to_show == n)
        now_hidden = false;

      if(now_hidden)
        return VISIT_SKIP;
    } else {
      if(the_object_to_show == n)
        now_hidden = true;
    }
    return VISIT_CONTINUE;
  }

  virtual VisitResult visitStyle(StyleNode *p, VisitState vs) {
    if(p->localStyleSet().has_hidden() && p->localStyleSet().hidden())
      return VISIT_SKIP; // optimization: skip over hidden nodes
    if(vs == VISIT_PRE)
      fprintf(f, "gsave\n");
    else 
      fprintf(f, "grestore\n");
    return VISIT_CONTINUE;
  }

  virtual VisitResult visitGroup(Group *g, VisitState vs) {
    if(vs == VISIT_PRE) {
      if(g == the_object_to_show)
        now_hidden = false;
    } else {
      if(g == the_object_to_show)
        now_hidden = true;
    }

    return VISIT_CONTINUE;
  }

  virtual VisitResult visitXForm(XFormNode *n, VisitState vs) { return VISIT_CONTINUE; }

  virtual void unscale(double origin_x, double origin_y) { 
    double factor = getScaleFactor();
    pushMatrix();
    multXForm(XForm::translation(Vect3(origin_x, origin_y, 0)));
    multXForm(XForm::uniform_scale(1/factor));
  }
  virtual void rescale() { 
    popMatrix();
  }
  virtual double getScaleFactor() {
    return sqrt(fabs(determinant(xform_stack.back())))*window_height_pixels;
  }

  virtual void pushMatrix() {
    xform_stack.push_back(xform_stack.back());
  }
  virtual void multXForm(const XForm &xf) {
    xform_stack.back() *= xf;
  }
  virtual void popMatrix() {
    xform_stack.pop_back();
  }

  virtual void beginPolyline() {
    vertno = 0;
    fprintf(f, "newpath\n");
    mode = PLINE;
  }
  virtual void beginFilledPolygon() {
    vertno = 0;
    fprintf(f, "newpath\n");
    mode = POLYGON;
  }
  virtual void beginUnfilledPolygon() {
    vertno = 0;
    fprintf(f, "newpath\n");
    mode = UPOLYGON;
  }
  virtual void beginPoints() {
    vertno = 0;
    mode =  POINTS;
  }
  virtual void end() {
    switch(mode) {
    case PLINE:
      fprintf(f, "stroke\n");
      break;
    case UPOLYGON:
      fprintf(f, "closepath\nstroke\n");
      break;
    case POLYGON:
      fprintf(f, "closepath\nfill\n");
    }
    mode = NONE;
  }

  virtual void infiniteVertex(double x, double y) {
    // TODO we ignore this here, should just draw off the page
  }

  virtual void vertex(double x, double y) {
    if(mode == NONE) return;

    Vect3 v = Vect3(x, y, 1);
    Vect3 world_point = xform_stack.back() * v;

    if(mode == POINTS) {
        // GL point size is diameter, while we want radius
        fprintf(f, "%g %g %g 0 360 arc closepath fill\n", world_point[0], world_point[1], pointsize/2.);
    } else {
      if(vertno == 0)
        fprintf(f, "%g %g moveto\n", world_point[0], world_point[1]);
      else
        fprintf(f, "%g %g lineto\n", world_point[0], world_point[1]);
      vertno++;
    }
  }

  virtual void text(double x, double y, const char *t) {
    Vect3 v = Vect3(x, y, 1);
    Vect3 world_point = xform_stack.back() * v;

    fprintf(f, "newpath\n");
    fprintf(f, "%g %g moveto\n", world_point[0], world_point[1]);
    fprintf(f, "(%s) show\n", t);
  }

  virtual void affineText(double x, double y, const char *t) {
    XForm xf = xform_stack.back();

    fprintf(f, "gsave\n");
    fprintf(f, "newpath\n");
    fprintf(f, "[%g %g %g %g %g %g] concat\n", xf(0,0), xf(1,0), xf(0,1), xf(1,1), xf(0,2), xf(1,2));
    fprintf(f, "%g %g moveto\n", x, y);
    fprintf(f, "(%s) show\n", t);
    fprintf(f, "grestore\n");
  }

  virtual void image(double x1, double y1, double x2, double y2, Image *img) {
    XForm xf = xform_stack.back();

    fprintf(f, "gsave\n");
    fprintf(f, "newpath\n");
    fprintf(f, "[%g %g %g %g %g %g] concat\n", xf(0,0), xf(1,0), xf(0,1), xf(1,1), xf(0,2), xf(1,2));
    fprintf(f, "%g %g translate\n", x1, y1);
    fprintf(f, "[%g 0 0 %g 0 0] concat\n", (x2-x1), (y2-y1));

    fprintf(f, "/picstr %d string def\n", 3*img->width());
    fprintf(f, "%d %d %d\n", img->width(), img->height(), 8);
    fprintf(f, "[%d 0 0 %d 0 0]\n", img->width(), img->height());
    fprintf(f, "{currentfile picstr readhexstring pop}\n");
    fprintf(f, "false 3\n");
    fprintf(f, "colorimage\n\n");

    unsigned char *data = img->data();
    const int img_height = img->height(), img_width = img->width(); 
    int offset = (img->glFormat() == GL_RGBA) ? 4 : 3;

    for(int y = 0; y < img_height; ++y) {
      for(int x = 0; x < img_width; ++x) {
        unsigned char
          r = data[offset*(img_width*y+x)+0],
          g = data[offset*(img_width*y+x)+1],
          b = data[offset*(img_width*y+x)+2];

        fprintf(f, "%.2x%.2x%.2x", r, g, b);
      }
      fprintf(f, "\n");
    }

    fprintf(f, "grestore\n");
  }

  void saveMode(MODE sav) {
    if(sav != NONE) end();
  }
  void restoreMode(MODE sav) {
    switch(sav) {
      case PLINE: beginPolyline(); break;
      case POLYGON: beginFilledPolygon(); break;
      case UPOLYGON: beginUnfilledPolygon(); break;
      case POINTS: beginPoints(); break;
      case NONE: break; // nothing necessary
    }
  }


  virtual void setHidden(bool h) {} // Not necessary
  virtual void setLineWidth(double w) {
    MODE savemode = mode;
    saveMode(savemode);
    fprintf(f, "%g setlinewidth\n", w*thicken);
    restoreMode(savemode);
  }

  virtual void setPointSize(double s) {
    MODE savemode = mode;
    saveMode(savemode);
    fprintf(f, "%%setpointsize %g\n", s*thicken);
    pointsize = s*thicken;
    restoreMode(savemode);
  }

  virtual void setColor(float r, float g, float b, float a = 1) {
    MODE savemode = mode;
    saveMode(savemode);
    fprintf(f, "%g %g %g setrgbcolor\n",r,g,b);
    restoreMode(savemode);
  }

  virtual void setFont(GLFont *font) {
    MODE savemode = mode;
    saveMode(savemode);

    fprintf(f, "/Times-Roman findfont\n"); // no multiple fonts for now
    fprintf(f, "%d scalefont\n", int(font->size()*thicken));
    fprintf(f, "setfont\n");
    restoreMode(savemode);
  }

  virtual void beginConcaveFilledPolygon() { beginFilledPolygon(); }
  virtual void concaveVertex(double x, double y) { vertex(x, y); }
  virtual void endConcaveFilledPolygon() { end(); }
};

void print(const char *file, GraphicsView *c, double width, GraphicsNode *g, double thicken, double border) {
  PSPrinterVisitor printer(file, thicken);
  if(!printer.valid()) return; // couldn't open file
  if(!g) g = c->sceneGraph()->rootGroup();

  XForm viewmat = c->getProjectionMatrix();

  double minX, minY, maxX, maxY;
  c->sceneGraph()->getBoundingRect(viewmat, g,
      minX, minY, maxX, maxY);

  setup_view(printer, width, viewmat, g, minX, minY, maxX, maxY, border, false);

  c->sceneGraph()->traverse(&printer);
}

void print(const char *file, GraphicsView *c, double width,
    int x1, int y1, int x2, int y2, GraphicsNode *g,
    double thicken) {
  PSPrinterVisitor printer(file, thicken);
  if(!printer.valid()) return; // couldn't open file
  if(!g) g = c->sceneGraph()->rootGroup();

  XForm viewmat = c->getProjectionMatrix();

  double minX = x1/double(c->height());
  double minY = (c->height()-y2)/double(c->height());
  double maxX = x2/double(c->height());
  double maxY = (c->height()-y1)/double(c->height());

  setup_view(printer, width, viewmat, g, minX, minY, maxX, maxY, 0, true);

  c->sceneGraph()->traverse(&printer);
}

void setup_view(PSPrinterVisitor &printer, double width, XForm viewmat, GraphicsNode *g,
    double x1, double y1, double x2, double y2,
    double border, bool clip) {
  // translate to the origin
  viewmat = XForm::translation(Vect3(-x1, -y1,0)) * viewmat;
  x2 -= x1;
  x1 = 0;
  y2 -= y1;
  y1 = 0;

  // scale to proper width, remember postscript units are 72nds of an inch
  double scale = width*72/x2;
  viewmat = XForm::uniform_scale(scale) * viewmat;
  x1 *= scale;
  y1 *= scale;
  x2 *= scale;
  y2 *= scale;

  // add a inch border, just for safety
  // technically, the EPS format requires that you take line width, point size, etc.
  // into account when calculating the bounding box, but that'd be a massive pain
  viewmat = XForm::translation(Vect3(border*72,border*72,0)) * viewmat;
  x2 += 2*border*72;
  y2 += 2*border*72;

  printer.setView(viewmat, g, x1, y1, x2, y2, clip);
}

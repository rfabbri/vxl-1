#ifndef BOUNDS_H_INCLUDED
#define BOUNDS_H_INCLUDED

#include "basegui_visitor.h"
#include "basegui_xform.h"
#include "math.h"

// this is a private scene graph visitor used in scenegraph.cpp to
// get the bounds of a given group

class BoundsVisitor : public SceneGraphVisitor 
  {
    private:
        double minX, maxX, minY, maxY;
        std::vector<XForm> xform_stack;
        bool ignore;
        
    public:
//        BoundsVisitor(const XForm &master_xform) : minX(HUGE_VAL), maxX(-HUGE_VAL), minY(HUGE_VAL), maxY(-HUGE_VAL), ignore(false)
        BoundsVisitor(const XForm &master_xform)
          {
            minX=(HUGE_VAL);
            maxX=(-HUGE_VAL);
            minY=(HUGE_VAL);
            maxY=(-HUGE_VAL);
            ignore=false;
            xform_stack.push_back(master_xform);
           };
        virtual ~BoundsVisitor(){};
        void getRect(double &x1, double &y1, double &x2, double &y2) 
          {
            x1 = minX;
            y1 = minY;
            x2 = maxX;
            y2 = maxY;
           }
        
        virtual VisitResult visitView(GraphicsView *s, VisitState vs) { return VISIT_CONTINUE; }
        virtual VisitResult visitSceneGraph(SceneGraph *s, VisitState vs) { return VISIT_CONTINUE; }
        virtual VisitResult visitGeometry(GraphicsNode *n, VisitState vs) { return VISIT_CONTINUE; }
        virtual VisitResult visitStyle(StyleNode *p, VisitState vs) { return VISIT_CONTINUE; }
        virtual VisitResult visitGroup(Group *g, VisitState vs) { return VISIT_CONTINUE; }
        virtual VisitResult visitXForm(XFormNode *n, VisitState vs) { return VISIT_CONTINUE; }
        
        virtual void unscale(double origin_x, double origin_y) { ignore = true; } // don't include stuff
        virtual void rescale() { ignore = false; }
        virtual double getScaleFactor() { return .001; }
        
        virtual void pushMatrix() 
          {
            xform_stack.push_back(xform_stack.back());
           }
        virtual void multXForm(const XForm &xf) 
          {
            xform_stack.back() *= xf;
           }
        virtual void popMatrix() 
          {
            xform_stack.pop_back();
           }
        
        virtual void beginPolyline() {}
        virtual void beginFilledPolygon() {}
        virtual void beginUnfilledPolygon() {}
        virtual void beginPoints() {}
        virtual void end() {}
        
        virtual void vertex(double x, double y) 
          {
            if(ignore) return;
            
            Vect3 v = Vect3(x, y, 1);
            Vect3 world_point = xform_stack.back() * v;

    if(world_point[0] < minX) minX = world_point[0];
    if(world_point[0] > maxX) maxX = world_point[0];
    if(world_point[1] < minY) minY = world_point[1];
    if(world_point[1] > maxY) maxY = world_point[1];
  }
  virtual void infiniteVertex(double vx, double vy) {
    if(ignore) return;

    if(vx < 0) minX = -HUGE_VAL;
    if(vx > 0) maxX = +HUGE_VAL;
    if(vy < 0) minY = -HUGE_VAL;
    if(vy > 0) maxY = +HUGE_VAL;
  }

  // TODO this doesn't consider the extents of the text
  virtual void text(double x, double y, const char *t) { vertex(x, y); }
  virtual void affineText(double x, double y, const char *t) { vertex(x, y); }
  virtual void image(double x1, double y1, double x2, double y2, Image *img) {
    vertex(x1, y1);
    vertex(x1, y2);
    vertex(x2, y2);
    vertex(x2, y1);
  }

  virtual void setLineWidth(double w) {}
  virtual void setHidden(bool h) { ignore = h; }
  virtual void setPointSize(double s) {}
  virtual void setColor(float r, float g, float b, float a = 1) {}
  virtual void setFont(GLFont *font) {}

  virtual void beginConcaveFilledPolygon() {}
  virtual void concaveVertex(double x, double y) { vertex(x, y); }
  virtual void endConcaveFilledPolygon() {}
};

#endif

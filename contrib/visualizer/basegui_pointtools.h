#ifndef POINTTOOLS_H_INCLUDED
#define POINTTOOLS_H_INCLUDED

#include "basegui_gtool.h"
#include "base_options.h"
#include "basegui_BaseWindow.h"
#include <math.h>

//////////////////////////////////
//POINT TOOL

template <class T>
class PointTool: public DrawingTool 
{
public:

  PointTool(BaseWindow *win, Group *pgroup):
    DrawingTool(win, pgroup), state(NONE)  { }
  
protected:
  enum { NONE, RUBBER_BANDING } state;
  T* g; //PointGeom
  
  virtual Result complete()
  {  
    //GfxCommand *c = new AddGeomCommand(g, permanent_group);
    return COMPLETED;
  }

  Result leftMouseDown(wxMouseEvent &evt) 
  {
    if(state == NONE) {
      g = new T(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
      tmp()->addChild(g);
      state = RUBBER_BANDING;
      _window->glw()->Refresh();
    }  
    return HANDLED;
  }

  Result leftMouseUp(wxMouseEvent &evt) 
  {
    if(state == NONE) return UNHANDLED;
  
    g->setPoint(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
    state = NONE;
    _window->glw()->Refresh();
    return complete();
  }

  Result mouseMove(wxMouseEvent &evt) 
  {
    if(state == NONE) return UNHANDLED;
    g->setPoint(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
  
    _window->glw()->Refresh();

    return HANDLED;
  }

  Result abort() 
  {
    if(state == NONE) return UNHANDLED;
  
    tmp()->removeChild(g);
    delete g; g = NULL;
  
    state = NONE;
    return ABORTED;
  }
};


//////////////////////////////////
//POINT FREEHAND TOOL

template <class T>
class PointFreehandTool: public DrawingTool {
public:
  PointFreehandTool(BaseWindow* win, Group *pgroup):
    DrawingTool(win,pgroup), state(NONE)
  {
    //threshold=3;
    if(GOptions["Base"].count("PointsThreshold")>0){
      threshold = atoi(GOptions["Base"]["PointsThreshold"].c_str());
    }
  }

protected:
  enum { NONE, DRAWING } state;
  std::vector<T *> points; //PointGeom
  int threshold;
  Point2D<double> prevPt;

  virtual Result complete()
  {
    //MultiCommand *c = new MultiCommand;
    //while(points_group->numChildren() != 0) {
    //  c->addCommand(new AddGeomCommand(points_group->child(0), permanent_group));
    //  points_group->removeChild(points_group->child(0));
    //}
    points.clear();
    return COMPLETED;
  }

  Result leftMouseDown(wxMouseEvent &evt)
  {
    if(state == NONE) { 
       T *g = new T(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
       tmp()->addChild(g);
       points.push_back(g);
       _window->glw()->Refresh();
 
       state = DRAWING;
       return HANDLED;
    }
    return UNHANDLED;
  }

  Result leftMouseUp(wxMouseEvent &evt)
  {
    if(state == NONE) return UNHANDLED;

    // don't add a point here, because of X event queueing wierdness.
    // often, there are some mouse moves which are sent before the mouseup,
    // but which correspond to moves happening _after_ the mouse up (because
    // of differing latencies of the event queues). this can cause our path
    // to double back on itself, so just ignore the mouse up.
    state = NONE;
    
    return complete();
  }

  Result mouseMove(wxMouseEvent &evt)
  {
    if(state == NONE) return UNHANDLED;

    Point2D<double> pt 
     = view->unproject(Point2D<double>(evt.GetX(), evt.GetY()));

    if(threshold==0 || 
      sqrt(pow((pt.getX()-prevPt.getX()),2)+pow((pt.getY()-prevPt.getY()),2))
      >=(double)threshold)
    {
       T *g = new T(pt);
       tmp()->addChild(g);
       points.push_back(g);
       _window->glw()->Refresh();
       prevPt=pt;
    }

    return HANDLED;
  }


  Result abort()
  {
    if(state == NONE) return UNHANDLED;

    state = NONE;
    points.clear();
    return ABORTED;
  }

};

#endif

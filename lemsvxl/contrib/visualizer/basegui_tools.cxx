#include "basegui_tools.h"

#include "command.h"
#include "basegui_style.h"
#include "basegui_geomnodes.h"
#include "base_options.h"
#include "basegui_debugdisplayer.h"

#include <vcl_cmath.h>

extern Options GOptions;
//StyleSet DrawingTool::_style;

//PAN TOOL

PanTool::PanTool(BaseWindow* win) :
  GTool(win), state(NONE) {}

GTool::Result PanTool::leftMouseDown(wxMouseEvent &evt) 
{
  if(state != NONE) return UNHANDLED;
  
  state = PAN;
  old_x = evt.GetX();
  old_y = evt.GetY();

  return HANDLED;
}

GTool::Result PanTool::leftMouseUp(wxMouseEvent &evt) 
{
  if(state == NONE) return UNHANDLED;
  state = NONE;
  return HANDLED;
}

GTool::Result PanTool::mouseMove(wxMouseEvent &evt) 
{
  int x = evt.GetX(), y = evt.GetY();
  
  switch(state) {
  case NONE:
    return UNHANDLED;
    
  case PAN:
    _window->glw()->view()->translate(Point2D<double>(x-old_x, y-old_y));
    old_x = x;
    old_y = y;
   _window->glw()->Refresh();

    return HANDLED;

  case ZOOM: {
    if (y-old_y < 0)
      _window->zoom(1.1);
    else if (y-old_y > 0)
      _window->zoom(1.0/1.1);

    // exponential zoom curve
    //double zoom_ratio = 
    //  exp(4*double(    y-mousedown_y)/view->height()) /
    //  exp(4*double(old_y-mousedown_y)/view->height());
    
    // this keeps fixed whatever point your mouse was over when the click
    // began. This is good for "drill down" style zooming
    //view->zoom(Point2D<double>(center_x, center_y), zoom_ratio);

    old_x = x;
    old_y = y;
  }
    return HANDLED;
  }
}

GTool::Result PanTool::rightMouseDown(wxMouseEvent &evt) 
{
  if(state != NONE) return UNHANDLED;
  
  state = ZOOM;
  mousedown_y = evt.GetY();
  old_x = evt.GetX();
  old_y = evt.GetY();
  center_x = evt.GetX();
  center_y = evt.GetY();
  return HANDLED;
}

GTool::Result PanTool::rightMouseUp(wxMouseEvent &evt)
{
  if(state == NONE) return UNHANDLED;
  state = NONE;
  return HANDLED;
}

GTool::Result PanTool::abort() {
  state = NONE;
  return UNHANDLED;
}

//GET INFO TOOL

GetInfoTool::GetInfoTool(BaseWindow* win) :
  GTool(win) {}

GTool::Result GetInfoTool::leftMouseDown(wxMouseEvent &evt)
{
  if (_window->DisplayDebugInfo()==VIS_SHOW_DEBUG_INFO)
    _window->displayDebugInfo(evt);
  
  return HANDLED;
}

GTool::Result GetInfoTool::mouseMove(wxMouseEvent &evt)
{
  if (_window->AutoHighlight())
        _window->HighlightSelected(evt);

  vcl_ostringstream ostrm;
  ostrm.precision (5);
  Point2D<double> mouse_pos (evt.GetX(),evt.GetY());

  ostrm << "Scale Factor: " << _window->view()->getScaleFactor();
  ostrm << ", Mouse Position: (" 
      << mouse_pos.getX() << "," << mouse_pos.getY() << ")";
  Point2D<double> actual_pos =   _window->view()->unproject (mouse_pos);
  ostrm << ", World Position: (" 
      << actual_pos.getX() << "," << actual_pos.getY() << ")";
  GDebugDisplayer.status(ostrm.str());

  return HANDLED;
}

GTool::Result GetInfoTool::abort(){return HANDLED;}

////////////////
//DELETE TOOL

DeleteTool::DeleteTool(BaseWindow* win) :
  GTool(win) , state(NONE) {}

GTool::Result DeleteTool::leftMouseDown(wxMouseEvent &evt)
{
  if (state==NONE){
    state = DELETING;
  }

  if (state==DELETING){
    Point2D<double> mouse_position(evt.GetX(),evt.GetY());
    
    vcl_vector<HitRecord> hits;
    if(_window->_hitTest(hits,mouse_position)==0)
        return HANDLED;

    vcl_vector<HitRecord>::iterator it = hits.begin();
    for ( ;it != hits.end(); ++it) {
      GraphicsNode *hitObj=it->hit_object;
      
      if (hitObj){
        hitObj->deleteSelf();
        hitObj = NULL;
      }
        
    }

    //clear the _prevHits list because all the elements are 
    //probably deleted
    _window->_prevHits.clear();

    _window->Refresh();
  }

  return HANDLED;
}

GTool::Result DeleteTool::mouseMove(wxMouseEvent &evt)
{
  if (state==NONE){
    if (_window->AutoHighlight())
        _window->HighlightSelected(evt);
  }

  if (state==DELETING){
    Point2D<double> mouse_position(evt.GetX(),evt.GetY());
    
    vcl_vector<HitRecord> hits;
    if(_window->_hitTest(hits,mouse_position)==0)
        return HANDLED;

    vcl_vector<HitRecord>::iterator it = hits.begin();
    for ( ;it != hits.end(); ++it) {
      GraphicsNode *hitObj=it->hit_object;
      
      if (hitObj){
        hitObj->deleteSelf();
        hitObj = NULL;
      }
        
    }

    //clear the _prevHits list because all the elements are 
    //probably deleted
    _window->_prevHits.clear();

    _window->Refresh();
  }
  return HANDLED;
}

GTool::Result DeleteTool::leftMouseUp(wxMouseEvent &evt)
{
  if (state==DELETING){
    state=NONE;
    //clear the _prevHits list because all the elements are 
    //probably deleted
    _window->_prevHits.clear();
  }

  return HANDLED;
}

GTool::Result DeleteTool::abort()
{
  state=NONE;
  return HANDLED;
}

//////////////////////////////////
//ARRROW TOOL

ArrowTool::ArrowTool(BaseWindow* win, Group *pgroup):
  DrawingTool(win,pgroup), arrow(NULL), state(NONE) {}

GTool::Result ArrowTool::leftMouseDown(wxMouseEvent &evt) {
  arrow = new ArrowGeom( view->unproject(Point2D<double>(evt.GetX(), evt.GetY())), view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
  
  tmp()->addChild(arrow);
  _window->Refresh();
  state = RUBBER_BANDING;
  return HANDLED;
}

GTool::Result ArrowTool::leftMouseUp(wxMouseEvent &evt)
{
  if(state != RUBBER_BANDING) return UNHANDLED;
  
  arrow->setEnd(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
  
  _window->Refresh();
  state = NONE;
  
  return COMPLETED;
}

GTool::Result ArrowTool::mouseMove(wxMouseEvent &evt) 
{
  if(state == NONE) return UNHANDLED;
 
  arrow->setEnd(view->unproject(Point2D<double>(evt.GetX(), evt.GetY())));
 
  _window->Refresh();
  return HANDLED;
}

GTool::Result ArrowTool::rightMouseDown(wxMouseEvent &evt) { return UNHANDLED; }
GTool::Result ArrowTool::rightMouseUp(wxMouseEvent &evt)
{
  return abort();
}

GTool::Result ArrowTool::abort() 
{
  if(state == NONE) return UNHANDLED;
  
  state = NONE;
  tmp()->removeChild(arrow);
  delete arrow; arrow=NULL;
  
  return ABORTED;
}

//////////////////////////////////
//EULER SPIRAL TOOL

EulerSpiralTool::EulerSpiralTool(BaseWindow* win, Group *pgroup):
  DrawingTool(win,pgroup) 
{
  state=SPIRALING;
  _arrowTool = new ArrowTool(win, pgroup);
  _curArrow  = 0;
  _esSpline  = 0;
}

EulerSpiralTool::~EulerSpiralTool()
{
  delete _arrowTool;
  _arrowTool=0;
  
  if (_esSpline)
    {
      tmp()->removeChild(_esSpline);
      delete _esSpline;
      
      _curArrow  = 0;
      _esSpline  = 0;
      state      = NONE;
    }
}

GTool::Result EulerSpiralTool::leftMouseDown(wxMouseEvent &evt) 
{
  state=SPIRALING;
  Result res=_arrowTool->leftMouseDown(evt);
  if (res!=HANDLED)
    {
      state=NONE;
    }
   
  return res;
}

GTool::Result EulerSpiralTool::leftMouseUp(wxMouseEvent &evt)
{
 if (state!=SPIRALING)
    return UNHANDLED;
  
  Result res=_arrowTool->leftMouseUp(evt);
  if (res==COMPLETED)
    {
      _curArrow= (ArrowGeom*)tmp()->back(); 
      if (!_esSpline)
    {
      _esSpline= new EulerSpiralSplineGeom();
      tmp()->addChild(_esSpline);
    }
      _esSpline->push_back(_curArrow);
      
      return HANDLED;
    } 
  
  //TODO
  vcl_cout<<"TODO: Something weird happening"<<vcl_endl;
  
  return res;
}

GTool::Result EulerSpiralTool::mouseMove(wxMouseEvent &evt) 
{
  if (state!=SPIRALING)
    return UNHANDLED;
  
  Result res=_arrowTool->mouseMove(evt);
  
  return res;

}

GTool::Result EulerSpiralTool::rightMouseDown(wxMouseEvent &evt) { return UNHANDLED; }
GTool::Result EulerSpiralTool::rightMouseUp(wxMouseEvent &evt)
{
  if (state!=SPIRALING)
    return UNHANDLED;
  
  _arrowTool->rightMouseDown(evt);

  if (_esSpline)
    {
      _curArrow  = 0;
      _esSpline  = 0;
      state      = NONE;
      return COMPLETED;
    }

  return HANDLED;
}

GTool::Result EulerSpiralTool::abort() {
  if(state != SPIRALING)
    return UNHANDLED;
  if(_esSpline){
    tmp()->removeChild(_esSpline);
  delete _esSpline;
  _esSpline = 0;
  }
  
  state = NONE;
  return ABORTED;
}

//////////////////////////////////
//BIARC TOOL

BiArcTool::BiArcTool(BaseWindow* win, Group *pgroup):
  DrawingTool(win,pgroup) 
{
  state=BIARCING;
  _curArrow = 0;

  _arrowTool = new ArrowTool(win, pgroup);
  _baSpline  = 0;
}

BiArcTool::~BiArcTool()
{
  delete _arrowTool;
  _arrowTool=0;
  
  if (_baSpline)
    {
      tmp()->removeChild(_baSpline);
      delete _baSpline;

      _baSpline  = 0;
      state      = NONE;
    }
  
}
              
GTool::Result BiArcTool::leftMouseDown(wxMouseEvent &evt) 
{
  state=BIARCING;
  Result res=_arrowTool->leftMouseDown(evt);
  if (res!=HANDLED)
    {
      state=NONE;
    }
  return res;
}

GTool::Result BiArcTool::leftMouseUp(wxMouseEvent &evt)
{
  if (state!=BIARCING)
    return UNHANDLED;
  
  Result res=_arrowTool->leftMouseUp(evt);
  if (res==COMPLETED)
    {
      _curArrow = (ArrowGeom*)tmp()->back();
      if (!_baSpline)
    {
      _baSpline= new BiArcSplineGeom();
      tmp()->addChild(_baSpline);
    }
      _baSpline->push_back(_curArrow);
      
      return HANDLED;
    } 
  
  //TODO
  vcl_cout<<"TODO: Something weird happening"<<vcl_endl;
  return res;
}

GTool::Result BiArcTool::mouseMove(wxMouseEvent &evt) 
{
  if (state!=BIARCING)
    return UNHANDLED;
  
  Result res=_arrowTool->mouseMove(evt);
  return res;
}

GTool::Result BiArcTool::rightMouseDown(wxMouseEvent &evt) { return UNHANDLED; }
GTool::Result BiArcTool::rightMouseUp(wxMouseEvent &evt)
{
  if (state!=BIARCING)
    return UNHANDLED;
  
  _arrowTool->rightMouseDown(evt);
  if (_baSpline)
    {
      tmp()->removeChild(_baSpline);
      _curArrow  = 0;
      _baSpline  = 0;
      state      = NONE;
      return COMPLETED;
    }
}

GTool::Result BiArcTool::abort() {
  if(state != BIARCING)
    return UNHANDLED;
  if (_baSpline){
  tmp()->removeChild(_baSpline);
  delete _baSpline;
  _baSpline = 0;
  }
  
  state = NONE;
  return ABORTED;
}

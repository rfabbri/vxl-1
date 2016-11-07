#ifndef GTOOL_H_INCLUDED
#define GTOOL_H_INCLUDED

#include <wx/wx.h>
#include <wx/event.h>

//Generic tool and Drawing Tool
class GTool;
class DrawingTool;
class BaseWindow;
class GraphicsView;

//#include "BaseWindow.h"
#include "basegui_geomnodes.h"
#include "basegui_style.h"

//Generic Tool class
class GTool 
{
public:
  BaseWindow* _window;

public:
  GTool(BaseWindow* win){_window = win;}
  virtual ~GTool(){}

  enum Result {
      UNHANDLED=0,
      HANDLED,
      COMPLETED,
      ABORTED,
  };

  virtual Result complete()              { return COMPLETED; }
  virtual Result abort()                            { return UNHANDLED; }
  virtual Result leftMouseDown(wxMouseEvent &evt)   { evt.Skip(); return UNHANDLED; }
  virtual Result leftMouseUp(wxMouseEvent &evt)     { evt.Skip(); return UNHANDLED; }
  virtual Result middleMouseDown(wxMouseEvent &evt) { evt.Skip(); return UNHANDLED; }
  virtual Result middleMouseUp(wxMouseEvent &evt)   { evt.Skip(); return UNHANDLED; }
  virtual Result rightMouseDown(wxMouseEvent &evt)  { evt.Skip(); return UNHANDLED; }
  virtual Result rightMouseUp(wxMouseEvent &evt)    { evt.Skip(); return UNHANDLED; }
  virtual Result mouseMove(wxMouseEvent &evt)       { evt.Skip(); return UNHANDLED; }

  virtual Result keyDown(wxKeyEvent &evt)            { evt.Skip(); return UNHANDLED; }
  virtual Result keyUp(wxKeyEvent &evt)        { evt.Skip(); return UNHANDLED; }
  virtual Result charUp(wxKeyEvent &evt)        { evt.Skip(); return UNHANDLED; }
 
};

// Drawing Tool
class DrawingTool: public GTool {

protected:
  Group    *drawing_group;
  StyleSet  _style;
  GraphicsView *view;
  Group  *permanent_group;
 
  Group *tmp() { assert(drawing_group); return drawing_group; }

public:
  DrawingTool(BaseWindow* win, Group *pgroup);
  virtual ~DrawingTool();

  virtual Result complete(){ return COMPLETED;}

  StyleSet getStyle(){ return _style;}
  int mergeStyle(StyleSet style_){
    _style=style_;
    return 0;
   }
 
};

#endif

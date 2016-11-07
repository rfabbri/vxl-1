#ifndef DRAWING_WINDOW_H_INCLUDED
#define DRAWING_WINDOW_H_INCLUDED

#include <wx/wx.h>
#include<wx/colordlg.h>

#include "basegui_basewindow.h"

class DrawingWindow: public BaseWindow 
{
protected:
  wxMenu   *_tools_menu;
  Group* _drawing_group;

public:
  DrawingWindow(MainGUIWindow *parent, const vcl_string &title, 
          BasePlugin *plugin=NULL);
  ~DrawingWindow();

  // returns the group into which everything was (will be) drawn
  Group* drawing_group() {return _drawing_group;}

  virtual void addCustomMenus();

  //Custom Menu handlers
   void OnPenColor ();
   void OnPenWidth ();

  virtual GTool* getToolFromID(int id);

  DECLARE_EVENT_TABLE()
};

// menu items ids
enum 
{
    VIS_SET_PEN_COLOR=CUSTOM_ID_BASE,
    VIS_SET_PEN_WIDTH,

    VIS_DRAWING_DELETE,
    VIS_DRAWING_PAN,
    VIS_DRAWING_LINE,
    VIS_DRAWING_POLYLINE,
   VIS_DRAWING_POLYGON,
    VIS_DRAWING_POLYLINE_FREEHAND,
   VIS_DRAWING_POLYGON_FREEHAND,
    VIS_DRAWING_POINT,
    VIS_DRAWING_POINT_FREEHAND,
    VIS_DRAWING_ARC,
    VIS_DRAWING_ARC_SPLINE,
    VIS_DRAWING_EULER_SPIRAL_SPLINE,
    VIS_DRAWING_BI_ARC_SPLINE,
    VIS_DRAWING_ARROW,
    VIS_DRAWING_FIRST = VIS_DRAWING_DELETE,
    VIS_DRAWING_LAST  = VIS_DRAWING_ARROW
};

#endif

#include "basegui_gtool.h"
#include "basegui_basewindow.h"
//#include "Options.h"


DrawingTool::DrawingTool(BaseWindow* win, Group *pgroup):
    GTool(win), view(win->view()), permanent_group(pgroup)
  {
    if (!permanent_group){
      drawing_group = new Group();
      view->sceneGraph()->rootGroup()->addChild(drawing_group);
    }
    else {
      drawing_group = permanent_group;
    }
  }

DrawingTool::~DrawingTool()
  {
    if(!permanent_group) {
      view->sceneGraph()->rootGroup()->removeChild(drawing_group);
      delete drawing_group;
    }
  }


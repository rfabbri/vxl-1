#ifndef DRAWTESTWINDOW_H_INCLUDED
#define DRAWTESTWINDOW_H_INCLUDED

#include "basegui_drawingwindow.h"

class DrawTestWindow : public DrawingWindow {
public:
  DrawTestWindow(MainGUIWindow *win, BasePlugin *plugin);
  ~DrawTestWindow() {}

  void addCustomMenus(wxMenuBar *menu,
        wxMenu *file, wxMenu *help, std::vector<wxAcceleratorEntry> &accels);
  void menuItemSelected(wxCommandEvent &evt);
  GTool *getToolFromID(int id);
};

#endif

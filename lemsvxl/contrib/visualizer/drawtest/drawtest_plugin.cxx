#include "basegui_baseplugin.h"

#include "basegui_pluginmanager.h"
#include "drawtest_window.h"

class DrawPlugin: public BasePlugin {
public:
  DrawPlugin() {
    SetPluginName("DRAWING");
    AddToPluginServices("DRW");
    SetPluginDescription("Plugin for drawing basic shapes\n\n");
    SetPluginMenuName("&Drawing Tools");

    PluginManager::RegisterPlugin(this);
  }

  ~DrawPlugin() {
    PluginManager::UnRegisterPlugin(this);
  }

  virtual BaseWindow *GetPluginWindow(MainGUIWindow *win) {
    return new DrawTestWindow(win,this);
  }
};

DrawPlugin drawplugin;

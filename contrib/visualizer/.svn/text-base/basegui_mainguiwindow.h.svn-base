#ifndef MAINGUIWINDOW_H 
#define MAINGUIWINDOW_H

#include <wx/wx.h>
#include <wx/mdi.h>
#include <wx/sashwin.h>
#include <wx/laywin.h>
#include <wx/notebook.h>
#include <vcl_vector.h>

#include "basegui_basewindow.h"
#include "basegui_baseplugin.h"
#include "basegui_pluginmanager.h"
#include "base_options.h"
#include "basegui_allbitmaps.h"


//! Our MDI parent class
/*!
  This is a skeleton class to create a GUI main().
  You can implement your own command-argument processing in OnInit().
 */
class VisApp : public wxApp
{
public:
    bool OnInit();
};
DECLARE_APP(VisApp);

class BaseWindow;

//! Our main()
/*!
  This class provides the basic functionalities of creating a child-frame,
  loading a file, supplying a help window, debug-window, status-bar, and such.
  Anything specific to each plugin child-window is implemented in 
  the plug-in window classes, which are derived from BaseWindow class.
*/
class MainGUIWindow : public wxMDIParentFrame
{
private:
  //don't bother to delete the menus.
  //They are automatically deleted
  wxMenuBar *mainMenuBar;
  wxMenu *fileMenu, *helpMenu, *pluginsMenu;
  wxToolBar *mainToolbar;
  vcl_vector<wxAcceleratorEntry >_accEntries;

public:
    MainGUIWindow(wxWindow *parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos, const wxSize& size, const long style);

    void loadFile(vcl_string filename);


    void createStdMenus(wxMenu *file, wxMenu *plugins, wxMenu *help,
      wxToolBar *toolBar,
      vcl_vector<wxAcceleratorEntry> &accelerators);

   wxToolBar* mainToolBar() {return mainToolbar;}

    void OnSize(wxSizeEvent& event);
    void OnShowHideDebug();
    void OnDisplayOptions(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSashDrag(wxSashEvent& event);
    void OnLoadFile(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    BaseWindow* getPlugin(wxCommandEvent &event);
    BaseWindow* getPluginFromName(vcl_string name);
    int         getPluginCount() ;
    vcl_string      getUniquePluginName();
    void printPluginList();
 private:
    void _newWindow(BaseWindow*win);
    void InitToolBar(wxToolBar* toolBar);
    void InitAccelerators(vcl_vector<wxAcceleratorEntry> &entries);
    void InitMenus(wxMenu *file, wxMenu *plugins, wxMenu *help);
    
    // adjusts the window layout to appropriate sizes
    void relayoutWindow();
    
    DECLARE_EVENT_TABLE();
      
 private:
  wxNotebook*  _notebook;
  enum {_NumDebugWins=3 };
  wxTextCtrl* _debugTexts[_NumDebugWins]; 
  wxSashLayoutWindow* _sash;
  vcl_vector<BaseWindow*> _subframes;
};

//! Enumerator for event-id.
/*!
  This gives distinctive id for each type of event. 
  The enum definition is centralized in MainGUIWindow.h and BaseWindow.h.
  Even the events unique to plug-ins are defined in BaseWindow.h so that
  it gurantees to the uniqueness of each event.
*/


enum {
    VIS_FILE_QUIT = MAIN_GUI_ID_BASE,
    VIS_FILE_NEW_WINDOW,
    VIS_FILE_LOAD_FILE,
    VIS_HELP_SHOWHIDE_DEBUG,
    VIS_HELP_DISPLAY_OPTIONS,
    VIS_HELP_ABOUT
};

#endif    /* MAINGUIWINDOW_H */

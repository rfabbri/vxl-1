#ifndef __BASE_WINDOW_H__
#define __BASE_WINDOW_H__

#include <wx/wx.h>
#include <wx/mdi.h>
#include <vcl_vector.h>
#include <vcl_string.h>

class BasePlugin;
class MainGUIWindow;
class BaseWindow;
class GTool;

#include "base_SStream2.h"
#include "basegui_DebugDisplayer.h"
#include "basegui_scenegraph.h"
#include "basegui_glwindow.h"
#include "basegui_glgraphics.h"
#include "base_points.h"

class BaseWindow: public wxMDIChildFrame
{
  protected:
    wxMenuBar  *_mainMenubar;  
    wxMenu    *_popupMenu;
    wxMenu    *_file_menu, *_plugins_menu,*_view_menu, *_help_menu;
    wxMenu    *_edit_menu;
    wxToolBar  *_toolbar; 

    int       _debugInfoStatus;
    bool      _autoHighlight;

    //list of accelerator entries 
    vcl_vector<wxAcceleratorEntry >accelEntries;

    //command list
    //std::deque<GfxCommand*> _undo_commands, _redo_commands;

    int _currentToolId;
    GTool* _currentTool;

    void undo();
    void redo();
    void clear_undo();

  private:
    BasePlugin      *_plugin;
    SceneGraph      *_sg;
    GLWindow        *_glw;
    MainGUIWindow   *_mainGUI;

  public:
    BaseWindow(MainGUIWindow *parent, const vcl_string& title, 
         BasePlugin* plugin_=NULL);
    ~BaseWindow();

    bool bFlipView;

    // *** stuff to call from your sub-class...
    void zoom(float);
    void Refresh();
    void RefreshNow();
    void setTitle(vcl_string);
    vcl_string getTitle(void);
    virtual int processCommandLine(int argc, const char * const *argv);
  
    SceneGraph* sg(){return _sg;}
    GLWindow* glw(){return _glw;}
    GraphicsView *view(){return _glw->view();}

    vcl_vector<GraphicsNode*>     _prevHits;
    int _hitTest (vcl_vector<HitRecord>& hits_, Point2D<double>& pos_);

    int _hitTest(vcl_vector<HitRecord> &hits_,
    const Point2D<double> topLeft, const Point2D<double>bottomRight);
  
    //Message, COUT, and StatusOut
    virtual void ClearMessage (int winid=0)
      { GDebugDisplayer.clear(winid); }

    vcl_ostringstream Message; //message to output
    virtual void MessageOut (int winid=0)
    {
      Message.precision(16);
      GDebugDisplayer.disp (Message.str(), winid);
      Message.str(vcl_string()); //clear vcl_string
    }
    virtual void StatusOut (void)
    {
      GDebugDisplayer.status (Message.str());
      Message.str(vcl_string()); //clear vcl_string
    }        

    //tool operations
    virtual GTool* getToolFromID(int id);
    int currentToolId () { return _currentToolId; }
    int DisplayDebugInfo() { return _debugInfoStatus; }
    bool AutoHighlight() { return _autoHighlight; }

    // *** stuff to override...
    virtual void CompletelyCreated();
    virtual void load(const vcl_string& filename);
    virtual void load();

    virtual void HighlightSelected (wxMouseEvent &event_);
    virtual void displayDebugInfo (wxMouseEvent &event_);
    virtual void displayCurrentInfo (wxMouseEvent &event_);

    // also, override this if you don't want the standard set of menu entries
    virtual void addBasicMenus();

    // override this to add your own custom menus. the menu IDs should 
    // start at CUSTOM_ID_BASE
    virtual void addCustomMenus();
    // override this to intercept menu selections you added in
    // addCustomMenus or addBasicMenus.
    // to get the menu item id, call evt.GetId()
    virtual void menuItemSelected(wxCommandEvent &evt);

    virtual void addBasicToolstoToolbar();
    virtual void addCustomToolstoToolbar();

    // *** stuff to ignore... as far as possible
    void initMenus();

    virtual void leftMouseDown(wxMouseEvent &evt);
    virtual void leftMouseUp(wxMouseEvent &evt);
    virtual void middleMouseDown(wxMouseEvent &evt);
    virtual void middleMouseUp(wxMouseEvent &evt);
    virtual void rightMouseDown(wxMouseEvent &evt);
    virtual void rightMouseUp(wxMouseEvent &evt);
    // mouseMove is sent while buttons are up and down... call
    // evt.Dragging() to see which
    virtual void mouseMove(wxMouseEvent &evt);

    virtual void keyDown(wxKeyEvent &evt);
    virtual void keyUp(wxKeyEvent &evt);
    virtual void charUp(wxKeyEvent &evt);


    //menu event handlers
    void OnActivate(wxActivateEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnUpdateRefresh(wxUpdateUIEvent& event);
    void OnZoom(wxCommandEvent& event);
    void OnZoomIn(wxCommandEvent& event);
    void OnZoomOut(wxCommandEvent& event);
    void OnRotate(wxCommandEvent& event);
    void OnRotateLeft(wxCommandEvent& event);
    void OnRotateRight(wxCommandEvent& event);
    void OnFlip(wxCommandEvent& event);
    void OnTranslate (wxCommandEvent& WXUNUSED(event), double xDisp, double yDisp);
    void OnViewLeft(wxCommandEvent& event);
    void OnViewRight(wxCommandEvent& event);
    void OnViewUp(wxCommandEvent& event);
    void OnViewDown(wxCommandEvent& event);
    void OnCloseChild(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMove(wxMoveEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnChangeTitle(wxCommandEvent& event);
    void OnCustomMenu(wxCommandEvent& event);
    void OnUndo(wxCommandEvent &evt);
    void OnRedo(wxCommandEvent &evt);
    void OntoggleDebugInfoStatus();
    void OntoggleAutoHighlight();
  
    //tool selection handler from Menu and Toolbar
    void OnToolSelectionEvent(wxCommandEvent &evt);

  private:
    // we override this to make it private: client code should ouly call Refresh()
    // (no parameters), which does the right thing
    virtual void Refresh(bool eraseBackground, const wxRect *rect);

    DECLARE_EVENT_TABLE()
};

#include "basegui_tools.h"

const int MAIN_GUI_ID_BASE    = 100;
const int BASE_WINDOW_ID_BASE = 200;
const int CUSTOM_ID_BASE      = 300;
//
//The base ID for the plugins.
//ID's starting from this will be allocated 
//to the plugins. The actual number of ID's 
//needed will vary. 
//
//WARNING: ID's from {VIS_PLUGINS_ID_BASE} to {VIS_PLUGINS_ID_BASE+(n-1)}
//where n is the number of plugins will
//be used.
const int VIS_PLUGINS_ID_BASE = 150;

// menu items ids
enum
  {
    VIS_LOAD_FILE = BASE_WINDOW_ID_BASE,
    VIS_VIEW_ZOOM_SUBMENU,
    VIS_VIEW_ZOOM,
    VIS_VIEW_ZOOM_IN,
    VIS_VIEW_ZOOM_OUT,
    VIS_VIEW_ROTATE_SUBMENU,
    VIS_VIEW_ROTATE,
    VIS_VIEW_ROTATE_LEFT,
    VIS_VIEW_ROTATE_RIGHT,
    VIS_VIEW_TRANSLATE_SUBMENU,
    VIS_VIEW_LEFT,
    VIS_VIEW_RIGHT,
    VIS_VIEW_UP,
    VIS_VIEW_DOWN,
    VIS_VIEW_FLIP,
    VIS_CHILD_REFRESH,
    VIS_CHILD_CHANGE_TITLE,
    VIS_FILE_CLOSE_CHILD,

    VIS_EDIT_UNDO,
    VIS_EDIT_REDO,

    VIS_SHOW_DEBUG_INFO,
    VIS_HIDE_DEBUG_INFO,
    VIS_TOGGLE_DEBUG_INFO_STATUS,
    VIS_TOGGLE_AUTO_HIGHLIGHT,

    VIS_VIEW_PAN,
    VIS_GET_INFO,
    VIS_EDIT_DIRECT_SELECTION,
    VIS_EDIT_GROUP_SELECTION

   };

#endif

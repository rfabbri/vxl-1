#include "basegui_basewindow.h"
#include "basegui_mainguiwindow.h"
#include "basegui_allbitmaps.h"
#include "basegui_printer.h"

#include <vcl_sstream.h>

BaseWindow*    basePluginWnd;

// Note that VIS_FILE_NEW_WINDOW and VIS_HELP_ABOUT commands get passed
// to the parent window for processing, so no need to
// duplicate event handlers here.
    BEGIN_EVENT_TABLE(BaseWindow, wxMDIChildFrame)
    EVT_MENU(VIS_VIEW_ZOOM,                BaseWindow::OnZoom)
    EVT_MENU(VIS_VIEW_ZOOM_IN,             BaseWindow::OnZoomIn)
    EVT_MENU(VIS_VIEW_ZOOM_OUT,            BaseWindow::OnZoomOut)
    EVT_MENU(VIS_VIEW_ROTATE,              BaseWindow::OnRotate)
    EVT_MENU(VIS_VIEW_ROTATE_LEFT,       BaseWindow::OnRotateLeft)
    EVT_MENU(VIS_VIEW_ROTATE_RIGHT,       BaseWindow::OnRotateRight)
    EVT_MENU(VIS_VIEW_FLIP,                BaseWindow::OnFlip)
    EVT_MENU(VIS_VIEW_LEFT,                BaseWindow::OnViewLeft)
    EVT_MENU(VIS_VIEW_RIGHT,               BaseWindow::OnViewRight)
    EVT_MENU(VIS_VIEW_UP,                  BaseWindow::OnViewUp)
    EVT_MENU(VIS_VIEW_DOWN,                BaseWindow::OnViewDown)
    EVT_MENU(VIS_FILE_CLOSE_CHILD,         BaseWindow::OnCloseChild)
    EVT_MENU(VIS_CHILD_REFRESH,            BaseWindow::OnRefresh)
    EVT_MENU(VIS_TOGGLE_DEBUG_INFO_STATUS, BaseWindow::OntoggleDebugInfoStatus)
    EVT_MENU(VIS_TOGGLE_AUTO_HIGHLIGHT,    BaseWindow::OntoggleAutoHighlight)
    EVT_MENU(VIS_CHILD_CHANGE_TITLE,       BaseWindow::OnChangeTitle)
    EVT_MENU(VIS_EDIT_UNDO,           BaseWindow::OnUndo)
    EVT_MENU(VIS_EDIT_REDO,           BaseWindow::OnRedo)
    EVT_MENU(VIS_GET_INFO,                 BaseWindow::OnToolSelectionEvent)
    EVT_MENU(VIS_VIEW_PAN,             BaseWindow::OnToolSelectionEvent)
    EVT_MENU_RANGE(CUSTOM_ID_BASE, CUSTOM_ID_BASE+100, BaseWindow::OnCustomMenu)
    EVT_CLOSE(BaseWindow::OnClose)
END_EVENT_TABLE()

    // ---------------------------------------------------------------------------
    // BaseWindow
    // ---------------------------------------------------------------------------

    BaseWindow::BaseWindow(MainGUIWindow *parent, const vcl_string& title_,
            BasePlugin* plugin)
    : wxMDIChildFrame((wxMDIParentFrame*)parent, -1, 
            wxString(title_.c_str()), 
            wxPoint(-1,-1), wxSize(-1,-1),
            wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL | wxMAXIMIZE),
_plugin(plugin), _mainGUI(parent)
  {
    bFlipView      = false;
    _debugInfoStatus  = VIS_SHOW_DEBUG_INFO;

    _sg          = new SceneGraph();

    //set default tool
    //_currentToolId = 0;//VIS_VIEW_PAN;
    //_currentTool = 0;//new PanTool(this);

    _currentToolId = VIS_VIEW_PAN;
    _currentTool   = new PanTool(this);

    // this should work for MDI frames as well as for normal ones
    SetSizeHints(100, 100);

    // Give it an icon
    SetIcon(wxIcon(home_xpm));

    _glw = new GLWindow(this, "GLWindow", _sg);
    _glw->SetCursor(wxCursor(wxCURSOR_ARROW));

    // Give it scrollbars  // Doesn't work. TAKA
    // canvas->SetScrollbars(20, 20, 50, 50);

    SetThemeEnabled(true);
    Show(TRUE);
  
  //Message Out
  ClearMessage (0);
  ClearMessage (1);
  ClearMessage (2);
  Message.precision(15);
   }

void BaseWindow::initMenus() {
      _mainMenubar = new wxMenuBar;
      _file_menu = new wxMenu;
      _help_menu = new wxMenu;
      _plugins_menu = new wxMenu;

      //append the MDI parents' menus and tools
      _mainGUI->createStdMenus(_file_menu, _plugins_menu, _help_menu, 0, accelEntries);

      //update the menu
      _mainMenubar->Append(_plugins_menu, "&Plugins");
      _mainMenubar->Append(_file_menu, "&File");
      addBasicMenus();
      addCustomMenus();
      _mainMenubar->Append(_help_menu, "&Help");
      SetMenuBar(_mainMenubar);

      //Create a custom popup menu for the future
      _popupMenu=new wxMenu();
      _popupMenu->Append(VIS_LOAD_FILE, "&Load");

      //create a toolbar
      _toolbar = 0;

#ifdef _WINDOWS
      _toolbar = CreateToolBar((long int)(wxTB_FLAT | wxTB_VERTICAL), -1, wxString("UNKNOWN"));
      assert(_toolbar!=0);
      addBasicToolstoToolbar();
      addCustomToolstoToolbar();
      _toolbar->Realize();
#endif

      //set the acceleration table
      //
      wxAcceleratorEntry *entries = new wxAcceleratorEntry[accelEntries.size()];
      for (int i=0;i<accelEntries.size();i++)
          entries[i].Set(accelEntries[i].GetFlags(), accelEntries[i].GetKeyCode(), accelEntries[i].GetCommand());

      wxAcceleratorTable t(accelEntries.size(), entries);
      SetAcceleratorTable(t);
 }

BaseWindow::~BaseWindow()
  {
    delete _sg;
    delete _popupMenu;
   }

void BaseWindow::load(const vcl_string& filename)
  {
    vcl_cout<<" BaseWindow:: Load:"<<filename<<vcl_endl;
   }

void BaseWindow::load(){}

void BaseWindow::addBasicMenus() 
  {
    _file_menu->Insert(1, VIS_FILE_CLOSE_CHILD, 
            "&Close", "Close this window");

    _edit_menu = new wxMenu;

    _edit_menu->Append(VIS_EDIT_UNDO, "&Undo\tCtrl-z");
    _edit_menu->Append(VIS_EDIT_REDO, "&Redo\tCtrl-r");
    _edit_menu->AppendSeparator();
    _edit_menu->Append(VIS_EDIT_DIRECT_SELECTION, "Direct Selection");
    _edit_menu->Append(VIS_EDIT_GROUP_SELECTION, "Group Selection");

    _view_menu = new wxMenu;
   
    wxMenu  *_zoom_menu = new wxMenu;
    
    _zoom_menu->Append(VIS_VIEW_ZOOM, "Arbitrary &Zoom");
    _zoom_menu->Append(VIS_VIEW_ZOOM_IN, "&Zoom In (+)\tCtrl-P");
    _zoom_menu->Append(VIS_VIEW_ZOOM_OUT, "&Zoom Out (-)\tCtrl-O");
    wxMenu  *_translate_menu = new wxMenu;
    _translate_menu->Append(VIS_VIEW_RIGHT,  "Translate Right \tRight Arrow");
    _translate_menu->Append(VIS_VIEW_LEFT, "Translate Left \tLeft Arrow");
    _translate_menu->Append(VIS_VIEW_DOWN,    "Translate Down \tDown Arrow");
    _translate_menu->Append(VIS_VIEW_UP,  "Translate Up \tUp Arrow");
    wxMenu  *_rotate_menu = new wxMenu;
    _rotate_menu->Append(VIS_VIEW_ROTATE, "Arbitrary &Rotate\t");
    _rotate_menu->Append(VIS_VIEW_ROTATE_LEFT, "&Rotate Left\tCtrl+Left Arrow");
    _rotate_menu->Append(VIS_VIEW_ROTATE_RIGHT, "&Rotate Right\tCtrl+Right Arrow");

    _view_menu->Append(VIS_VIEW_ZOOM_SUBMENU,"&Zoom", _zoom_menu, "Zoom");
    _view_menu->Append(VIS_VIEW_ZOOM_SUBMENU, "&Rotate", _rotate_menu, "Rotate");
    _view_menu->AppendSeparator();
    _view_menu->Append(VIS_VIEW_FLIP, "&Flip y-axis\tCtrl-F", "", true);
    _view_menu->Append(VIS_VIEW_TRANSLATE_SUBMENU, "&Translate", _translate_menu, "Translate");

    _mainMenubar->Append(_view_menu, "&View");
    _mainMenubar->Append(_edit_menu, "&Edit");

    //wxMenu *child_menu = new wxMenu;
    //child_menu->Append(VIS_CHILD_REFRESH, "&Refresh picture");
    //child_menu->Append(VIS_CHILD_CHANGE_TITLE, "Change &title...\t");
    //menu_bar->Append(child_menu, "&Child");

    _help_menu->Append(VIS_TOGGLE_DEBUG_INFO_STATUS, "Toggle Debug &Info","",true);    
    _help_menu->Append(VIS_TOGGLE_AUTO_HIGHLIGHT   , "Toggle Auto &Highlight","",true);    
    _help_menu->Append(VIS_GET_INFO   , "Get Debug Information","",true);    

  ///////////// View Accelerators ///////////////
  accelEntries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, (int) '=',      VIS_VIEW_ZOOM_IN));
  accelEntries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, (int) '-',      VIS_VIEW_ZOOM_OUT));
  accelEntries.push_back(wxAcceleratorEntry(wxACCEL_CTRL,      WXK_LEFT,    VIS_VIEW_ROTATE_LEFT));
  accelEntries.push_back(wxAcceleratorEntry(wxACCEL_CTRL,      WXK_RIGHT,   VIS_VIEW_ROTATE_RIGHT));
  accelEntries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL,    WXK_LEFT,    VIS_VIEW_LEFT));
  accelEntries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL,    WXK_RIGHT,   VIS_VIEW_RIGHT));
  accelEntries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL,    WXK_UP,      VIS_VIEW_UP));
  accelEntries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL,    WXK_DOWN,    VIS_VIEW_DOWN));
  accelEntries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, (int) 'G', VIS_GET_INFO));
  accelEntries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, (int) 'g', VIS_GET_INFO));
  accelEntries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, (int) 'M', VIS_VIEW_PAN));
  accelEntries.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, (int) 'm', VIS_VIEW_PAN));
}

void BaseWindow::addBasicToolstoToolbar()
  {
    assert(_toolbar!=0);
    _toolbar->SetToolSeparation(5);
    _toolbar->SetToolPacking(5);

    _toolbar->AddTool( VIS_EDIT_DIRECT_SELECTION, wxBitmap(pointer_xpm),
            wxBitmap(pointer_xpm), true, 0, "Select");
    _toolbar->AddTool( VIS_GET_INFO, wxBitmap(helpcs_xpm),
            wxBitmap(helpcs_xpm), true, 0, "Get Info");
    _toolbar->AddSeparator();
    //_toolbar->AddTool(VIS_EDIT_UNDO, wxBitmap(tb_undo_xpm),"Undo");
    //_toolbar->AddTool(VIS_EDIT_REDO, wxBitmap(tb_redo_xpm),"Redo");
    //_toolbar->AddSeparator();
    _toolbar->AddTool(VIS_VIEW_ZOOM_IN, wxBitmap(zoomin_xpm),"Zoom In");
    _toolbar->AddTool(VIS_VIEW_ZOOM_OUT, wxBitmap(zoomout_xpm),"Zoom Out");
    //_toolbar->AddSeparator();
    _toolbar->AddTool(VIS_VIEW_PAN, wxBitmap(pan_xpm),
            wxBitmap(pan_xpm), true, 0, "Pan");
    _toolbar->AddSeparator();  

   }

void BaseWindow::addCustomMenus() {}
void BaseWindow::addCustomToolstoToolbar() 
  {
    assert(_toolbar!=0);
   }


int BaseWindow::processCommandLine(int argc, const char* const* argv)
  {
    vcl_cout<<" ERROR: The plugin you are using does not override the \"processCommandLine(int const char* const*)\" function !"<<vcl_endl;
    return 0;
   }

void BaseWindow::CompletelyCreated() {}

//###########################################################
//             TOOL SELECTION HANDLERS
//###########################################################

void BaseWindow::OnToolSelectionEvent(wxCommandEvent &evt) 
  {  
    if (_currentToolId != evt.GetId()){
          if (_currentTool){
                delete _currentTool;
                if (_toolbar)
                    _toolbar->ToggleTool(_currentToolId, false); //turn off the other one
           }
          _currentTool = getToolFromID(evt.GetId());
          _currentToolId = evt.GetId();
     }
    else {
          if (_currentTool){
                delete _currentTool;
                if (_toolbar)
                    _toolbar->ToggleTool(_currentToolId, false);
           }
          _currentTool = 0;
          _currentToolId = 0;
     }
   }

GTool* BaseWindow::getToolFromID(int id) 
  {
    GTool *tool=0;

    switch(id) {
          case VIS_GET_INFO:
              tool = new GetInfoTool(this); 
              break;
          case VIS_VIEW_PAN:
              tool = new PanTool(this);
              //_glw->SetCursor(wxCursor(wxCURSOR_MAGNIFIER));
              break;
          default: break;
     }

    return tool;
   }

//###########################################################
//                     MENU HANDLERS
//###########################################################

void BaseWindow::OnCustomMenu(wxCommandEvent& event) {
      menuItemSelected(event);
 }

void BaseWindow::menuItemSelected(wxCommandEvent &evt) {
      //cerr << "BaseWindow::menuItemSelected " << evt.GetId() << vcl_endl;
 }

void BaseWindow::OnCloseChild(wxCommandEvent& WXUNUSED(event))
  {
    Close(TRUE);
   }

// this doesn't really work, so override it and make it private... use Refresh() instead
void BaseWindow::Refresh(bool eraseBackground, const wxRect *rect) 
  {
    wxMDIChildFrame::Refresh(eraseBackground, rect);
   }

void BaseWindow::Refresh() 
  {
    if(_glw)
        _glw->Refresh();
   }

void BaseWindow::RefreshNow() 
  {
    if (_glw)
        _glw->RefreshNow();
   }


void BaseWindow::OnRefresh(wxCommandEvent& WXUNUSED(event))
  {
    Refresh();
   }

void BaseWindow::OnZoom(wxCommandEvent& WXUNUSED(event))
  {
    wxString res = wxGetTextFromUser
            ( "Enter a ratio:", "Zoom the image.",
              "1");
    if(res.length()==0)
        return;

    float ratio;
    vcl_stringstream strm(res.c_str());
    if((strm >> ratio) && ratio != (float)0){
          zoom(ratio);

          vcl_ostringstream vcl_ostrm;
          vcl_ostrm << "Scale Factor: " << view()->getScaleFactor();
          GDebugDisplayer.status(vcl_ostrm.str());
     }
   }

void BaseWindow::OnZoomIn(wxCommandEvent& WXUNUSED(event))
  {
    zoom((float)1.1);

    vcl_ostringstream vcl_ostrm;
    vcl_ostrm << "Scale Factor: " << view()->getScaleFactor();
    GDebugDisplayer.status(vcl_ostrm.str());
   }

void BaseWindow::OnZoomOut(wxCommandEvent& WXUNUSED(event))
  {
    zoom(1/(float)1.1);

    vcl_ostringstream vcl_ostrm;
    vcl_ostrm << "Scale Factor: " << view()->getScaleFactor();
    GDebugDisplayer.status(vcl_ostrm.str());
   }

void BaseWindow::OnRotate(wxCommandEvent& WXUNUSED(event))
  {
    long res = wxGetNumberFromUser
            ( "Rotate the image.",
              "Enter a degree:", "Rotation",
              0, -360, 360, this );
    if(res == -1)
        return;

    wxSize size = GetClientSize();
    Point2D<double> point(size.x/2,size.y/2);
    view()->rotate(point,(float)res*2*3.1415/360);

    Refresh();
   }

#define ROTATE_STEP 10

void BaseWindow::OnRotateLeft(wxCommandEvent& WXUNUSED(event))
  {
    wxSize size = GetClientSize();
    Point2D<double> point(size.x/2,size.y/2);
    view()->rotate(point,(float)ROTATE_STEP*2*3.1415/360);

    Refresh();
   }

void BaseWindow::OnRotateRight(wxCommandEvent& WXUNUSED(event))
  {
    wxSize size = GetClientSize();
    Point2D<double> point(size.x/2,size.y/2);
    view()->rotate(point,(float)-ROTATE_STEP*2*3.1415/360);

    Refresh();
   }

void BaseWindow::OnFlip(wxCommandEvent& event) {
      bFlipView = !bFlipView;
      _view_menu->Check (VIS_VIEW_FLIP, bFlipView);
      view()->flip_y_axis();
      Refresh();
 }

void BaseWindow::OnTranslate (wxCommandEvent& WXUNUSED(event), double xDisp, double yDisp)
  {
    Point2D<double> screendelta (xDisp, yDisp);
    view()->translate (screendelta);

    Refresh();
   }

#define TRANSLATE_STEP 25

//View Left == Translate Right
void BaseWindow::OnViewLeft(wxCommandEvent& event)
  {
    OnTranslate (event, -TRANSLATE_STEP, 0);
   }
void BaseWindow::OnViewRight(wxCommandEvent& event)
  {
    OnTranslate (event, TRANSLATE_STEP, 0);
   }
void BaseWindow::OnViewUp(wxCommandEvent& event)
  {
    OnTranslate (event, 0, -TRANSLATE_STEP);
   }

void BaseWindow::OnViewDown(wxCommandEvent& event)
  {
    OnTranslate (event, 0, TRANSLATE_STEP);
   }

void BaseWindow::OnChangeTitle(wxCommandEvent& WXUNUSED(event))
  {
    static wxString s_title = _T("Canvas Frame");

    wxString title = wxGetTextFromUser(_T("Enter the new title for MDI child"),
            _T("MDI sample question"),
            s_title,
            GetParent()->GetParent());
    if ( !title )
        return;

    s_title = title;
    SetTitle(s_title);
   }

void BaseWindow::OnActivate(wxActivateEvent& event)
  {
    if ( event.GetActive() && _glw )
        _glw->SetFocus();
   }

void BaseWindow::OnClose(wxCloseEvent& event)
  {
#if 0
    // TAKA if ( canvas && canvas->IsDirty() )
    if ( wxMessageBox("Really close?", "Please confirm",
                wxICON_QUESTION | wxYES_NO) != wxYES ) {
          event.Veto();  
          return;
     }
#endif

    event.Skip();
   }

void BaseWindow::OntoggleDebugInfoStatus()
  {
    assert(_help_menu!=0);
    if (_debugInfoStatus==VIS_HIDE_DEBUG_INFO)
      {
        _debugInfoStatus = VIS_SHOW_DEBUG_INFO;
        _help_menu->Check (VIS_TOGGLE_DEBUG_INFO_STATUS, true);
       }
    else
      {
        _debugInfoStatus = VIS_HIDE_DEBUG_INFO;
        _help_menu->Check (VIS_TOGGLE_DEBUG_INFO_STATUS, false);
       }

    return;
   }


void BaseWindow::OntoggleAutoHighlight()
  {
    assert(_help_menu!=0);
    if (_autoHighlight)
      {
        _autoHighlight = false;
        _help_menu->Check (VIS_TOGGLE_AUTO_HIGHLIGHT, false);

        // Set the color back to normal for the previously selected objects
        if(_prevHits.size()>0)
          {
            vcl_vector<GraphicsNode*>::iterator it;
            it = _prevHits.begin();
            for (; it != _prevHits.end(); ++it) 
              {
                GraphicsNode *hitObj = *it;
                hitObj->pop();
               }
            _prevHits.clear();
            Refresh();
           }
       }
    else
      {
        _autoHighlight = true;
        _help_menu->Check (VIS_TOGGLE_AUTO_HIGHLIGHT, true);
       }
    return;
   }

void BaseWindow::OnUndo(wxCommandEvent &evt) { undo(); }
void BaseWindow::OnRedo(wxCommandEvent &evt) { redo(); }

//###########################################################
//           MOUSE AND KEYBOARD EVENT HANDLERS
//###########################################################

void BaseWindow::leftMouseDown(wxMouseEvent &evt) 
  {
    if (_currentTool)
        _currentTool->leftMouseDown(evt);
    else
        evt.Skip();
   }

void BaseWindow::leftMouseUp(wxMouseEvent &evt) 
  {
    if (_currentTool)
        _currentTool->leftMouseUp(evt);
    else
        evt.Skip();
   }

void BaseWindow::middleMouseDown(wxMouseEvent &evt) 
  {
    if (_currentTool)
        _currentTool->middleMouseDown(evt);
    else
        evt.Skip();
   }

void BaseWindow::middleMouseUp(wxMouseEvent &evt) 
  { 
    if (_currentTool)
        _currentTool->middleMouseUp(evt);
    else
        evt.Skip();
   }

void BaseWindow::rightMouseDown(wxMouseEvent &evt) 
  { 
    if (_currentTool)
        _currentTool->rightMouseDown(evt);
    else
        evt.Skip();
   }

void BaseWindow::rightMouseUp(wxMouseEvent &evt) 
  {
    GTool::Result res = GTool::UNHANDLED;
    if (_currentTool)
        res = _currentTool->rightMouseUp(evt);

    if (res == GTool::UNHANDLED)
        PopupMenu(_popupMenu, (int)evt.GetX(), (int)evt.GetY());
   }

void BaseWindow::mouseMove(wxMouseEvent &evt) 
  { 
    if (_currentTool)
        _currentTool->mouseMove(evt);
    else
        evt.Skip();
   }

//
//Skip the event if you do not handle it.
//
void BaseWindow::keyDown(wxKeyEvent &evt) 
  { 
   if (_currentTool)
       _currentTool->keyDown(evt); 
   else
        evt.Skip();
   }

void BaseWindow::keyUp(wxKeyEvent &evt) 
  { 
    if (_currentTool)
        _currentTool->keyUp(evt);
    else
        evt.Skip();
   }

void BaseWindow::charUp(wxKeyEvent &evt) 
  {
    if (_currentTool)
        _currentTool->charUp(evt); 
    else
        evt.Skip();
   }


//###########################################################
//                HIT TEST HANDLERS
//###########################################################

int BaseWindow::_hitTest (vcl_vector<HitRecord>& hits_, Point2D<double>& pos_) 
  {
    return  view()->hitTest(hits_,pos_,5);
   }

int BaseWindow::_hitTest(vcl_vector<HitRecord> &hits_,
      const Point2D<double> topLeft, const Point2D<double>bottomRight)
{
  return view()->hitTest(hits_, topLeft, bottomRight);
}

void BaseWindow::HighlightSelected(wxMouseEvent& event) 
  {
    Point2D<double> mouse_position(event.GetX(),event.GetY());
    vcl_vector<HitRecord> hits;
    bool dirty=false;

    _hitTest(hits,mouse_position);

    // Set the color back to normal for the previously selected objects
    if(_prevHits.size()>0)
      {
        vcl_vector<GraphicsNode*>::iterator it;
        it = _prevHits.begin();
        for (; it != _prevHits.end(); ++it) 
          {
            GraphicsNode *hitObj = *it;
            hitObj->pop();
           }
        _prevHits.clear();
        dirty=true;
       }

    // Set the color to RED for the newly selected objects
    if(hits.size()>0)
      {
        vcl_vector<HitRecord>::iterator it;
        it = hits.begin();
        for (; it != hits.end(); ++it) 
          {
            GraphicsNode *hitObj = it->hit_object;
            hitObj->pushStyle(Color(RED));
            _prevHits.push_back(hitObj);
           }
        dirty=true;
       }

    if(dirty)
      {
        Refresh();
       }
   }

void BaseWindow::displayDebugInfo(wxMouseEvent& event)
  {
    vcl_ostringstream vcl_ostrm;

    Point2D<double> mouse_position(event.GetX(),event.GetY());

    vcl_vector<HitRecord> hits;
    if(_hitTest(hits,mouse_position)==0)
        return;

    Point2D<double> pos = view()->unproject(Point2D<double>(event.GetX(), event.GetY()));
    vcl_ostrm << " Position = "<<pos<<" \n\n";
    for (vcl_vector<HitRecord>::iterator it = hits.begin(); it != hits.end(); ++it) 
      {
        GraphicsNode *hitObj=it->hit_object;
        vcl_string information;

        if (hitObj)
          {
            hitObj->getInformation(information,pos);
            hitObj->getInformation(information);
            if (information.length()>0)
              {
                vcl_ostrm<<information;
               }
           }
       }

    if (vcl_ostrm.str().length()>0) 
      {    
        GDebugDisplayer.clear();
        GDebugDisplayer.disp(vcl_ostrm.str());
       }
    return;
   }
void BaseWindow::displayCurrentInfo (wxMouseEvent &event)
  {
    vcl_ostringstream vcl_ostrm;
    vcl_ostrm.precision (14);
    Point2D<double> mouse_pos (event.GetX(),event.GetY());
    
    vcl_ostrm << "Scale Factor: " << view()->getScaleFactor();
    vcl_ostrm << ", Mouse Position: (" 
            << mouse_pos.getX() << "," << mouse_pos.getY() << ")";
    Point2D<double> actual_pos =   view()->unproject (mouse_pos);
    vcl_ostrm << ", World Position: (" 
            << actual_pos.getX() << "," << actual_pos.getY() << ")";
    GDebugDisplayer.status(vcl_ostrm.str());
   }

void BaseWindow::undo() {
      //if(!_in_drawing_mode || _undo_commands.empty()) return;

      //GfxCommand *cmd = _undo_commands.back();
      //cmd->unexecute();
      //_undo_commands.pop_back();
      //_redo_commands.push_front(cmd);
      //Refresh();
 }

void BaseWindow::redo() {
      //if(!_in_drawing_mode || _redo_commands.empty()) return;

      //GfxCommand *cmd = _redo_commands.front();
      //cmd->execute();
      //_redo_commands.pop_front();
      //_undo_commands.push_back(cmd);
      //Refresh();
 }

void BaseWindow::clear_undo() {
      //_redo_commands.clear();
      //_undo_commands.clear();

      //command->execute();
      //  _redo_commands.clear();
      //  if(command->can_undo())
      //    _undo_commands.push_back(res.command);

 }

void BaseWindow::zoom(float ratio)
  {
    wxSize size = GetClientSize();
    Point2D<double> point(size.x/2,size.y/2);
    view()->zoom(point,(float)ratio);
    _glw->Refresh();
   }

/*
        void BaseWindow::pushTool(int id) {
        for(tool_stack_t::reverse_iterator i = _tool_stack.rbegin(); i != _tool_stack.rend(); ++i) {
        if(i->second)
        i->second->abort();
        }

        _tool_stack.push_back(std::make_pair(id, (DrawingTool*)0));
        }

        void BaseWindow::popTool() {
        assert(!_tool_stack.empty());

        _tool_stack.back().second->abort();
        delete _tool_stack.back().second;

        _tool_stack.pop_back();
        }
        */




void BaseWindow::setTitle(vcl_string title)
  {
    SetTitle(wxString(title.c_str()));
   }

vcl_string BaseWindow::getTitle(void)
  {
    return vcl_string(GetTitle());
   }

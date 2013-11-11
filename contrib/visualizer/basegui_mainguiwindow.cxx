//! Our MDI parent class
/*!
  This class provides the basic functionalities of creating a child-frame,
  loading a file, supplying a help window, debug-window, status-bar, and such.
*/
#include "basegui_mainguiwindow.h"
#include "basegui_msgout.h"
#include "basegui_messageprinter.h"
#include <wx/image.h>

#include <vcl_cstdlib.h>
#include "commandoptions.h"
int       MessagePrinter::_state=1;
int       MessagePrinter::_priorityThreshold=1000;
vcl_string    MessagePrinter::_outputFileName;
vcl_ofstream  MessagePrinter::_fp;
int       MessagePrinter::_count=0;

MESSAGE_OPTION MessageOption = MSG_TERSE;

IMPLEMENT_APP(VisApp)

//! Event table for the parent MDI window.
/*!
  Note that these functions are not treated as virtual methods.  The
  polymorphism must be ahieved by creating another EVENT_TABLE in your derived
  class.
*/
BEGIN_EVENT_TABLE(MainGUIWindow,  wxMDIParentFrame)
    EVT_MENU(VIS_HELP_SHOWHIDE_DEBUG,  MainGUIWindow::OnShowHideDebug)
    EVT_MENU(VIS_HELP_DISPLAY_OPTIONS,  MainGUIWindow::OnDisplayOptions)
    EVT_MENU(VIS_HELP_ABOUT,       MainGUIWindow::OnAbout)
    EVT_MENU(VIS_FILE_LOAD_FILE,   MainGUIWindow::OnLoadFile)
    EVT_MENU_RANGE(VIS_PLUGINS_ID_BASE,
       VIS_PLUGINS_ID_BASE+10, MainGUIWindow::getPlugin)
    EVT_MENU(VIS_FILE_QUIT,        MainGUIWindow::OnQuit)
    EVT_SASH_DRAGGED_RANGE(-1, -1, MainGUIWindow::OnSashDrag)

    EVT_CLOSE(MainGUIWindow::OnClose)

    EVT_SIZE(MainGUIWindow::OnSize)
END_EVENT_TABLE()

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// VisApp
// ---------------------------------------------------------------------------

void _loadInitFile(vcl_string filename);

// Initialise this in OnInit, not statically
bool VisApp::OnInit()
  {
    ::wxInitAllImageHandlers();
    vcl_string init_file_name("");
    vcl_string default_plugin("");
    bool list_plugins(false);
    bool print_compile_time(false);
  
    
    CommandOptions command_options;

    //
    //This is needed due to some weird restrictions on 'const' qualifications.
    //Look up cv-quals in the g++ manual.
    //
    const char *const *argv_copy = (const char *const *)(argv);
    command_options.register_flag  (print_compile_time, "compile-time",   '\0',  "Date and time this program was compiled");
    command_options.register_flag  (list_plugins,       "list-plugins",   '\0',  "List the Plugins Available");
    command_options.register_option(default_plugin,     "plugin",         '\0',  "Name of Plugin To Load", "Plugin-Name");
    
    _loadInitFile(init_file_name);

    int winSize[2]={0,0};
    if(GOptions["Base"].count("WindowSizeX")>0)
        winSize[0] = atoi(GOptions["Base"]["WindowSizeX"].c_str());
    if(winSize[0]<=0)
        winSize[0] = 1024;
    if(GOptions["Base"].count("WindowSizeY")>0)
        winSize[1] = atoi(GOptions["Base"]["WindowSizeY"].c_str());
    if(winSize[1]<=0)
        winSize[1] = 742;

    // Create the main frame window

    MainGUIWindow *GFrame = new MainGUIWindow ((wxFrame *)NULL, -1, "Visualizer", wxPoint(-1, -1), wxSize(winSize[0], winSize[1]),
            wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);

    SetTopWindow(GFrame);
   
    try
      {
        command_options.process_command_line(argc, argv_copy);
       }
    catch (commandoptions_error &ex) 
      {
        if (default_plugin.size()==0)
          {
            default_plugin = GFrame->getUniquePluginName();
           }

        if (default_plugin.size()==0)
            vcl_cerr << "Error: " << ex.what() << vcl_endl;

        //return 1;
       }

    if (default_plugin.size()==0)
      {
        default_plugin = GFrame->getUniquePluginName();
       }

    if (list_plugins)
      {
        GFrame->printPluginList();
        exit(0);
       }

    if (print_compile_time)
      {
        vcl_cout<<" Time Of Compilation = "<<vcl_string(__TIME__)<<vcl_endl;
        vcl_cout<<" Date Of Compilation = "<<vcl_string(__DATE__)<<vcl_endl;
        exit(0);
       }
    
    
    
   
    BaseWindow *win=0;
    if (default_plugin.size()>0)
      {
        win = GFrame->getPluginFromName(default_plugin);
        if (win)
            win->processCommandLine(argc, argv_copy);
       }
    else
      {
        if(GOptions["Base"].count("DefaultPlugin")>0)
           win=GFrame->getPluginFromName(GOptions["Base"]["DefaultPlugin"]);
       }
   
    GFrame->Show();
    //
    //Hack Hack!! The following two calls
    //are needed for the screen to be refreshed.
    //They do not "do" anything.
    //
    GFrame->OnShowHideDebug();
    GFrame->OnShowHideDebug();
    
    return TRUE;
   }


void MainGUIWindow::printPluginList()
  {
    vcl_vector<BasePlugin *> plugin_list = PluginManager::GetPluginList();

    vcl_vector<vcl_string> temp;
    for (unsigned int i=0;i<plugin_list.size();i++)
      {
        vcl_cout<<vcl_endl;
        vcl_cout<<" Plugin-Name      : "<<plugin_list[i]->GetPluginName()<<vcl_endl;
        vcl_cout<<" Plugin-Menu-Name : "<<plugin_list[i]->GetPluginMenuName()<<vcl_endl;
        vcl_cout<<" Services         : ";
        temp = plugin_list[i]->GetPluginServices();
        for (unsigned int j=0;j<temp.size();j++)
          {
            vcl_cout<<" "<<temp[j];
            if ((j+1)<(temp.size()))
                vcl_cout<<", ";
            else
                vcl_cout<<"  ";
           }
        vcl_cout<<vcl_endl;
        vcl_cout<<" Description      : "<<plugin_list[i]->GetPluginDescription()<<vcl_endl;
        vcl_cout<<vcl_endl;
       }
    vcl_cout<<" Total-Plugins        : "<<plugin_list.size()<<vcl_endl;
   }


void _loadInitFile(vcl_string filename)
{
  vcl_string defaultFilename("./visualizer.ini");
  vcl_ifstream initFile;
  if(filename.size()==0){
    filename = defaultFilename;
  }
  initFile.open(filename.c_str(),vcl_ios::in);
  if(initFile.fail()){
    // Suppress the error message for now since most of people
    // dont have an init-file.
#if 0
    vcl_cerr<<"[Error]["<<__FILE__<<":"<<__LINE__<<"] "
  <<" Failed to open the init file." << vcl_endl;
#endif
    return;
  }

  while(!initFile.eof()){
    vcl_string buf;
    vcl_getline(initFile,buf);
    // Skip empty or comment lines.
    if(buf.length()>=2 && buf[0]!='#'){
      vcl_string className,attrName,value;
      SStream2 strm(buf,".=");
      if(strm >> className >> Check(".") >> attrName >> Check("=")
   >> value) {
  // Parsing success.
  GOptions.setValue(className,attrName,value);
      }
    }
  }
}

void MainGUIWindow::createStdMenus(wxMenu *file, wxMenu *plugins, wxMenu *help,
        wxToolBar *toolBar, vcl_vector<wxAcceleratorEntry> &accelerators) {
  InitMenus(file, plugins, help);
  if(toolBar)
    InitToolBar(toolBar);
  InitAccelerators(accelerators);
}

void MainGUIWindow::InitMenus(wxMenu *file_menu, wxMenu *plugins_menu, wxMenu *help_menu) {
  file_menu->Append(VIS_FILE_LOAD_FILE, "&Load file\tCtrl-L", 
      "Load a file into a new child window");
  file_menu->Append(VIS_FILE_QUIT, "E&xit\tCtrl-X", "Quit the program");

  //
  //Add Plugins.
  //
  vcl_vector<BasePlugin *> plugins = PluginManager::GetPluginList();
  for (unsigned int i = 0;i<plugins.size();i++)
  {
    assert(plugins[i]->GetPluginID() < 50); // this will mess up our ID scheme
    plugins_menu->Append(VIS_PLUGINS_ID_BASE + plugins[i]->GetPluginID(), plugins[i]->GetPluginMenuName().c_str()); 
  }

  help_menu->Append(VIS_HELP_SHOWHIDE_DEBUG, "&Show/Hide Debug Window\tAlt-D");
  help_menu->Append(VIS_HELP_DISPLAY_OPTIONS, "&Display Options (Debug2)");
  help_menu->Append(VIS_HELP_ABOUT, "&About\tAlt-A");
}

void MainGUIWindow::InitToolBar(wxToolBar* toolBar)
{
   // xpms available: 
   //new_xpm, open_xpm, save_xpm, copy_xpm, cut_xpm, paste_xpm, print_xpm, help_xpm

   toolBar->SetToolSeparation(5);
   toolBar->SetToolPacking(5);
   
    toolBar->AddTool( VIS_FILE_LOAD_FILE, wxBitmap(open_xpm), "Open file");
    //toolBar->AddTool( VIS_HELP_ABOUT, wxBitmap(help_xpm), "Help");

}

void MainGUIWindow::InitAccelerators(vcl_vector<wxAcceleratorEntry> &entries)
{
  entries.push_back(wxAcceleratorEntry(wxACCEL_ALT, (int) 'L', VIS_FILE_LOAD_FILE));
  entries.push_back(wxAcceleratorEntry(wxACCEL_ALT, (int) 'X', VIS_FILE_QUIT));
    entries.push_back(wxAcceleratorEntry(wxACCEL_ALT, (int) 'D', VIS_HELP_SHOWHIDE_DEBUG));
  entries.push_back(wxAcceleratorEntry(wxACCEL_ALT, (int) 'A', VIS_HELP_ABOUT));
}

// ---------------------------------------------------------------------------
// MainGUIWindow
// ---------------------------------------------------------------------------

// Define my frame constructor
MainGUIWindow::MainGUIWindow(wxWindow *parent,
                 const wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 const long style)
       : wxMDIParentFrame(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
  _sash = new wxSashLayoutWindow(this, -1,
          wxDefaultPosition, wxSize(400, 30),
          wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
  _sash->SetDefaultSize(wxSize(400, 1000));
  _sash->SetOrientation(wxLAYOUT_VERTICAL);
  _sash->SetAlignment(wxLAYOUT_RIGHT);
  _sash->SetBackgroundColour(wxColour(255, 255, 255));
  _sash->SetSashVisible(wxSASH_LEFT, TRUE);
  _sash->SetExtraBorderSize(1);

  _notebook =  new wxNotebook(_sash, -1);
  for(int cnt=0;cnt<_NumDebugWins;cnt++){
    _debugTexts[cnt] = new wxTextCtrl
      (_notebook, -1, _T("Debug\nWindow"),
       wxDefaultPosition, wxDefaultSize, 
       wxTE_MULTILINE | wxTE_READONLY);
    _debugTexts[cnt]->SetMaxLength (1E10);
    vcl_string name("Debug 0");
    name.at(6) = name.at(6) + cnt;
    _notebook->AddPage(_debugTexts[cnt], _T(name.c_str()));
  }
  GDebugDisplayer.debugTexts(_debugTexts,_NumDebugWins);
  GDebugDisplayer.notebook(_notebook);
  _notebook->Show(TRUE);

  mainMenuBar = new wxMenuBar;
  fileMenu    = new wxMenu;
  helpMenu    = new wxMenu;
  pluginsMenu = new wxMenu;

  mainToolbar = CreateToolBar(wxTB_FLAT | wxTB_HORIZONTAL, GetId());
  createStdMenus(fileMenu, pluginsMenu, helpMenu, mainToolbar, _accEntries);
  mainToolbar->Realize();
  SetToolBar(mainToolbar);

  mainMenuBar->Append(fileMenu, "&File");
  mainMenuBar->Append(pluginsMenu, "&Plugins");
  mainMenuBar->Append(helpMenu, "&Help");

  wxAcceleratorTable t(_accEntries.size(), (&(_accEntries[0])));
    SetAcceleratorTable(t);
    SetMenuBar(mainMenuBar);

  GDebugDisplayer.statusBar(CreateStatusBar());
  
    
    // Give it an icon
  //SetIcon(wxIcon(logo_xpm));
    //
    //Hide Debug Window

  bool show=false;
  if(GOptions["Base"].count("ShowDebugWindow")>0)
    show=atoi(GOptions["Base"]["ShowDebugWindow"].c_str());
  _sash->Show(show);

  relayoutWindow();
    /**
     * Do not call this function here. 
     * It is called in OnInit()
     * The advantage is that some options to the visualizer
     * (like --compile-time, --help etc.) do not reqire the 
     * GUI and calling this function here, will result in
     * a window always popping up
     *
     * RD
     * */
    //
    //Show(TRUE);
    //
}

void MainGUIWindow::OnClose(wxCloseEvent& event)
{
#if 0
if ( event.CanVeto() && (gs_nFrames > 0) )
    {
        wxString msg;
        msg.Printf(_T("%d windows still open, close anyhow?"), gs_nFrames);
        if ( wxMessageBox(msg, "Please confirm",
                          wxICON_QUESTION | wxYES_NO) != wxYES )
        {
            event.Veto();

            return;
        }
    }
#endif

    event.Skip();
}

void MainGUIWindow::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void MainGUIWindow::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox("Visualizer 1.0 \n"
                       "Authors:  \n "
                       "    Brian Bloniarz \n"
                       "    Raghavan Dhandapani \n"
                       "    Takayasu Harada \n");
}

void MainGUIWindow::relayoutWindow() {
  wxLayoutAlgorithm layout;
  layout.LayoutMDIFrame(this);

  // Leaves bits of itself behind sometimes
  GetClientWindow()->Refresh(false);
}

void MainGUIWindow::OnShowHideDebug()
{
  _sash->Show(!_sash->IsShown());
  relayoutWindow();
}

void MainGUIWindow::OnSashDrag(wxSashEvent& event)
{
    if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
        return;
    _sash->SetDefaultSize(wxSize(event.GetDragRect().width, 1000));
    relayoutWindow();
}

void MainGUIWindow::OnDisplayOptions(wxCommandEvent& WXUNUSED(event) )
{
  vcl_ostringstream istrm;
  istrm << "[List of Options]" << vcl_endl;
  for(Options::iterator itr=GOptions.begin();
      itr != GOptions.end() ; itr++){
    istrm << vcl_endl << "Class=" << itr->first << vcl_endl;
    for(vcl_map<vcl_string,vcl_string>::iterator itr2=itr->second.begin();
  itr2 != itr->second.end() ; itr2++){
      istrm << "Attr=" << itr2->first << "," 
      << "Value=" << itr2->second << vcl_endl;
    }
  }
  // Display it on the debug-window #2 (for now).
  GDebugDisplayer.clear(2);
  GDebugDisplayer.disp(istrm.str(),2);
  Refresh();
}

void MainGUIWindow::OnSize(wxSizeEvent& WXUNUSED(event))
{
#if 0
    int w, h;
    GetClientSize(&w, &h);

    _sash->SetSize(0, 0, 200, h);
    GetClientWindow()->SetSize(200, 0, w - 200, h);
#else
  relayoutWindow();
#endif
}

void MainGUIWindow::_newWindow(BaseWindow *subframe)
{
  _subframes.push_back(subframe);
  subframe->initMenus();
}


void MainGUIWindow::OnLoadFile(wxCommandEvent& event)
{
  // TODO get from plugin list
  wxFileDialog dialog
    (
     this,
     _T("Open file"),
     _T(""),
     _T(""),
     //_T("Esf Files (*.esf)|*.esf|Shgm Files (*.shgm)|*.shgm")
     _T("All Files (*)|*")
     );
  if (dialog.ShowModal() != wxID_OK){
    return;
  }
  loadFile(vcl_string(dialog.GetPath().c_str()));
}

void MainGUIWindow::loadFile(vcl_string filename) 
  {
    if(filename.length()<=3)
      {
        return;
       }
    
    //
    //Find the plugin
    //from the filename.
    //
    
  int pos = filename.rfind('.');
  vcl_string extension = filename.substr(pos+1, filename.length());
  for (unsigned int i=0;i<extension.size();i++) 
    {
      extension.at(i)=toupper(extension.at(i));
     }
  
  vcl_cout<<" File Extension = "<<extension<<vcl_endl;
  BasePlugin *plugin = PluginManager::GetPluginFromService(extension);
  if (plugin) 
    {
      BaseWindow *win = plugin->GetPluginWindow(this);
      _newWindow(win);
      win->load(filename);
      
      return;
     }
  else
    {
      vcl_cout<<" Error: <MainGUIWindow::load> Unable to load Plugin with Service: "<<extension<<" {"<<__FILE__<<", "<<__LINE__<<"} "<<vcl_endl;
     }
  
   }

BaseWindow* MainGUIWindow::getPlugin(wxCommandEvent &event)
  {
    int id = event.GetId();
    id    -= VIS_PLUGINS_ID_BASE;  

    BasePlugin *plugin = PluginManager::GetPluginFromID(id);
    if (plugin) 
      {
        BaseWindow *win = plugin->GetPluginWindow(this);
        _newWindow(win);
        //win->load();
        return win;
       }
    else
      {
        vcl_cout<<" Error: <MainGUIWindow::getPlugin> Unable to load Plugin with ID: "<<id<<" {"<<__FILE__<<", "<<__LINE__<<"} "<<vcl_endl;
       }
    return 0;
   }

BaseWindow* MainGUIWindow::getPluginFromName(vcl_string name)
  {
    BasePlugin *plugin = PluginManager::GetPluginFromName(name);
    if (plugin) 
      {
        BaseWindow *win = plugin->GetPluginWindow(this);
        assert(win!=0);
        _newWindow(win);
        return win;
       }
    else
      {
        vcl_cout<<" Error: <MainGUIWindow::getPluginFromName> Unable to load Plugin with Name ["<<name<<"] {"<<__FILE__<<", "<<__LINE__<<"} "<<vcl_endl;
       }
    return 0;
   }

int MainGUIWindow::getPluginCount()
  {
    return PluginManager::GetPluginCount();
   }
/**
 * If there is only one plugin, get it's name else return "".
 * */
vcl_string MainGUIWindow::getUniquePluginName()
  {
    return PluginManager::GetUniquePluginName();
   }

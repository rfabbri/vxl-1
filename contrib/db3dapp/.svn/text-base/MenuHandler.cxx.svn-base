/*************************************************************************
 *    NAME: Alexander K. Bowman
 *    USER: akb
 *    FILE: MenuHandler.cxx
 *    DATE: 
 *************************************************************************/

#include <vgui/vgui.h>
#include <vcl_string.h>
#include <vgui/vgui_dialog.h>

#include "MenuHandler.h"
#include "SceneHandler.h"
#include "SceneEventHandler.h"

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/SbLinear.h>
/*************************************************************************
 * Function Name: MenuHandler::MenuHandler
 * Parameters: 
 * Effects: 
 *************************************************************************/

MenuHandler::MenuHandler()
{
    defaults();
    
}

/*************************************************************************
 * Function Name: MenuHandler::~MenuHandler
 * Parameters:
 * Effects:
 *************************************************************************/

MenuHandler::~MenuHandler()
{

}

/*************************************************************************
 * Function Name: MenuHandler::defaults
 * Parameters:
 * Effects: sets the program default values
 *************************************************************************/
void
MenuHandler::defaults()
{
  _isFileOpen = false;
  ext = "";
  _pointSize = 1.0f;
  _lineWidth = 1.0f;
  _nodeSize = 0.1f;

}

/*************************************************************************
 * Function Name: MenuHandler::
 * Parameters:  
 * Returns: 
 * Effects:
 *************************************************************************/

MenuHandler*
MenuHandler::instance_ = 0;


/*************************************************************************
 * Function Name: MenuHandler::instance
 * Parameters:
 * Effects:
 *************************************************************************/
MenuHandler*
MenuHandler::instance()
{
  if (!instance_)
  {
    instance_ = new MenuHandler();
  }
  return MenuHandler::instance_;
}


/*************************************************************************
 * Function Name: MenuHandler::fileNew
 * Parameters:
 * Effects:
 *************************************************************************/
void 
MenuHandler::fileNew()           
{
  vgui_dialog params("File->New...");  
  params.message("Not Implemented Yet, Fill in at MenuHandler.cpp");
  if (!params.ask())
    return;
}

/*************************************************************************
 * Function Name: MenuHandler::fileOpen
 * Parameters:
 * Effects:
 *************************************************************************/
void 
MenuHandler::fileOpen()           
{
  // if something already open, 
  if( _isFileOpen )
  {
    vgui_dialog error( "Error" );
    error.message( "There's already an open file!" );
    error.ask();
    return;
  }
  vgui_dialog params("Open");
  params.file( "Open...", ext, _fileToOpen);
  if (!params.ask())
    return;

  // check the filetype.
  vcl_string fileType;
  int a = _fileToOpen.find_last_of ('.');
  int b = _fileToOpen.find_last_of ('\0');
  fileType = _fileToOpen.substr (a, b);

  // if it's the right file type, load it
  if( strcmp( fileType.c_str(), ".IV") == 0 || strcmp( fileType.c_str(), ".iv") == 0 )
  {
    SceneHandler::instance()->openIV( _fileToOpen );
    _isFileOpen = true;
  }
  else if( strcmp( fileType.c_str(), ".FS") == 0 || strcmp( fileType.c_str(), ".fs") == 0 )
  {
    SceneHandler::instance()->openFS( _fileToOpen );
    _isFileOpen = true;
  }
  else if( strcmp( fileType.c_str(), ".P3D") == 0 || strcmp( fileType.c_str(), ".p3d") == 0 )
  {
    SceneHandler::instance()->openP3D( _fileToOpen );
    _isFileOpen = true;
  }
  else
  {
    vgui_dialog error( "Error" );
    error.message( (fileType + " files not supported.").c_str() );
    if (!error.ask())
      return;
    else
    {
      fileOpen();
      return;
    }
  }
  options_view_show_hide();
}

/*************************************************************************
 * Function Name: MenuHandler::fileSave
 * Parameters:
 * Effects:
 *************************************************************************/
void 
MenuHandler::fileSave()           
{
  vgui_dialog params("File->Save...");  
  
  vcl_string filename("");
  params.field( "Save as:", filename );
  int fileType;
  vcl_vector<vcl_string> choices;
  choices.push_back( "P3D" );
  choices.push_back( "IV" );
  params.choice("File type", choices, fileType );

  if( !params.ask() )
    return;
  
  if( fileType == 0 )
    SceneHandler::instance()->saveP3D( filename );
  else if( fileType == 1 )
    {
    bool saveNodes=false, saveLinks=false, saveFaces=true;
    vgui_dialog params2( "Save as IV" );
    params2.checkbox("Save Vertices", saveNodes );
    params2.checkbox("Save Links", saveLinks );
    params2.checkbox("Save Faces", saveFaces );
    if( !params2.ask() )
      return;
    SceneHandler::instance()->saveIV( filename, saveNodes, saveLinks, saveFaces );
    }

}

/*************************************************************************
 * Function Name: MenuHandler::fileQuit
 * Parameters:
 * Effects:
 *************************************************************************/
void 
MenuHandler::fileQuit()           
{
  vgui_dialog quit_dl("Quit");
  quit_dl.message("   Are you sure you want to quit 3D Shock Viewer??   ");
  if (quit_dl.ask())
    {
    SceneHandler::instance()->getRoot()->removeAllChildren();
    vgui::quit();
    }
}

/*************************************************************************
 * Function Name: MenuHandler::addNodeByPoint
 * Parameters:
 * Effects:
 *************************************************************************/

void 
MenuHandler::addNodeByPoint()           
{
  float x=0,y=0,z=0;
  vgui_dialog params( "Add New Node" );
  params.field("X",x);
  params.field("Y",y);
  params.field("Z",z);
  if (!params.ask())
    return;
  SceneHandler::instance()->addNode( SbVec3f(x,y,z), _nodeSize );
 
}

/*************************************************************************
 * Function Name: MenuHandler::addNodeByDragger
 * Parameters:
 * Effects:
 *************************************************************************/

void 
MenuHandler::addNodeByDragger()           
{
  SceneHandler::instance()->addNodeByDragger( _nodeSize );
 
}


/*************************************************************************
 * Function Name: MenuHandler::addLink
 * Parameters:
 * Effects:
 *************************************************************************/

void 
MenuHandler::addLink()           
{
  vgui_dialog params( "Add New Link" );
  if (!params.ask())
    return;
}


/*************************************************************************
 * Function Name: MenuHandler::addFace
 * Parameters:
 * Effects:
 *************************************************************************/

void 
MenuHandler::addFace()           
{
  vgui_dialog params( "Add New Face" );
  if (!params.ask())
    return;
}

/*************************************************************************
 * Function Name: MenuHandler::optionsDisplay
 * Parameters:
 * Effects:
 *************************************************************************/

void 
MenuHandler::optionsDisplay()           
{
  vgui_dialog params("Drawing Options");
  float nodeSize = _nodeSize ;
  params.field( "Node Size", nodeSize );
  
  float lineWidth = _lineWidth;
  params.field( "Line Width", lineWidth );

  float pointSize = _pointSize; 
  params.field( "Point Size", pointSize ); 

  if( !params.ask() )
    return;

  // if value is changed search for all the cubes
  if( nodeSize != _nodeSize )
  {
    _nodeSize = nodeSize; 
    SceneHandler::instance()->changeNodeSize( nodeSize );
  }
  if( lineWidth != _lineWidth ) 
  {
    _lineWidth = lineWidth; 
    SceneHandler::instance()->changeLineWidth( lineWidth );
  }
  if( pointSize != _pointSize ) 
  {
    _pointSize = pointSize; 
    SceneHandler::instance()->changePointSize( pointSize );
  }

}


void
MenuHandler::optionsColor()
{
  vgui_dialog params( "Color Options" );
  
  vcl_vector<vcl_string> choices;
  choices.push_back( "GRAY" );
  choices.push_back( "RED" );
  choices.push_back( "ORANGE" );
  choices.push_back( "YELLOW" );
  choices.push_back( "GREEN" );
  choices.push_back( "BLUE" );
  choices.push_back( "PURPLE" );
  choices.push_back( "WHITE" );

  int nodeColor = _nodeColor;
  int linkColor = _linkColor;
  int faceColor = _faceColor;
  int highlightColor = SceneEventHandler::instance()->getHighlightColor();

  params.choice( "Node color: ", choices, nodeColor );
  params.choice( "Link color: ", choices, linkColor );
  params.choice( "Face color: ", choices, faceColor );
  params.choice( "Highlight color: ", choices, highlightColor );
  if( !params.ask() )
    return;

  if( nodeColor != _nodeColor )
  {
    SceneHandler::instance()->changeNodeColor( nodeColor );
    _nodeColor = nodeColor;
  }
  if( linkColor != _linkColor )
  {
    SceneHandler::instance()->changeLinkColor( linkColor );
    _linkColor = linkColor;
  }
  if( faceColor != _faceColor )
  {
    SceneHandler::instance()->changeFaceColor( faceColor );
    _faceColor = faceColor;
  }
  if( faceColor != SceneEventHandler::instance()->getHighlightColor() )
  {
    SceneEventHandler::instance()->changeHighlightColor( highlightColor );
    _faceColor = faceColor;
  }


}

void MenuHandler::options_view_show_hide()
{
  // find all the switches
  SoSearchAction sa;
  sa.setInterest(SoSearchAction::ALL);
  sa.setType( SoSwitch::getClassTypeId() );
  sa.apply( SceneHandler::instance()->getRoot() );

  SoPathList switchPaths = sa.getPaths();

  vgui_dialog params("Show");  
  vcl_vector<SoSwitch*> nodes;
  vcl_vector<bool> show;

  for( int i=0; i< switchPaths.getLength(); i++ )
  {
    bool isShown;
    SoSwitch* swit = (SoSwitch*)switchPaths[i]->getTail();
    if( swit->whichChild.getValue() == SO_SWITCH_ALL )
      isShown = true;
    else
      isShown = false;
    nodes.push_back( swit );
    show.push_back( isShown );
  }

  // puts checkboxes in the dialog window for each element
  for( unsigned int i = 0; i<nodes.size(); i++ )
  {
     params.checkbox( nodes[i]->getName(), (bool&) show[i]);
  }
  
  // error message if there aren't any SoSwitches
  if(nodes.size() == 0)
    params.message("Cannot hide anything");
  if (!params.ask())
    return;

    
  // hide or unhide the child by changing whichChild 
  for( unsigned int i = 0; i<nodes.size(); i++)
  {
    if(show[i])
      ((SoSwitch*)nodes[i])->whichChild = SO_SWITCH_ALL;
    else 
      ((SoSwitch*)nodes[i])->whichChild = SO_SWITCH_NONE;
  }


}


void
MenuHandler::actionInterrogation()
{
  SceneEventHandler::instance()->setAction( INTERROGATION );
}

void
MenuHandler::actionAddition()
{
  SceneEventHandler::instance()->setAction( ADDITION );
}

void
MenuHandler::actionDeletion()
{
  SceneEventHandler::instance()->setAction( DELETION );
}

void
MenuHandler::actionAlteration()
{
  SceneEventHandler::instance()->setAction( ALTERATION );
}



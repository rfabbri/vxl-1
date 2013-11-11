/*************************************************************************
 *    NAME: Alexander K. Bowman
 *    USER: akb
 *    FILE: SceneEventHandler.cxx
 *    DATE: 
 *************************************************************************/

#include "SceneEventHandler.h"
#include "SceneHandler.h"
#include "MenuHandler.h"

#include <dbmsh/vis/dbmsh_vis_node.h>
#include <dbmsh/vis/dbmsh_vis_face.h>
#include <dbmsh/vis/dbmsh_vis_link.h>

#include <Inventor/nodes/SoSeparator.h>

#include <Inventor/draggers/SoDragPointDragger.h>



/*************************************************************************
 * Function Name: SceneEventHandler::SceneEventHandler
 * Parameters: 
 * Effects: 
 *************************************************************************/

SceneEventHandler::SceneEventHandler() : _action(ADDITION), _highlightColor(3),
  _draggerGroup(NULL)
{
      
}

/*************************************************************************
 * Function Name: SceneEventHandler::~SceneEventHandler
 * Parameters:
 * Effects:
 *************************************************************************/

SceneEventHandler::~SceneEventHandler()
{

}


/*************************************************************************
 * Function Name: SceneEventHandler::
 * Parameters:  
 * Returns: 
 * Effects:
 *************************************************************************/

SceneEventHandler*
SceneEventHandler::instance_ = 0;


/*************************************************************************
 * Function Name: SceneEventHandler::instance
 * Parameters:
 * Effects:
 *************************************************************************/
SceneEventHandler*
SceneEventHandler::instance()
{
  if (!instance_)
  {
    instance_ = new SceneEventHandler();
  }
  return SceneEventHandler::instance_;
}


/*************************************************************************
 * Function Name: SceneEventHandler::click
 * Parameters:
 * Effects:
 *************************************************************************/
void
SceneEventHandler::click( SoPath* path )
{
  if( _action == INTERROGATION )
    interrogation( path );
  else if( _action == ADDITION )
    interrogation( path );
  else if( _action == DELETION )
    deletion( path );
  else if( _action == ALTERATION )
    alteration( path );
}

/*************************************************************************
 * Function Name: SceneEventHandler::unclick
 * Parameters:
 * Effects:
 *************************************************************************/
void
SceneEventHandler::unclick( SoPath* path )
{
  if( _highlightedVector.size()>1 )
    addition( (dbmsh_vis_node*)path->getTail() );

  for( unsigned int i = 0; i<_highlightedVector.size(); i++ )
  {
    _highlightedVector[i]->unhighlight();
  }
  _highlightedVector.clear();

  _additionVector.clear();
  
  if( _draggerGroup )
    {
    SceneHandler::instance()->getRoot()->removeChild( _draggerGroup );
    _draggerGroup = NULL;
    }

}


/*************************************************************************
 * Function Name: SceneEventHandler::highlight
 * Parameters:
 * Effects:
 *************************************************************************/
void
SceneEventHandler::highlight( dbmsh_vis_shape* shape )
{
  _highlightedVector.push_back( shape );
  shape->highlight( SceneHandler::colorFromNumber( _highlightColor)  );
}

/*************************************************************************
 * Function Name: SceneEventHandler::interrogation
 * Parameters:
 * Effects:
 *************************************************************************/
void
SceneEventHandler::interrogation( SoPath* path )
{
  // GET THE NAME
  vcl_string name = path->getTail()->getTypeId().getName().getString();
  
  // NODE SHAPE
  if( path->getTail()->getTypeId() == dbmsh_vis_node::getClassTypeId() )
  {
    dbmsh_vis_node* node = (dbmsh_vis_node*)path->getTail();
    node->interrogate();
    highlight( node );
    _additionVector.push_back( node );

  }

  if( path->getTail()->getTypeId() == dbmsh_vis_face::getClassTypeId() )
  {
    dbmsh_vis_face* face = (dbmsh_vis_face*)path->getTail();
    face->interrogate();
    highlight( face );

  }

  if( path->getTail()->getTypeId() == dbmsh_vis_link::getClassTypeId() )
  {
    dbmsh_vis_link* link = (dbmsh_vis_link*)path->getTail();
    link->interrogate();
    highlight( link );

  }
  
}

/*************************************************************************
 * Function Name: SceneEventHandler::addition
 * Parameters:
 * Effects:
 *************************************************************************/
void
SceneEventHandler::addition( SoPath* path )
{
  // addition can only happen on an unclick
  // so this just keeps track of the highlighted cubes
  vcl_string name = path->getTail()->getTypeId().getName().getString();
  if( !strcmp( name.c_str(), "dbmsh_vis_node" ) )
  {
    dbmsh_vis_node* cube = (dbmsh_vis_node*)path->getTail();
    highlight( cube );
    _additionVector.push_back( cube );
  }
    
}

/*************************************************************************
 * Function Name: SceneEventHandler::addition
 * Parameters:
 * Effects:
 *************************************************************************/
void 
SceneEventHandler::addition( dbmsh_vis_node* cube )
{
  if( _additionVector.size() == 2 && cube == _highlightedVector[0]  )   
    SceneHandler::instance()->addLink( (dbmsh_vis_node*)_additionVector[0], (dbmsh_vis_node*)_additionVector[1] ); 
  if( _additionVector.size() > 2 && cube == _additionVector[0] )
    SceneHandler::instance()->addFace( _additionVector );
    

}

/*************************************************************************
 * Function Name: SceneEventHandler::deletion
 * Parameters:
 * Effects:
 *************************************************************************/
void
SceneEventHandler::deletion( SoPath* path )
{
  vcl_string name = path->getTail()->getTypeId().getName().getString();
  if( path->getTail()->getTypeId() == dbmsh_vis_node::getClassTypeId() )
  {
    dbmsh_vis_node* node = (dbmsh_vis_node*)path->getTail();
    SceneHandler::instance()->removeNode( node );
    
  }
  else if( path->getTail()->getTypeId() == dbmsh_vis_link::getClassTypeId() )
  {
    dbmsh_vis_link* link = (dbmsh_vis_link*)path->getTail();
    SceneHandler::instance()->removeLink( link );
    
  }
  else if( path->getTail()->getTypeId() == dbmsh_vis_face::getClassTypeId() )
  {
    dbmsh_vis_face* face = (dbmsh_vis_face*)path->getTail();
    SceneHandler::instance()->removeFace( face );
    
  }
}

/*************************************************************************
 * Function Name: SceneEventHandler::addition
 * Parameters:
 * Effects:
 *************************************************************************/
void 
SceneEventHandler::alteration( SoPath* path )
{
  vcl_string name = path->getTail()->getTypeId().getName().getString();
  if( path->getTail()->getTypeId() == dbmsh_vis_node::getClassTypeId() )
  {
    alteration( (dbmsh_vis_node*)path->getTail() );
  }
    

}


// This method is called by the dragger when it's moved
static void
dragger_moved(void * data, SoDragger * drag)
{
  SoDragPointDragger * dragger = (SoDragPointDragger *)drag;
  dbmsh_vis_node* node = (dbmsh_vis_node*)data;
  SbVec3f v = dragger->translation.getValue();    
  v*=(1.5f*MenuHandler::instance()->getNodeSize());
  //node->changePosition( node->getPosition(), v );
  node->setPosition( v );
}

/*************************************************************************
 * Function Name: SceneEventHandler::alteration
 * Parameters:
 * Effects: for Mesh editing 
 *************************************************************************/

void
SceneEventHandler::alteration( dbmsh_vis_node* node )
{
  if( _draggerGroup )
  {
    SceneHandler::instance()->getRoot()->removeChild( _draggerGroup );
    _draggerGroup = NULL;
  }
  highlight( node );
  

  SoSeparator* dragGroup = new SoSeparator;
  _draggerGroup = dragGroup;

  float size = 1.5f* MenuHandler::instance()->getNodeSize();

  SoScale* scale = new SoScale;
  scale->scaleFactor = SbVec3f( size, size, size );
  dragGroup->addChild( scale );

  SoDragPointDragger* dragger = new SoDragPointDragger;
  dragGroup->addChild(dragger);

  SceneHandler::instance()->getRoot()->addChild( dragGroup );
  dragger->translation.setValue( node->getPosition()/size );    
  
  dragger->addMotionCallback( dragger_moved, node );
  
  
}



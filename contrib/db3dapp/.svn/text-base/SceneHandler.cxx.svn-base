/*************************************************************************
 *    NAME: Alexander K. Bowman
 *    USER: akb
 *    FILE: SceneHandler.cxx
 *    DATE: 
 *************************************************************************/

#include "SceneHandler.h"
#include "SceneEventHandler.h"
#include "MenuHandler.h"

#include <dbmsh/vis/dbmsh_vis_node.h>
#include <dbmsh/vis/dbmsh_vis_link.h>
#include <dbmsh/vis/dbmsh_vis_face.h>

#include <dbmsh/dbmsh_node.h>
#include <dbmsh/dbmsh_link.h>
#include <dbmsh/dbmsh_face.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>

#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>

#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoDrawStyle.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>

#include <Inventor/SbColor.h>

/*************************************************************************
 * Function Name: SceneHandler::SceneHandler
 * Parameters: 
 * Effects: 
 *************************************************************************/

SceneHandler::SceneHandler()
{
}



/*************************************************************************
 * Function Name: SceneHandler::~SceneHandler
 * Parameters:
 * Effects:
 *************************************************************************/

SceneHandler::~SceneHandler()
{

}



SceneHandler*
SceneHandler::instance_ = 0;

SceneHandler*
SceneHandler::instance()
{
  if (!instance_)
  {
    instance_ = new SceneHandler();
  }
  return SceneHandler::instance_;
}

/*************************************************************************
 * Function Name: SceneHandler::setRoot
 * Parameters:
 * Effects:
 *************************************************************************/
void 
SceneHandler::setRoot(SoSeparator* root)
{
  _root = root;
  _faces = new SoSeparator;
  _links = new SoSeparator;
  _nodes = new SoSeparator;

  
  SoSwitch* nodeSwitch = new SoSwitch;
  nodeSwitch->setName( SbName( "Nodes" ) );
  nodeSwitch->whichChild = SO_SWITCH_ALL;
  SoSwitch* linkSwitch = new SoSwitch;
  linkSwitch->setName( SbName( "Links" ) );
  linkSwitch->whichChild = SO_SWITCH_ALL;
  SoSwitch* faceSwitch = new SoSwitch;
  faceSwitch->setName( SbName( "Faces" ) );
  faceSwitch->whichChild = SO_SWITCH_ALL;
  
  // draw both sides of polygons
  SoShapeHints* hints = new SoShapeHints; 
  hints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
  hints->vertexOrdering = SoShapeHints::CLOCKWISE;
  hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE;
  _faces->addChild( hints );

  // colors
  SoBaseColor* faceColor = new SoBaseColor;
  _faces->addChild( faceColor );

  SoBaseColor* linkColor = new SoBaseColor;
  _links->addChild( linkColor );

  SoBaseColor* nodeColor = new SoBaseColor;
  _nodes->addChild( nodeColor );

  // for line width
  SoDrawStyle* ds = new SoDrawStyle;
  _links->addChild( ds );

  nodeSwitch->addChild( _nodes );
  linkSwitch->addChild( _links );
  faceSwitch->addChild( _faces );

  _root->addChild( nodeSwitch );
  _root->addChild( linkSwitch );
  _root->addChild( faceSwitch );
  
}

/*************************************************************************
 * Function Name: SceneHandler::getRoot
 * Parameters:
 * Effects:
 *************************************************************************/
SoSeparator* 
SceneHandler::getRoot()
{
  return _root;
}


void 
SceneHandler::setCamera( SoCamera* camera )
{
  _camera = camera;

}


/*************************************************************************
 * Function Name: SceneHandler::clearAll
 * Parameters:
 * Effects:
 *************************************************************************/
void 
SceneHandler::clearAll()      
{
  _root->removeAllChildren();
}

/*************************************************************************
 * Function Name: SceneHandler::addNode
 * Parameters:
 * Effects:
 *************************************************************************/
dbmsh_vis_node*
SceneHandler::addNode( SbVec3f pos, float nodeSize )
{
    return addNode( _nodes, pos, nodeSize);
}


dbmsh_vis_node* 
SceneHandler::addNode( SoSeparator* group, SbVec3f pos, float nodeSize )
{
  SoSeparator* newNode = new SoSeparator;

  float x, y, z;
  pos.getValue(x, y, z);

  dbmsh_node* node = new dbmsh_node;
  node->setPosition( x, y, z );
  dbmsh_vis_node* cube = new dbmsh_vis_node;
  _meshNodes.push_back( node );
  cube->setItem( node );
  node->setDrawnItem( cube);
  
  cube->depth = nodeSize;
  cube->width = nodeSize;
  cube->height = nodeSize;
  cube->setParent( newNode );

  SoTranslation* trans = new SoTranslation;
  trans->translation = pos;
  cube->setTranslation( trans );

  newNode->addChild( trans );
  newNode->addChild( cube );

  group->addChild( newNode );

  return cube;
}


void 
SceneHandler::addNodeByDragger( float nodeSize )
{
  SbVec3f camera_pos = _camera->position.getValue();
  SbRotation camrot = _camera->orientation.getValue();
  float distance = _camera->focalDistance.getValue();
  distance*=(5.0f/6);
  SbVec3f lookat(0, 0, -1); // init to default view direction vector
  camrot.multVec(lookat, lookat);
  
  dbmsh_vis_node* node = addNode( _nodes, camera_pos + distance*lookat, nodeSize);
  SceneEventHandler::instance()->alteration( node );
  

}
/*************************************************************************
 * Function Name: SceneHandler::addLink
 * Parameters:
 * Effects:
 *************************************************************************/
dbmsh_vis_link*
SceneHandler::addLink( dbmsh_vis_node* node1, dbmsh_vis_node* node2 )
{
  return addLink( _links, node1, node2 );
}


dbmsh_vis_link*
SceneHandler::addLink( SoSeparator* group, dbmsh_vis_node* node1, dbmsh_vis_node* node2 )
{
  SoSeparator* newLinkSep = new SoSeparator;
  

  dbmsh_link* link = new dbmsh_link;
  dbmsh_vis_link* line = new dbmsh_vis_link;
  _meshLinks.push_back( link );
  link->setDrawnItem( line );
  line->setItem( link );
  line->setParent( newLinkSep );  
  vcl_vector<dbmsh_face*> faces = commonFaces( node1, node2 );
  for( unsigned int i=0; i<faces.size(); i++ )
  {
    dbmsh_face* face = faces[i];
    face->addLink( link );
    link->addFace( face );
  }  

  node1->getItem()->addLink( link );
  node2->getItem()->addLink( link );
  link->addNode( node1->getItem() );
  link->addNode( node2->getItem() );

  SoCoordinate3* coords = new SoCoordinate3;
  coords->point.set1Value( 0, node1->getPosition() );
  coords->point.set1Value( 1, node2->getPosition() );

  line->setCoords( coords );

  newLinkSep->addChild( coords );
  newLinkSep->addChild( line );

  group->addChild( newLinkSep );
  return line;
}

/*************************************************************************
 * Function Name: SceneHandler::addFace
 * Parameters:
 * Effects:
 *************************************************************************/

dbmsh_vis_face*
SceneHandler::addFace( vcl_vector<dbmsh_vis_node*> nodes )
{
  return addFace( _faces, nodes );
}


dbmsh_vis_face*
SceneHandler::addFace( SoSeparator* group, vcl_vector<dbmsh_vis_node*> nodes )
{
  SoSeparator* newFaceSep = new SoSeparator;
  
  dbmsh_face* face = new dbmsh_face;
  dbmsh_vis_face* sheet = new dbmsh_vis_face;
  _meshFaces.push_back( face );
  sheet->setItem( face );
  face->setDrawnItem( sheet );  
  sheet->setParent( newFaceSep );
  vcl_vector<dbmsh_link*> links = commonLinks( nodes );
  for( unsigned int i=0; i<links.size(); i++ )
  {
    dbmsh_link* link = links[i];
    link->addFace( face );
    face->addLink( link );
  }  


  SoCoordinate3* coords = new SoCoordinate3;
  for( unsigned int i = 0; i< nodes.size(); i++ )
    {
    dbmsh_vis_node* node = (dbmsh_vis_node*)nodes[i];
    face->addNode( node->getItem() );
    node->getItem()->addFace( face );
    coords->point.set1Value( i, node->getPosition() );
    }
  
  sheet->setCoords( coords );

  newFaceSep->addChild( coords );
  newFaceSep->addChild( sheet );

  group->addChild( newFaceSep );
  
  return sheet;

}

dbmsh_vis_face*
SceneHandler::addFace( vcl_queue<dbmsh_vis_node*> nodes )
{
  vcl_vector<dbmsh_vis_node*> newNodes;

  while(nodes.size() > 0 )
    {
    newNodes.push_back( nodes.front() );
    nodes.pop();
    }
  return addFace( newNodes );
}


/*************************************************************************
 * Function Name: SceneHandler::removeFace
 * Parameters:
 * Effects:
 *************************************************************************/
void SceneHandler::removeFace( dbmsh_vis_face *face )
{
  SoSearchAction sa;
  sa.setInterest( SoSearchAction::FIRST );
  sa.setNode( face );
  sa.apply( _faces );

  SoPath* path = sa.getPath();

  face->getItem()->remove();
  SoSeparator* parent = (SoSeparator*)path->getNodeFromTail(2);
  SoSeparator* faceGroup = (SoSeparator*)path->getNodeFromTail(1);
  parent->removeChild( faceGroup );

}

/*************************************************************************
 * Function Name: SceneHandler::removeLink
 * Parameters:
 * Effects:
 *************************************************************************/
void SceneHandler::removeLink( dbmsh_vis_link *link )
{
  SoSearchAction sa;
  sa.setInterest( SoSearchAction::FIRST );
  sa.setNode( link );
  sa.apply( _links );

  SoPath* path = sa.getPath();

  link->getItem()->remove();
  SoSeparator* parent = (SoSeparator*)path->getNodeFromTail(2);
  SoSeparator* linkGroup = (SoSeparator*)path->getNodeFromTail(1);
  parent->removeChild( linkGroup );
  
}

/*************************************************************************
 * Function Name: SceneHandler::removeNode
 * Parameters:
 * Effects:
 *************************************************************************/
void SceneHandler::removeNode( dbmsh_vis_node *node )
{
  
  SoSearchAction sa;
  sa.setInterest( SoSearchAction::FIRST );
  sa.setNode( node );
  sa.apply( _nodes );

  SoPath* path = sa.getPath();

  node->getItem()->remove();
  SoSeparator* parent = (SoSeparator*)path->getNodeFromTail(2);
  SoSeparator* nodeGroup = (SoSeparator*)path->getNodeFromTail(1);
  parent->removeChild( nodeGroup );
  
}


/*************************************************************************
 * Function Name: SceneHandler::changePointSize
 * Parameters:
 * Effects:
 *************************************************************************/
void SceneHandler::changePointSize( float size )
{
  SoSearchAction sa;
  sa.setInterest(SoSearchAction::ALL);
  sa.setType( SoDrawStyle::getClassTypeId() );
  sa.apply( _root );

  SoPathList switchPaths = sa.getPaths();
  // change the cube size to the new cube size
  for( int i=0; i< switchPaths.getLength(); i++ )
  {
    SoDrawStyle* ds = (SoDrawStyle*)switchPaths[i]->getTail();
    ds->pointSize = size;
  
  }
}

/*************************************************************************
 * Function Name: SceneHandler::changeNodeSize
 * Parameters:
 * Effects:
 *************************************************************************/
void SceneHandler::changeNodeSize( float size )
{
  SoSearchAction sa;
  sa.setInterest(SoSearchAction::ALL);
  sa.setType( SoCube::getClassTypeId() );
  sa.apply( _root );

  SoPathList switchPaths = sa.getPaths();
  // change the cube size to the new cube size
  for( int i=0; i< switchPaths.getLength(); i++ )
  {
    SoCube* cube = (SoCube*)switchPaths[i]->getTail();
    cube->height = size;
    cube->width = size;
    cube->depth = size;
  }
}

/*************************************************************************
 * Function Name: SceneHandler::changeLineWidth
 * Parameters:
 * Effects:
 *************************************************************************/
void SceneHandler::changeLineWidth( float width )
{
  SoSearchAction sa;
  sa.setInterest(SoSearchAction::ALL);
  sa.setType( SoDrawStyle::getClassTypeId() );
  sa.apply( _root );

  SoPathList switchPaths = sa.getPaths();
  // change the cube size to the new cube size
  for( int i=0; i< switchPaths.getLength(); i++ )
  {
    SoDrawStyle* ds = (SoDrawStyle*)switchPaths[i]->getTail();
    ds->lineWidth = width;
  
  }
}

/*************************************************************************
 * Function Name: SceneHandler::changeFaceColor
 * Parameters:
 * Effects:
 *************************************************************************/
void SceneHandler::changeFaceColor( int color )
{
  changeColor( _faces, color );
}

/*************************************************************************
 * Function Name: SceneHandler::changeLinkColor
 * Parameters:
 * Effects:
 *************************************************************************/
void SceneHandler::changeLinkColor( int color )
{
  changeColor( _links, color );
}

/*************************************************************************
 * Function Name: SceneHandler::clearNodeColor
 * Parameters:
 * Effects:
 *************************************************************************/
void SceneHandler::changeNodeColor( int color )
{
  changeColor( _nodes, color );
}

/*************************************************************************
 * Function Name: SceneHandler::changeColor
 * Parameters:
 * Effects:
 *************************************************************************/
void SceneHandler::changeColor( SoSeparator* root, int colorNumber )
{
  SoSearchAction sa;
  sa.setInterest(SoSearchAction::FIRST);
  sa.setType( SoBaseColor::getClassTypeId() );
  sa.apply( root );
  
  SbColor color = colorFromNumber( colorNumber );
  
  SoPath* path = sa.getPath();
  ((SoBaseColor*)path->getTail())->rgb = color;
}

/*************************************************************************
 * Function Name: SceneHandler::colorFromNumber
 * Parameters:
 * Effects:
 *************************************************************************/
SbColor SceneHandler::colorFromNumber( int number )
{
  SbColor color;
  switch( number )
  {
    case 0:  // GRAY
      color = SbColor( 0.5f, 0.5f, 0.5f );
    break;
    case 1:  // RED
      color = SbColor( 1, 0, 0 );
    break;
    case 2:  // ORANGE
      color = SbColor( 1, 0.4f, 0 );
    break;
    case 3:  // YELLOW
      color = SbColor( 1, 1, 0 );
    break;
    case 4:  // GREEN
      color = SbColor( 0, 1, 0 );
    break;
    case 5:  // BLUE
      color = SbColor( 0, 0, 1 );
    break;
    case 6:  // PURPLE
      color = SbColor( 0.6f, 0, 0.8f);
    break;
    case 7: // WHITE
      color = SbColor( 1, 1, 1 );
    break;
  }
  return color;
}

/*************************************************************************
 * Function Name: SceneHandler::commonFaces
 * Parameters:
 * Effects:
 *************************************************************************/
vcl_vector<dbmsh_face*> SceneHandler::commonFaces( dbmsh_vis_node* node1, dbmsh_vis_node* node2 )
{
  vcl_vector<dbmsh_face*> faces = node1->getItem()->commonFaces( node2->getItem() );

  return faces;
}

/*************************************************************************
 * Function Name: SceneHandler::commonLinks
 * Parameters:
 * Effects:
 *************************************************************************/
vcl_vector<dbmsh_link*> SceneHandler::commonLinks( vcl_vector<dbmsh_vis_node*> nodes )
{
  vcl_vector<dbmsh_link*> links;
  
  for( unsigned int i = 0; i< nodes.size(); i++ )
  {
    for( unsigned int j=i+1; j<nodes.size(); j++ )
    {
    dbmsh_link* link = nodes[i]->getItem()->commonLink( nodes[j]->getItem() );
    if( link )
      links.push_back( link );
    }
  }



  return links;
}



/*************************************************************************
 * Function Name: SceneHandler::viewAll
 * Parameters:
 * Effects:
 *************************************************************************/
/*
void SceneHandler::viewAll()
{
  _camera->viewAll(_root, SbViewportRegion() );
}
*/



/*************************************************************************
 * Function Name: SceneHandler::openIV
 * Parameters:
 * Effects:
 *************************************************************************/
void SceneHandler::openIV( vcl_string strfilename )
{
  const char* filename = strfilename.c_str();
  SoInput mySceneInput;
  if (!mySceneInput.openFile(filename)) 
  {
    vcl_fprintf(stderr, "Cannot open file %s\n", filename);
    return;
  }

   // Read the whole file into the database
  SoSeparator *scene = SoDB::readAll(&mySceneInput);
  if (scene == NULL) 
  {
    vcl_fprintf(stderr, "Problem reading IV file %s\n", filename);
    return;
  }
  scene->ref();
  
  SoSearchAction sa;
  sa.setInterest( SoSearchAction::ALL );
  sa.setType( SoIndexedFaceSet::getClassTypeId() );
  sa.apply( scene );
  
  SoPathList facePaths = sa.getPaths();

  sa.setType( SoIndexedTriangleStripSet::getClassTypeId() );
  sa.apply( scene );

  SoPathList stripPaths = sa.getPaths();

  float nodeSize = MenuHandler::instance()->getNodeSize();

  for( int i=0; i< facePaths.getLength(); i++ )
  {
    SoPath* path = facePaths[i];
    
    // get the indices
    SoIndexedFaceSet* indices = (SoIndexedFaceSet*)path->getTail();

    path->truncate( path->getLength() - 1 );
    SoSeparator* parent = (SoSeparator*)path->getTail();
    
    sa.setType( SoCoordinate3::getClassTypeId() );
    sa.setInterest( SoSearchAction::FIRST );
    sa.apply( parent );
     
    SoCoordinate3* vertices = (SoCoordinate3*)sa.getPath()->getTail();
        
    // get the normals
    //sa.setType( SoNormal::getClassTypeId() );
    //sa.apply( parent );
    
    //SoNormal* normals = (Normal*)sa.getPath()->getTail();

    vcl_vector<dbmsh_vis_node*> nodes;
    for( int curVert = 0; curVert < vertices->point.getNum(); curVert++ )
    {
      SbVec3f pos = vertices->point[ curVert ];
      nodes.push_back( addNode( _nodes, pos, nodeSize ) );
    }
    int curIndex = 0;
    while( curIndex< indices->coordIndex.getNum() )
    {
      // this while loop divides out the individual geometry
      int geomIndex = 0;
      vcl_vector< dbmsh_vis_node* > nodeShapes;
      while( indices->coordIndex[ curIndex ] != -1 )
      {
        nodeShapes.push_back( nodes[ indices->coordIndex[ curIndex ] ] );     
        curIndex++;
        geomIndex++;
      }
      
      addFace( _faces, nodeShapes );
      curIndex++;
    }
    parent->removeChild( indices );
    parent->removeChild( vertices );
  }
  //_root->addChild( scene );


  for( int i=0; i< stripPaths.getLength(); i++ ) 
  {
  
    SoPath* path = stripPaths[i];
    
    // get the indices
    SoIndexedTriangleStripSet* indices = (SoIndexedTriangleStripSet*)path->getTail();
    SoVertexProperty* prop = (SoVertexProperty*)indices->vertexProperty.getValue();
    
    path->truncate( path->getLength() - 1 );


    vcl_vector<dbmsh_vis_node*> nodes;
    
    for( int curVert = 0; curVert < prop->vertex.getNum(); curVert++ )
    {
      SbVec3f pos = prop->vertex[ curVert ];
      nodes.push_back( addNode( _nodes, pos, nodeSize ) );
    }
    
    // go through the indices dividing into individual geometries
    int curIndex = 0;
    
    while( curIndex< indices->coordIndex.getNum() ) {
      //new face
      vcl_queue<dbmsh_vis_node*> nodeShapes;

      // this while loop divides out the individual geometry
      int geomIndex = 0;
      while( indices->coordIndex[ curIndex ] != -1 )
      {
        nodeShapes.push( nodes[ indices->coordIndex[ curIndex ] ] );
        if( nodeShapes.size() == 3 )
        { 
          addFace( nodeShapes );
          nodeShapes.pop();
        }
        curIndex++;
        geomIndex++;
      }
      
      
      curIndex++;
    }  
  }

  scene->unref();
}

void SceneHandler::openFS( vcl_string strfilename )
{
}

void
SceneHandler::openP3D( vcl_string filename )
{
  float nodeSize = MenuHandler::instance()->getNodeSize();

  FILE* fp;
  if ( (fp = fopen(filename.c_str(), "r") ) == NULL) 
  {
    vcl_fprintf (stderr, "ERROR: Can't open input .P3D file %s\n", filename.c_str());
    return; 
  }
  int dim;
  vcl_fscanf (fp, "%d\n", &dim);
  assert (dim==3);
  int numGenes;
  vcl_fscanf (fp, "%d\n", &numGenes);

  for( int i=0; i<numGenes; i++ ) 
  {
    double x, y, z;
    vcl_fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
    addNode( SbVec3f(x,y,z), nodeSize );

  }

  fclose (fp);

}

void
SceneHandler::saveP3D( vcl_string filename )
{
  int num = _meshNodes.size();
  FILE* fp = fopen( filename.c_str(), "w" );
  if( fp == NULL )
  {
    vcl_fprintf (stderr, "ERROR: Can't create file %s\n", filename.c_str());
    return; 
  }
  vcl_fprintf( fp, "3\n" );
  vcl_fprintf( fp, "%i\n", num );
  for( int i=0; i<num; i++ )
  {
    SbVec3f pos = ((dbmsh_vis_node*)_meshNodes[i]->getDrawnItem())->getPosition();
    vcl_fprintf (fp, "%f %f %f\n", pos[0], pos[1], pos[2] );
  }
  

}

void
SceneHandler::saveIV( vcl_string filename, bool saveNodes, bool saveLinks, bool saveFaces )
{
  SoSeparator* saveRoot = new SoSeparator;
  saveRoot->ref();

  SoShapeHints* hints = new SoShapeHints; 
  hints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
  hints->vertexOrdering = SoShapeHints::CLOCKWISE;
  hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE;
  saveRoot->addChild( hints );


  SoCoordinate3* coords = new SoCoordinate3;
  saveRoot->addChild( coords );
  for( unsigned int i=0; i<_meshNodes.size(); i++ )
  {
    coords->point.set1Value( i, ((dbmsh_vis_node*)_meshNodes[i]->getDrawnItem())->getPosition() );
  }
  
  SoIndexedFaceSet* faceSet = new SoIndexedFaceSet;
  saveRoot->addChild( faceSet );

  int cur = 0;
  for( unsigned int i=0; i<_meshFaces.size(); i++)
  {
    SoCoordinate3* points = ((dbmsh_vis_face*)_meshFaces[i]->getDrawnItem())->getCoords();
    for( int j=0; j<points->point.getNum(); j++ )
    {
      int index = coords->point.find( points->point[j] );
      faceSet->coordIndex.set1Value( cur, index );
      cur++;
    }
  faceSet->coordIndex.set1Value( cur, -1 );
  cur++;
  }


  SoWriteAction wa;
  if( wa.getOutput()->openFile( filename.c_str() ));
  {
    wa.getOutput()->setBinary(false);
    wa.apply( saveRoot );
    wa.getOutput()->closeFile(); 
  }
  
  saveRoot->unref();

}



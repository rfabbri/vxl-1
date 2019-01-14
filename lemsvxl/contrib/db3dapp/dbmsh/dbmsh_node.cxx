/*************************************************************************
 *    NAME: Alexander K. Bowman
 *    USER: akb
 *    FILE: dbmsh_node.cxx
 *    DATE: 
 *************************************************************************/

#include "dbmsh_node.h"
#include "dbmsh_face.h"
#include "dbmsh_link.h"


/*************************************************************************
 * Function Name: dbmsh_node::dbmsh_node
 * Parameters: 
 * Effects: 
 *************************************************************************/

dbmsh_node::dbmsh_node()
{
 
}

/*************************************************************************
 * Function Name: dbmsh_node::~dbmsh_node
 * Parameters:
 * Effects:
 *************************************************************************/

dbmsh_node::~dbmsh_node()
{

}

/*************************************************************************
 * Function Name: dbmsh_node::remove
 * Parameters:
 * Effects:
 *************************************************************************/
void dbmsh_node::remove()
{
  std::set<dbmsh_face*>::iterator faceIter;
  for( faceIter = _faces.begin(); faceIter!=_faces.end(); faceIter++ )
    (*faceIter)->removeNode( this );
  std::set<dbmsh_link*>::iterator linkIter;
  for( linkIter = _links.begin(); linkIter!=_links.end(); linkIter++ )
    (*linkIter)->removeNode( this );
}

/*************************************************************************
 * Function Name: dbmsh_node::remove
 * Parameters:
 * Effects:
 *************************************************************************/
dbmsh_link* dbmsh_node::commonLink( dbmsh_node* node )
{
  dbmsh_link* link = NULL;
  
  std::set<dbmsh_link*> links = node->getLinks(); 
  
  std::set<dbmsh_link*>::iterator foundLink;
  std::set<dbmsh_link*>::iterator linkIter;
  for( linkIter = _links.begin(); linkIter!=_links.end(); linkIter++ )
  {
    // see if they're the same
    if( (foundLink = links.find( (*linkIter) ) ) != links.end() )
    {
      link = (*foundLink);
      break;
    }
  }
  

  return link;
}

/*************************************************************************
 * Function Name: dbmsh_node::remove
 * Parameters:
 * Effects:
 *************************************************************************/
std::vector<dbmsh_face*> dbmsh_node::commonFaces( dbmsh_node *node )
{
  std::vector<dbmsh_face*> facesVector;
  
  std::set<dbmsh_face*> faces = node->getFaces();

  std::set<dbmsh_face*>::iterator faceIter;
  std::set<dbmsh_face*>::iterator foundFace;
  
  for( faceIter = _faces.begin(); faceIter!=_faces.end(); faceIter++ )
  {
    if( ( foundFace = faces.find( (*faceIter) ) ) != faces.end() )
      facesVector.push_back( *foundFace ); 
  }

  return facesVector;
}




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
  vcl_set<dbmsh_face*>::iterator faceIter;
  for( faceIter = _faces.begin(); faceIter!=_faces.end(); faceIter++ )
    (*faceIter)->removeNode( this );
  vcl_set<dbmsh_link*>::iterator linkIter;
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
  
  vcl_set<dbmsh_link*> links = node->getLinks(); 
  
  vcl_set<dbmsh_link*>::iterator foundLink;
  vcl_set<dbmsh_link*>::iterator linkIter;
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
vcl_vector<dbmsh_face*> dbmsh_node::commonFaces( dbmsh_node *node )
{
  vcl_vector<dbmsh_face*> facesVector;
  
  vcl_set<dbmsh_face*> faces = node->getFaces();

  vcl_set<dbmsh_face*>::iterator faceIter;
  vcl_set<dbmsh_face*>::iterator foundFace;
  
  for( faceIter = _faces.begin(); faceIter!=_faces.end(); faceIter++ )
  {
    if( ( foundFace = faces.find( (*faceIter) ) ) != faces.end() )
      facesVector.push_back( *foundFace ); 
  }

  return facesVector;
}




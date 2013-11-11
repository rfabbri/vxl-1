/*************************************************************************
 *    NAME: Alexander K. Bowman
 *    USER: akb
 *    FILE: dbmsh_link.cxx
 *    DATE: 
 *************************************************************************/

#include "dbmsh_link.h"
#include "dbmsh_node.h"
#include "dbmsh_face.h"


/*************************************************************************
 * Function Name: dbmsh_link::dbmsh_link
 * Parameters: 
 * Effects: 
 *************************************************************************/

dbmsh_link::dbmsh_link()
{
 
}

/*************************************************************************
 * Function Name: dbmsh_link::~dbmsh_link
 * Parameters:
 * Effects:
 *************************************************************************/

dbmsh_link::~dbmsh_link()
{

}

/*************************************************************************
 * Function Name: dbmsh_link::remove
 * Parameters:
 * Effects:
 *************************************************************************/

void dbmsh_link::remove()
{
  vcl_set<dbmsh_node*>::iterator nodeIter;
  for( nodeIter = _nodes.begin(); nodeIter!=_nodes.end(); nodeIter++ )
    (*nodeIter)->removeLink( this );
  vcl_set<dbmsh_face*>::iterator faceIter;
  for( faceIter = _faces.begin(); faceIter!=_faces.end(); faceIter++ )
    (*faceIter)->removeLink( this );
}


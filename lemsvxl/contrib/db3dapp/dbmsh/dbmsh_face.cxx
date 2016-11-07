/*************************************************************************
 *    NAME: Alexander K. Bowman
 *    USER: akb
 *    FILE: dbmsh_face.cxx
 *    DATE: 
 *************************************************************************/

#include "dbmsh_face.h"
#include "dbmsh_link.h"
#include "dbmsh_node.h"

/*************************************************************************
 * Function Name: dbmsh_face::dbmsh_face
 * Parameters: 
 * Effects: 
 *************************************************************************/

dbmsh_face::dbmsh_face()
{
 
}

/*************************************************************************
 * Function Name: dbmsh_face::~dbmsh_face
 * Parameters:
 * Effects:
 *************************************************************************/

dbmsh_face::~dbmsh_face()
{

}

/*************************************************************************
 * Function Name: dbmsh_face::remove
 * Parameters:
 * Effects:
 *************************************************************************/
void dbmsh_face::remove()
{
  vcl_set<dbmsh_link*>::iterator linkIter;
  for( linkIter = _links.begin(); linkIter!=_links.end(); linkIter++ )
    (*linkIter)->removeFace( this );
  vcl_set<dbmsh_node*>::iterator nodeIter;
  for( nodeIter = _nodes.begin(); nodeIter!=_nodes.end(); nodeIter++ )
    (*nodeIter)->removeFace( this );

}


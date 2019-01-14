/**************************************************************/
/*    Name: Alexander Bowman akb
//    File: SceneHandler.h
//    Asgn:
//    Date:
***************************************************************/

#ifndef SceneHandler_h_
#define SceneHandler_h_

#include <string>
#include <Inventor/SbLinear.h>
#include <vector>
#include <queue>

class SoSeparator;
class SoCamera;
class dbmsh_vis_node;
class dbmsh_vis_shape;
class dbmsh_vis_link;
class dbmsh_vis_face;
class dbmsh_face;
class dbmsh_link;
class dbmsh_node;

#include <Inventor/SbColor.h>

class SceneHandler{

public:

  SceneHandler();
  virtual ~SceneHandler();
  static SceneHandler* instance();
  
  void setRoot( SoSeparator* root );
  SoSeparator* getRoot();       
  void setCamera( SoCamera* camera );
  //void viewAll();

  void clearAll();       

  void openIV( std::string filename );
  void openFS( std::string filename );
  void openP3D( std::string filename );
  void saveP3D( std::string filename );
  void saveIV( std::string filename, bool saveNodes, bool saveLinks, bool saveFaces );

  dbmsh_vis_node* addNode( SbVec3f pos, float nodeSize );
  dbmsh_vis_node* addNode( SoSeparator* group, SbVec3f pos, float nodeSize );
  void addNodeByDragger( float nodeSize );
  dbmsh_vis_link* addLink( dbmsh_vis_node* node1, dbmsh_vis_node* node2 );
  dbmsh_vis_link* addLink( SoSeparator* group, dbmsh_vis_node* node1, dbmsh_vis_node* node2 );
  dbmsh_vis_face* addFace( std::vector<dbmsh_vis_node*> nodes );
  dbmsh_vis_face* addFace( SoSeparator* group, std::vector<dbmsh_vis_node*> nodes );
  dbmsh_vis_face* addFace( std::queue<dbmsh_vis_node*> nodes );

  void removeNode( dbmsh_vis_node* node );
  void removeLink( dbmsh_vis_link* link );
  void removeFace( dbmsh_vis_face* face );

  void changeNodeSize( float nodeSize );
  void changeLineWidth( float lineWidth );
  void changePointSize( float pointSize );

  void changeNodeColor( int color ); 
  void changeLinkColor( int color );
  void changeFaceColor( int color );
  void changeColor( SoSeparator* root, int color );
  static SbColor colorFromNumber( int colorNumber );
  
  static std::vector<dbmsh_face*> commonFaces( dbmsh_vis_node* node1, dbmsh_vis_node* node2 );
  static std::vector<dbmsh_link*> commonLinks( std::vector<dbmsh_vis_node*> );  


protected:

private:
  static SceneHandler* instance_;
  SoSeparator* _root;
  SoCamera* _camera;

  SoSeparator* _nodes;
  SoSeparator* _links;
  SoSeparator* _faces;

  std::vector<dbmsh_node*> _meshNodes;
  std::vector<dbmsh_link*> _meshLinks;
  std::vector<dbmsh_face*> _meshFaces;


};

#endif

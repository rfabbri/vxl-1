/**************************************************************/
/*  Name: Alexander Bowman
//  File: SceneEventHandler.h
//  Asgn:
//  Date:
***************************************************************/

#ifndef SceneEventHandler_h_
#define SceneEventHandler_h_

class SoPath;
class dbmsh_vis_shape;
class dbmsh_vis_node;
class SoDragger;
class SoSeparator;

#include <vcl_vector.h>
#include <Inventor/SbColor.h>

enum Action {
INTERROGATION,
ADDITION,
DELETION,
ALTERATION
};

class SceneEventHandler 
{
private:
  static SceneEventHandler* instance_;
  
  Action _action;
  vcl_vector<dbmsh_vis_shape*> _highlightedVector;
  vcl_vector<dbmsh_vis_node*> _additionVector;

  int _highlightColor;

  SoSeparator* _draggerGroup;
  SoSeparator* _parent;

public:
  SceneEventHandler();
  virtual ~SceneEventHandler();
  static SceneEventHandler* instance();
  
  void click( SoPath* path );
  void unclick( SoPath* path );

  void addition( SoPath* path );
  void addition( dbmsh_vis_node* node );

  void interrogation( SoPath* path );

  void deletion( SoPath* path );

  void alteration( SoPath* path );
  void alteration( dbmsh_vis_node* node );

  void highlight( dbmsh_vis_shape* shape );

  void setAction( Action action ) { _action = action; }
  int getHighlightColor() { return _highlightColor; }
  void changeHighlightColor( int newColor ) { _highlightColor = newColor; }
};

#endif

/**************************************************************/
/*  Name: MingChing Chang and Alexander Bowman
//  File: dbsk3dapp_action.h
//  Asgn:
//  Date:
***************************************************************/

#ifndef _dbsk3dapp_action_h_
#define _dbsk3dapp_action_h_

#include "dbsk3dapp_view_manager.h"

class dbmsh3dvis_gui_elm;
class SbColor;
class fs_vertex_SoCube;

class dbsk3dapp_action 
{
public:
  typedef enum {
    ACT_DELETION=0,
    ACT_INTERROGATION,
    ACT_SELECTION,
    ACT_ADDITION,
    ACT_ALTERATION
  } ACTION;

protected:
  dbsk3dapp_view_manager*  view_manager_;
  ACTION                action_;

  vcl_vector<SoGroup*>  _parents;
  vcl_vector<SoNode*>   _children;

public:
  dbsk3dapp_action (dbsk3dapp_view_manager* view) {
    action_ = ACT_INTERROGATION;
    view_manager_ = view;
  }
  virtual ~dbsk3dapp_action() {
  }

  void set_action (ACTION action) { 
    action_ = action; 
  }

  SoGroup* get_parent_group (SoNode* sonode);

  void select_highlight (dbmsh3dvis_gui_elm *shape,
                         SoGroup* parent,
                         SbColor rgb);

  //####### Action handling functions ########
  void action_click (SoPath* path);
  void action_unclick (SoPath* path);

  void action_interrogation (SoPath* path);
  void action_selection (SoPath* path);
  void action_alteration( SoPath* path );
  void alteration_node ( fs_vertex_SoCube* cube );

};

void callback_click (void* data, SoPath* path);
void callback_unclick (void* data, SoPath* path);

#endif

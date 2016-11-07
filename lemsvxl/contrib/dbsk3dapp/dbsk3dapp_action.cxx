/*************************************************************************
 *    NAME: Alexander K. Bowman
 *    USER: akb
 *    FILE: dbsk3dapp_action.cxx
 *    DATE: 
 *************************************************************************/

#include <vul/vul_sprintf.h>

#include <dbmsh3d/dbmsh3d_utils.h>
#include <dbsk3d/dbsk3d_fs_vertex.h>
#include <dbsk3d/dbsk3d_fs_edge.h>
#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_sheet.h>

//note the order here!
#include "dbsk3dapp_action.h"
#include "dbsk3dapp_manager.h"

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbsk3d/vis/dbsk3d_vis_backpt.h>

#include <Inventor/draggers/SoDragPointDragger.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>

#define SELECT_COLOR SbColor(1.0f,0.5f,1.0f)
#define SELECT_CONNECTED_COLOR SbColor(0.9f,0.9f,0.9f)

#define RED SbColor(1,0,0)
#define GREEN SbColor(0,1,0)
#define BLUE SbColor(0,0,1)
#define YELLOW SbColor(1,1,0)
#define PURPLE SbColor(1,0,1)
#define CYAN SbColor(0,1,1)

// ####################################################################

void callback_click (void* data, SoPath* path)
{
  dbsk3dapp_manager::instance()->action()->action_click (path);
}

void callback_unclick (void* data, SoPath* path)
{
  dbsk3dapp_manager::instance()->action()->action_unclick (path);
}

// ####################################################################

SoGroup* dbsk3dapp_action::get_parent_group (SoNode* sonode)
{
  return view_manager_->get_parent_group (sonode);
}

void dbsk3dapp_action::select_highlight (dbmsh3dvis_gui_elm *shape,
                                         SoGroup* parent, SbColor color)
{
  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;

  parent->insertChild (basecolor, 1);

  _parents.push_back (parent);
  _children.push_back (basecolor);
}

// ###############################################################################
//       Action handling functions
// ###############################################################################

void dbsk3dapp_action::action_click (SoPath* path)
{
  switch (action_) {
    case ACT_INTERROGATION:
      action_selection (path);
      action_interrogation (path);
    break;
    case ACT_SELECTION:
      action_selection( path );
    break;
    case ACT_DELETION:
    break;
    case ACT_ALTERATION:
      action_alteration( path );
    break;
    case ACT_ADDITION:
    break;
  }
}

void dbsk3dapp_action::action_unclick (SoPath* path)
{
  // removes things done when clicked
  for (unsigned int i = 0; i<_parents.size(); i++) {
    _parents[i]->removeChild( _children[i] );
  }
  _parents.clear();
  _children.clear();  
}

void dbsk3dapp_action::action_interrogation (SoPath* path)
{
  vcl_string name = path->getTail()->getTypeId().getName().getString();
  vcl_ostringstream ostrm;

  if(!strcmp("fs_vertex_SoCube", name.c_str())) {
    fs_vertex_SoCube* guielement = (fs_vertex_SoCube*) path->getTail();
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) guielement->element();
    FV->getInfo (ostrm);
  }
  else if (!strcmp("fs_edge_SoLineSet", name.c_str())) {
    fs_edge_SoLineSet* guielement = (fs_edge_SoLineSet*) path->getTail();
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) guielement->element();
    FE->getInfo (ostrm);
  }
  else if (!strcmp("fs_face_SoFaceSet", name.c_str())) {
    dbmsh3dvis_face_SoFaceSet* guielement = (dbmsh3dvis_face_SoFaceSet*) path->getTail();
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) guielement->element();
    FF->getInfo (ostrm);
  }
  else if (!strcmp("fs_sheet_SoIndexedFaceSet", name.c_str())) {
    fs_sheet_SoIndexedFaceSet* guielement = (fs_sheet_SoIndexedFaceSet*) path->getTail();
    dbsk3d_fs_sheet* S = (dbsk3d_fs_sheet*) guielement->element();
    S->getInfo (ostrm);
  }
  else if (!strcmp("ms_node_SoSphere", name.c_str())) {
    ms_node_SoSphere* guielement = (ms_node_SoSphere*) path->getTail();
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) guielement->element();
    MN->getInfo (ostrm);
  }
  else if (!strcmp("ms_curve_SoLineSet", name.c_str())) {
    ms_curve_SoLineSet* guielement = (ms_curve_SoLineSet*) path->getTail();
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) guielement->element();
    MC->getInfo (ostrm);
  }
  else if (!strcmp("ms_sheet_SoIndexedFaceSet", name.c_str())) {
    ms_sheet_SoIndexedFaceSet* guielement = (ms_sheet_SoIndexedFaceSet*) path->getTail();
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) guielement->element();
    MS->getInfo (ostrm);
  }
  else if (!strcmp("dbmsh3dvis_vertex_SoPointSet", name.c_str())) {
    //: works only if each generator is drawn individually
    dbmsh3dvis_vertex_SoPointSet* guielement = (dbmsh3dvis_vertex_SoPointSet*) path->getTail();
    dbmsh3d_vertex* G = (dbmsh3d_vertex*) guielement->element();
    G->getInfo (ostrm);
  }
  else if (!strcmp("PointSet", name.c_str())) {
    //: works if all generators are drawn as a point cloud
    vcl_cout<<"Point Cloud";
  }

  vcl_cout<< ostrm.str();
}

void dbsk3dapp_action::action_selection (SoPath* path)
{
  vcl_string name = path->getTail()->getTypeId().getName().getString();
  
  //: Assume only one parent of the path->getTail().
  SoGroup* parent = get_parent_group (path->getTail());

  if (!strcmp("fs_face_SoFaceSet", name.c_str())) {
    select_highlight ((fs_face_SoFaceSet*)path->getTail(), 
                       parent, SELECT_CONNECTED_COLOR );    
  }
  else if (!strcmp("fs_edge_SoLineSet", name.c_str())) {
    select_highlight ((fs_edge_SoLineSet*)path->getTail(), 
                       parent, SELECT_CONNECTED_COLOR );
  }
  else if(!strcmp("fs_vertex_SoCube", name.c_str())) {
    select_highlight ((fs_vertex_SoCube*)path->getTail(), 
                       parent, SELECT_CONNECTED_COLOR );
  }
  else if (!strcmp("ms_sheet_SoIndexedFaceSet", name.c_str())) {
    select_highlight ((ms_sheet_SoIndexedFaceSet*)path->getTail(), 
                       parent, SELECT_CONNECTED_COLOR );
  }
  else if (!strcmp("ms_curve_SoLineSet", name.c_str())) {
    select_highlight ((ms_curve_SoLineSet*)path->getTail(), 
                       parent, SELECT_CONNECTED_COLOR );
  }
  else if (!strcmp("ms_node_SoSphere", name.c_str())) {
    select_highlight ((ms_node_SoSphere*)path->getTail(), 
                       parent, SELECT_CONNECTED_COLOR );
  }
  else if (!strcmp("dbmsh3dvis_vertex_SoPointSet", name.c_str())) {
    //: only work when dbmsh3dvis_vertex_SoPointSet is drawn!
    select_highlight ((dbmsh3dvis_vertex_SoPointSet*)path->getTail(), 
                       parent, SELECT_CONNECTED_COLOR );
  }
} 

void dbsk3dapp_action::action_alteration (SoPath* path)
{
  vcl_string name = path->getTail()->getTypeId().getName().getString();
  
  SoGroup* parent = get_parent_group (path->getTail());

  if (path->getTail()->getTypeId() == fs_vertex_SoCube::getClassTypeId()) {
    
    select_highlight ((fs_vertex_SoCube*)path->getTail(), 
                       parent, SELECT_CONNECTED_COLOR );

    alteration_node ( (fs_vertex_SoCube*)path->getTail() );
  }
}

// This method is called by the dragger when it's moved
static void dragger_moved (void * data, SoDragger * drag)
{
  SoDragPointDragger * dragger = (SoDragPointDragger *)drag;
  fs_vertex_SoCube* node = (fs_vertex_SoCube*)data;
  SbVec3f v = dragger->translation.getValue();    
  v*=(1.5f*dbsk3dapp_manager::instance()->dual_bnd_shock_vis()->cube_size_);
  node->translation()->translation = v;
  vgl_point_3d<double>& point = ((dbsk3d_fs_vertex*)node->element())->get_pt();
  point.set (v[0], v[1], v[2]);
  
  dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) node->element();
  vcl_set<dbsk3d_fs_edge*> links;
  vcl_set<dbsk3d_fs_face*> patches;
  FV->get_incident_elms (links, patches);

  vcl_set<dbsk3d_fs_edge*>::iterator lit = links.begin();
  for (; lit != links.end(); lit++) {
    fs_edge_SoLineSet* line = (fs_edge_SoLineSet*)(*lit)->vis_pointer();
    if (line)
      line->update();
  }

  vcl_set<dbsk3d_fs_face*>::iterator pit = patches.begin();
  for (; pit != patches.end(); pit++) {
    fs_face_SoFaceSet* patch = (fs_face_SoFaceSet*)(*pit)->vis_pointer();
    if (patch)
      patch->update();
  }
}

void dbsk3dapp_action::alteration_node (fs_vertex_SoCube* node)
{
  vcl_cout<< "alteration\n";

  SoSeparator* dragGroup = new SoSeparator;

  float size = 1.5f* dbsk3dapp_manager::instance()->dual_bnd_shock_vis()->cube_size_;
  SoScale* scale = new SoScale;
  scale->scaleFactor = SbVec3f( size, size, size );
  dragGroup->addChild( scale );
  
  SoDragPointDragger* dragger = new SoDragPointDragger();
  dragGroup->addChild(dragger);


  SoGroup* parent = getParent (node,
                               dbsk3dapp_manager::instance()->view_manager()->master_root());
  parent->insertChild(dragGroup,0 );


  vgl_point_3d<double> point = ((dbsk3d_fs_vertex*)node->element())->get_pt();
  dragger->translation.setValue( point.x()/size, point.y()/size, point.z()/size );
    
  dragger->addMotionCallback( dragger_moved, node );    
}











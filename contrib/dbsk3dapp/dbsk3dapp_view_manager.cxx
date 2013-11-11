/*************************************************************************
 *    NAME: Alexander K. Bowman
 *    USER: akb
 *    FILE: dbsk3dapp_view_manager.cxx
 *    DATE: 
 *************************************************************************/

#include "dbsk3dapp_view_manager.h"
#include "dbsk3dapp_manager.h"

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>

#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>

// ####################################################################

dbsk3dapp_view_manager::dbsk3dapp_view_manager()
{
}

dbsk3dapp_view_manager::~dbsk3dapp_view_manager()
{
  _camera->unref();

  master_root_->removeAllChildren();
  //Ming: You may have a crash here if the coin2d.dll is not the correct version!
  master_root_->unref();
}

// ####################################################################

SoGroup* dbsk3dapp_view_manager::get_parent_group (SoNode* sonode)
{
  return getParent (sonode, master_root_);
}

bgui3d_examiner_tableau_new dbsk3dapp_view_manager::_add_one_view_tableau ()
{
  SoSeparator* view_root = new SoSeparator;
  view_root->addChild (_camera);
  master_root_->addChild (view_root);
  assign_rgb_light (view_root);

  SoSelection* selection = new SoSelection;
  view_root->addChild (selection);
  view_drawing_SoSelections_.push_back (selection);

  selection->policy = SoSelection::SHIFT;
  selection->addSelectionCallback (callback_click);
  selection->addDeselectionCallback (callback_unclick);

  bgui3d_examiner_tableau_new tab3d (view_root);
  tab3d->enable_idle();
  tableaus_.push_back (tab3d);

  return tab3d;
}

// set up the initial M x N tableaus
#define MM 2 // 4,1,1
#define NN 2 // 4,2,1

// the initial view
#define M 1 // 4,1,1
#define N 2 // 4,2,1

void dbsk3dapp_view_manager::setup_scene (vgui_grid_tableau_sptr grid)
{
  grid_tableau_ = grid;

  master_root_ = new SoSeparator;
  master_root_->ref();

  _camera = new SoPerspectiveCamera;
  _camera->ref();

  //: setup the 4x4 16 tableaus!!
  for( int i=0; i<(MM*NN); i++ ) {    
    bgui3d_examiner_tableau_new tab3d = _add_one_view_tableau();
    grid->add_at (tab3d, i%NN, i/NN);
  }

  resize_grid (M,N);

  //: default grid MxN
  for (int i=0; i<(M*N); i++)
    grid_tableau_->add_at (tableaus_[i], i%N, i/N); 
   
} 

// view the entire scenegraph
void dbsk3dapp_view_manager::view_all ()
{
  _camera->viewAll (master_root_, SbViewportRegion());
}
  
// zoom in on a specific part of a scenegraph
void dbsk3dapp_view_manager::view_subgraph (SoNode* node)
{
  _camera->viewAll (node, SbViewportRegion());
}


//: adds SoNode node to view i of the grid
void dbsk3dapp_view_manager::add_to_view (unsigned int i, SoNode* node) 
{
  assert (i < tableaus_.size());
  assert (i < view_drawing_SoSelections_.size());

  /*SoSeparator* test = new SoSeparator;
  view_drawing_SoSelections_[i]->addChild (test);
  view_drawing_SoSelections_[i]->removeChild (test);*/

  ///int numCols = grid_tableau_->cols();  
  ///int row = i/numCols;
  ///int col = i%numCols;

  ///vgui_tableau_sptr vtab_ptr = grid_tableau_->get_tableau_at (col, row);
  ///vgui_tableau* vtab = vtab_ptr.ptr(); /////unused 

  view_drawing_SoSelections_[i]->addChild (node);
}


void dbsk3dapp_view_manager::clear_view (unsigned int i) 
{ 
  assert (i < view_drawing_SoSelections_.size());
  view_drawing_SoSelections_[i]->removeAllChildren(); 
}

void dbsk3dapp_view_manager::clear_all_views () 
{ 
  for (unsigned int i=0; i<view_drawing_SoSelections_.size(); i++) 
    clear_view (i); 
}

// resizes the grid tableau to rows x cols
void dbsk3dapp_view_manager::resize_grid (unsigned int rows, unsigned int cols)
{
  int numRows = grid_tableau_->rows();
  int numCols = grid_tableau_->cols();

  while (numRows > rows) {
    grid_tableau_->remove_row();
    numRows--;
  }
  while (numRows < rows) {
    grid_tableau_->add_row();
    numRows++;
  }
  while (numCols > cols) {
    grid_tableau_->remove_column();
    numCols--;
  }
  while (numCols< cols) {
    grid_tableau_->add_column();
    numCols++;
  }
}

// queries which layout you want for a number of grid spots
void dbsk3dapp_view_manager::choose_nxn_grid (unsigned int num)
{
  vgui_dialog params("Rows and Columns");
  vcl_vector<vcl_string> choices;
  int pick;

  switch (num) {
    case 1:
      resize_grid (1, 1);
    break;
    case 2:
      choices.push_back( "1x2");
      choices.push_back( "2x1");
      pick = 0;
      params.choice("Rows x Columns", choices, pick );
      if( !params.ask() )
        return;
      if( pick == 0 )
        resize_grid (1, 2);
      else
        resize_grid (2, 1);
    break;
    case 3:
      choices.push_back( "1x3");
      choices.push_back( "3x1");
      pick = 0;
      params.choice("Rows x Columns", choices, pick );
      if( !params.ask() )
        return;
      if( pick == 0 )
        resize_grid (1, 3);
      else
        resize_grid (3, 1);
    break;
    case 4:
      choices.push_back( "2x2" );
      choices.push_back( "1x4" );
      choices.push_back( "4x1" );
      pick = 0;
      params.choice("Rows x Columns", choices, pick );
      if( !params.ask() )
        return;
      if( pick == 0 )
        resize_grid (2, 2);
      else if( pick == 1 )
        resize_grid (1, 4);
      else
        resize_grid (4, 1);
    break;
    case 5:
      choices.push_back( "2x3" );
      choices.push_back( "3x2" );
      pick = 0;
      params.choice("Rows x Columns", choices, pick );
      if( !params.ask() )
        return;
      if( pick == 0 )
        resize_grid (2, 3);
      else if( pick == 1 )
        resize_grid (3, 2);
    break;
    case 6:
      choices.push_back( "2x3" );
      choices.push_back( "3x2" );
      pick = 0;
      params.choice("Rows x Columns", choices, pick );
      if( !params.ask() )
        return;
      if( pick == 0 )
        resize_grid (2, 3);
      else if( pick == 1 )
        resize_grid (3, 2);
    break;
    case 7:
      choices.push_back( "2x4" );
      choices.push_back( "4x2" );
      pick = 0;
      params.choice("Rows x Columns", choices, pick );
      if( !params.ask() )
        return;
      if( pick == 0 )
        resize_grid (2, 4);
      else if( pick == 1 )
        resize_grid (4, 2);
    break;
    case 8:
      choices.push_back( "2x4" );
      choices.push_back( "4x2" );
      pick = 0;
      params.choice("Rows x Columns", choices, pick );
      if( !params.ask() )
        return;
      if( pick == 0 )
        resize_grid (2, 4);
      else if( pick == 1 )
        resize_grid (4, 2);
    break;
    case 9:
      resize_grid(3, 3);
    break;

    case 10:
      choices.push_back( "3x4" );
      choices.push_back( "4x3" );
      pick = 0;
      params.choice("Rows x Columns", choices, pick );
      if( !params.ask() )
        return;
      if( pick == 0 )
        resize_grid (3, 4);
      else if( pick == 1 )
        resize_grid (4, 3);
    break;
    case 11:
      choices.push_back( "3x4" );
      choices.push_back( "4x3" );
      pick = 0;
      params.choice("Rows x Columns", choices, pick );
      if( !params.ask() )
        return;
      if( pick == 0 )
        resize_grid (3, 4);
      else if( pick == 1 )
        resize_grid (4, 3);
    break;
    case 12:
      choices.push_back( "3x4" );
      choices.push_back( "4x3" );
      pick = 0;
      params.choice("Rows x Columns", choices, pick );
      if( !params.ask() )
        return;
      if( pick == 0 )
        resize_grid (3, 4);
      else if( pick == 1 )
        resize_grid (4, 3);
    break;
    default:
      resize_grid (4, 4);
    break;
  }
}

// views "num" grid spots
void dbsk3dapp_view_manager::add_tableaus_to_grid ()
{
  int numCols = grid_tableau_->cols();  
  for( unsigned i=0; i<tableaus_.size(); i++ ) {
    grid_tableau_->add_at( tableaus_[i], i%numCols, i/numCols );
  }
  unselect_all_tableaus ();
}

// takes the selected grid tableaus and displays them by themselves
void dbsk3dapp_view_manager::view_selected_tableaus ()
{
  vcl_vector<int> col;
  vcl_vector<int> row;
  vcl_vector<int> times;
  
  unsigned int num = grid_tableau_->get_selected_positions (&col, &row, &times);
  
  vcl_vector<bgui3d_examiner_tableau*> tabs;
  for (unsigned int i=0; i<num; i++) {
    vgui_tableau_sptr vtab_ptr = grid_tableau_->get_tableau_at( col[i], row[i] );
    vgui_tableau* vtab = vtab_ptr.ptr();
    bgui3d_examiner_tableau* tab = (bgui3d_examiner_tableau*)vtab;
    tabs.push_back(tab);
  }

  choose_nxn_grid (num);
  ///add_tableaus_to_grid ();

  int numCols = grid_tableau_->cols();

  for (unsigned i=0; i<tabs.size(); i++)
    grid_tableau_->add_at( tabs[i], i%numCols, i/numCols );  

  unselect_all_tableaus ();
}


// unselects all the selected grid locations
void dbsk3dapp_view_manager::unselect_all_tableaus ()
{
  int numRows = grid_tableau_->rows();
  int numCols = grid_tableau_->cols();  
  
  // deselect the grid areas
  for( int i=0; i<numRows; i++ )
    for( int j=0; j<numCols; j++ )
      grid_tableau_->set_selected (i, j, false);
}

// set the text in the active tableau
void dbsk3dapp_view_manager::set_active_tableau_text (const vcl_string & string)
{
  unsigned int col;
  unsigned int row;
  
  // get active tableau position
  grid_tableau_->get_active_position (&col, &row);

  // get tableau of that position
  vgui_tableau_sptr vtab_ptr = grid_tableau_->get_tableau_at (col, row);
  vgui_tableau* vtab = vtab_ptr.ptr();
  
  bgui3d_examiner_tableau* tab = (bgui3d_examiner_tableau*) vtab;
  
  tab->setText (string);
}


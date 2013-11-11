// This is dbsk3d_fs_sheet_set.cxx
//: MingChing Chang
//  Nov 30, 2006        Creation

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <dbsk3d/dbsk3d_fs_sheet_set.h>

//####################### Sheet Component Extraction #######################

void dbsk3d_fs_sheet_set::build_fs_sheet_set ()
{
  vul_printf (vcl_cout, "\nbuild_fs_sheet_set(): totally %u fs_faces.\n", 
               fs_mesh_->facemap().size());
  fs_sheet_id_counter_ = 0;
  assert (sheetmap_.size() == 0);

  //Detect shock link types.
  fs_mesh_->detect_valid_FE_type ();

  //Go through all shock-patch-elms and reset i_value_ flag to -1.
  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_mesh_->facemap().begin();
  for (; pit != fs_mesh_->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    if (FF->b_valid())
      FF->set_sid (FF_UNVISITED_SID);
    else
      FF->set_sid (FF_INVALID_SID);
  }

  //Go through all shock-patch-elms again and label sheet components.
  pit = fs_mesh_->facemap().begin();
  for (; pit != fs_mesh_->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;

    //Create a new sheet component S if FF is valid and not labelled before.
    if (FF->b_valid() && FF->sid() == FF_UNVISITED_SID) {       
      dbsk3d_fs_sheet* S = new dbsk3d_fs_sheet (fs_sheet_id_counter_++);
      add_fs_sheet (S);
      S->add_FF_to_S (FF);
      //Propagate to all adjacent patch-elms within the same sheet component.
      propagate_label_S (S, FF);
    }
  }

  vul_printf (vcl_cout, "  Totally %d sheet-components built.\n", fs_sheet_id_counter_);
}

//: Propagate the labelling of sheet component from the inputFF.
//  Repeat labelling neighbors until a bounary link type != E_TYPE_MANIFOLD is found.
//
void dbsk3d_fs_sheet_set::propagate_label_S (dbsk3d_fs_sheet* S, dbsk3d_fs_face* inputFF)
{
  //Initially sheet-component S should contain inputFF.
  unsigned int nP = S->n_FFs (); 
  assert (nP == 1);
  assert (inputFF->sid() > FF_UNVISITED_SID);
  vcl_queue<dbsk3d_fs_face*> FF_queue;  

  //Put all valid neighboring unvisited patch-elms of inputFF to the FF_queue.
  put_neighbor_FF_to_Q (S, inputFF, FF_queue);

  //Loop of sheet_comp propagation.
  while (FF_queue.size() > 0) {
    //Pop one patch-elm from FF_queue and propagate it. 
    dbsk3d_fs_face* FF = FF_queue.front();
    FF_queue.pop();
    
    if (FF->sid() > FF_UNVISITED_SID)
      continue; //Skip the already visited patch-elm in queue.

    //Visit FF and add FF to sheet component S.
    S->add_FF_to_S (FF);
    nP++;

    //Put all valid neighboring unvisited patch-elms of FF to the FF_queue.
    put_neighbor_FF_to_Q (S, FF, FF_queue);
  }

  //Determine sheet type: all shock tabs are labelled FS_TYPE_TAB already.
  if (S->type() != FS_TYPE_TAB && S->type() != FS_TYPE_UNBOUNDED) {
    S->set_type (FS_TYPE_INTERIOR);
    S->set_cost (FS_COST_UNDEFINED);
  }

  assert (nP == S->n_FFs());

  #if DBMSH3D_DEBUG>2
  vul_printf (vcl_cout, "S %d type %s has %d patch-elms.\n", 
               S->id(), S->type_string().c_str(), nP);
  #endif
}

//: Put all valid neighboring unvisited patch-elms of FF to the FF_queue.
//  Also, if any shock rib is found during the exploration, set S's type to FS_TYPE_TAB.
//
void dbsk3d_fs_sheet_set::put_neighbor_FF_to_Q (dbsk3d_fs_sheet* S, dbsk3d_fs_face* FF, 
                                                vcl_queue<dbsk3d_fs_face*>& FF_queue)
{
  assert (FF->b_valid() && FF->sid() >= 0);
  //Go through each adjacent shock link elements.  
  dbmsh3d_halfedge* curHG = FF->halfedge();
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) curHG->edge();
    unsigned int n = FE->count_valid_Fs();

    //If FE is E_TYPE_RIB, set S->type_ to be FS_TYPE_TAB.
    if (n == 1 && S->type() != FS_TYPE_UNBOUNDED)
      S->set_type (FS_TYPE_TAB);
    else if (n == 2) { //If FE is E_TYPE_MANIFOLD && other-patch-elm not visited.
        //Add the other unvisited patch-elm into queue.
        dbsk3d_fs_face* FFo = (dbsk3d_fs_face*) FE->other_valid_F (FF);
        if (FFo->sid() == FF_UNVISITED_SID)
          FF_queue.push (FFo);
    }

    curHG = curHG->next();
  }
  while (curHG != FF->halfedge());
}

//####################### Sheet Component Operations #######################

//Prune S and mark all S's patch-elm invalid.
void dbsk3d_fs_sheet_set::remove_del_fs_sheet (dbsk3d_fs_sheet* S)
{  
  S->set_all_FFs_invalid (); //Mark all patch-elms invalid.
  remove_fs_sheet (S);      //Remove S from the sheetmap.
  delete S;
}

//: Merge the two input sheets S1 and S2 together
void dbsk3d_fs_sheet_set::merge_fs_sheets (dbsk3d_fs_sheet* S1, dbsk3d_fs_sheet* S2)
{ 
  //Add all sheet elements of S2 to S1
  for (dbmsh3d_ptr_node* cur = S2->FF_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
    S1->add_FF_to_S (FF);
  }

  //If S2 is a tab (with A3 boundary), the merged S1 is a tab.
  //If S2 is unbounded, the merged S1 is also unbounded.
  if (S2->type() == FS_TYPE_TAB && S1->type() == FS_TYPE_INTERIOR)
    S1->set_type (FS_TYPE_TAB);

  //Delete S2
  remove_fs_sheet (S2);
  delete S2;
  
  if (S1->type() == FS_TYPE_TAB || S1->type() == FS_TYPE_UNBOUNDED)
    S1->compute_splice_cost ();
}

//: Loop through all sheets and remove the empty ones.
unsigned int dbsk3d_fs_sheet_set::remove_empty_sheets ()
{
  unsigned int n_empty_sheets_removed = 0;
  vcl_map<int, dbsk3d_fs_sheet*>::iterator it = sheetmap_.begin();
  while (it != sheetmap_.end()) {
    dbsk3d_fs_sheet* S = (*it).second;

    if (S->have_FFs()) {
      it++;
    }
    else { //Remove S from the sheet set.
      n_empty_sheets_removed++;
      if (it == sheetmap_.begin()) {
        sheetmap_.erase (it);
        delete S;
        it = sheetmap_.begin();
      }
      else {
        vcl_map<int, dbsk3d_fs_sheet*>::iterator prev = it;
        prev--;
        sheetmap_.erase (it);
        delete S;
        it = prev++;
      }
    }
  }
  return n_empty_sheets_removed;
}

void dbsk3d_fs_sheet_set::remove_invalid_FF ()
{
  vul_printf (vcl_cout, "  remove_invalid_FF(): %u sheets,", sheetmap_.size());
  if (sheetmap_.size() == 0)
    return;

  vcl_map<int, dbsk3d_fs_sheet*>::iterator sit = sheetmap_.begin();
  while (sit != sheetmap_.end()) {
    dbsk3d_fs_sheet* S = (*sit).second;

    //Check each FF of S
    dbmsh3d_ptr_node* prev = NULL;
    dbmsh3d_ptr_node* cur = S->FF_list(); 
    while (cur != NULL) {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
      if (FF->b_valid() == false) { //erase FF from S
        if (prev == NULL) { //cur is list head.
          dbmsh3d_ptr_node* tmp = cur;
          cur = cur->next();
          S->set_FF_list (cur);
          delete tmp;
        }
        else { //cur is in middle of list.
          dbmsh3d_ptr_node* tmp = cur;
          cur = cur->next();
          prev->set_next (cur);
          delete tmp;
        }
      }
      else { //visit next.
        prev = cur;
        cur = cur->next();
      }
    }

    if (S->have_FFs() == false) { //delete S from map
      sit++;
      sheetmap_.erase (S->id());
    }
    else
      sit++;
  }

  vul_printf (vcl_cout, " remaining %u sheets.\n", sheetmap_.size());
}

bool dbsk3d_fs_sheet_set::check_integrity ()
{
  vcl_map<int, dbsk3d_fs_sheet*>::iterator sit = sheetmap_.begin();
  for (; sit != sheetmap_.end(); sit++) {
    dbsk3d_fs_sheet* S = (*sit).second;

    if (S->type() ==  FS_TYPE_TAB) {
      if (S->cost() < 0) {
        assert (0);
        return false;
      }
    }
    else if (S->type() ==  FS_TYPE_INTERIOR) {
      if (S->cost() != FS_COST_UNDEFINED) {
        assert (0);
        return false;
      }
    }
    else if (S->type() == FS_TYPE_UNBOUNDED) {
      if (S->cost() < 0) {
        assert (0);
        return false;
      }
    }
    else {
      assert (0);
      return false;
    }
  }

  return true;
}




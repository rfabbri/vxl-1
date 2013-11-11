// This is dbsk3d_fs_comp_set.cxx
//: MingChing Chang
//  Dec 08, 2006        Creation

#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <dbsk3d/dbsk3d_fs_comp_set.h>

//####################### Shock Component Extraction #######################

unsigned int dbsk3d_fs_comp_set::label_shock_components ()
{
  vul_printf (vcl_cout, "\nlabel_shock_components(): totally %u sheets.\n", 
              fs_ss_->sheetmap().size());
  fs_comp_id_counter_ = 0;

  //Assume all fs_sheets are initially unvisited.
  //Reset all fs_sheet's fs_face FF.sid.
  vcl_map<int, dbsk3d_fs_sheet*>::iterator it = fs_ss_->sheetmap().begin();
  for (; it != fs_ss_->sheetmap().end(); it++) {
    dbsk3d_fs_sheet* S = (*it).second;
    assert (S->b_visited() == false);

    S->set_all_FFs_sid ();
  }

  //Go through all sheets and label shock components.
  it = fs_ss_->sheetmap().begin();
  for (; it != fs_ss_->sheetmap().end(); it++) {
    dbsk3d_fs_sheet* S = (*it).second;

    //Create a new shock component C if S is not labelled before.
    if (S->b_visited() == false) {       
      dbsk3d_fs_comp* C = new dbsk3d_fs_comp (fs_comp_id_counter_++);
      add_fs_comp (C);
      S->set_visited (true);
      C->add_sheet (S);
      //Propagate to all incident sheets within the same shock component.
      propagate_label_C (C, S);
    }
  }

  vul_printf (vcl_cout, "  Totally %d shock-components built.\n", fs_comp_id_counter_);
  return fs_comp_id_counter_;
}

//: Propagate the labelling of shock component C from the inputS until finished.
//
void dbsk3d_fs_comp_set::propagate_label_C (dbsk3d_fs_comp* C, dbsk3d_fs_sheet* inputS)
{  
  unsigned int nS = 1; //Initially shock component C should contain inputS.
  assert (inputS->b_visited());
  vcl_queue<dbsk3d_fs_sheet*> sheet_queue;  

  //Put all incident unvisited sheets of inputS to the sheet_queue.
  put_neighbor_S_to_queue (inputS, sheet_queue);
  
  while (sheet_queue.size() > 0) { //Loop of shock component propagation.
    //Pop one sheet S from sheet_queue and propagate it. 
    dbsk3d_fs_sheet* S = sheet_queue.front();
    sheet_queue.pop();
    
    if (S->b_visited())
      continue; //Skip the already visited S in queue.

    //Visit S and add S to shock component C.
    S->set_visited (true);
    C->add_sheet (S);
    nS++;

    //Put all incident unvisited sheets of S to the sheet_queue.
    put_neighbor_S_to_queue (S, sheet_queue);
  }

  #if DBMSH3D_DEBUG>2
  vul_printf (vcl_cout, "component C %d has %d sheets.\n", C->id(), nS);
  #endif
}

//: In labelling shock component C,
//  put all incident unvisited sheets of inputS to the sheet_queue, i.e.,
//  looking for neighboring sheets across A13 (or dege. A1n) shock links.
//
void dbsk3d_fs_comp_set::put_neighbor_S_to_queue (dbsk3d_fs_sheet* inputS, 
                                                  vcl_queue<dbsk3d_fs_sheet*>& sheet_queue)
{
  //Go through each shock-patch-elm FF of inputS.
  for (dbmsh3d_ptr_node* cur = inputS->FF_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
  
    //Look for FF's A13 or A1n fs_edges.  
    dbmsh3d_halfedge* curHG = FF->halfedge();
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) curHG->edge();
      unsigned int n = FE->count_valid_Fs();

      if (n > 2) {
        //Look for FE's each incident valid shock-patch-elm Po on the other shock sheet.
        dbmsh3d_halfedge* he = FE->halfedge();
        do {
          dbsk3d_fs_face* Po = (dbsk3d_fs_face*) he->face();
          if (Po != FF && Po->b_valid() && Po->sid() != inputS->id()) {
            //Add Po's shock sheet So to the sheet_queue, if So not visited.
            dbsk3d_fs_sheet* So = fs_ss_->sheetmap (Po->sid());
            if (So->b_visited() == false)
              sheet_queue.push (So);
          }          
          he = he->pair();
        }
        while (he != FE->halfedge() && he != NULL);
      }
      curHG = curHG->next();
    }
    while (curHG != FF->halfedge());
  }
}

//####################### Sheet Component Operations #######################

//: Return whether first element is greater than the second
//  Sorts in DESCENDING order.
bool fs_comp_greater (const dbsk3d_fs_comp* C1, const dbsk3d_fs_comp* C2)
{
  return C1->cost() > C2->cost();
}

#define N_SHOCK_COMP_TO_PRINT 20

void dbsk3d_fs_comp_set::sort_shock_components ()
{
  vul_printf (vcl_cout, "sort_shock_components(): total %u fs_comps.\n", comp_list_.size());  

  //Compute the cost (size) of each shock component
  for (unsigned int i=0; i<comp_list_.size(); i++) {
    dbsk3d_fs_comp* C = comp_list_[i];
    C->compute_cost ();
    if (i<N_SHOCK_COMP_TO_PRINT)
      vul_printf (vcl_cout, "\t%2d-th component (id %2d) has %d assigned genes\n", 
                   i, C->id(), C->cost());
  }

  //Sort the components according to their costs in descending order.
  vcl_sort (comp_list_.begin(), comp_list_.end(), fs_comp_greater);
  vul_printf (vcl_cout, "After Sorting...\n");

  for (unsigned int i=0; i<comp_list_.size(); i++) {
    dbsk3d_fs_comp* C = comp_list_[i];
    C->set_id (i); //Reset C's id to be ordered.
    if (i<N_SHOCK_COMP_TO_PRINT)
      vul_printf (vcl_cout, "\t%2d-th Component (id %2d) has %d assigned genes\n", 
                   i, C->id(), C->cost());
  }

  #if DBMSH3D_DEBUG >= 1
  vul_printf (vcl_cout, "\n List of shock sheets of all shock components...\n");
  //Print all fs_sheets of each component.
  for (unsigned int i=0; i<comp_list_.size(); i++) {
    dbsk3d_fs_comp* C = comp_list_[i];
    vul_printf (vcl_cout, "\t%2d-th Component (id %2d) has %u fs_sheets: ", 
                i, C->id(), C->fs_sheets().size());
    for (unsigned int j=0; j<C->fs_sheets().size(); j++) {
      dbsk3d_fs_sheet* S = C->fs_sheets(j);
      vul_printf (vcl_cout, "%d ", S->id());
    }
    vul_printf (vcl_cout, "\n");
  }
  #endif
  vul_printf (vcl_cout, "\n");
}

//: Detele all shock elements and sheets for unspecified component.
//  Loop through comp_list_ and 
//    - clear all unspecified fs_comps and set their entry in comp_list_ to NULL.
//    - delete all unspecified shock sheets in fs_ss_[]
//    - mark all shock elements of the unspecified component to be invalid.
//  Delete all invalid patch, link, and node elements of fs_mesh.
//: return remaining shock-sheet-elements.
unsigned int dbsk3d_fs_comp_set::delete_unspecified_comps (vcl_set<int>& ith_comp_list)
{
  if (comp_list_.size() == 0)
    return 0;

  if (ith_comp_list.find (-1) != ith_comp_list.end()) {
    //Keep all components.
    for (unsigned int i=0; i<comp_list_.size(); i++) {
      dbsk3d_fs_comp* C = comp_list_[i];
      //Mark all shock elements of C to be valid.
      C->mark_all_FFs_valid ();
    }
  }
  else {
    for (unsigned int i=0; i<comp_list_.size(); i++) {
      dbsk3d_fs_comp* C = comp_list_[i];
      //Mark all fs_faces of the ith_comp to be valid.
      if (ith_comp_list.find (i) != ith_comp_list.end())
        C->mark_all_FFs_valid();
      else //Mark unspecified components' fs_faces to be invalid.
        C->mark_all_FFs_invalid ();        
    }

    for (unsigned int i=0; i<comp_list_.size(); i++) {
      dbsk3d_fs_comp* C = comp_list_[i];
      if (ith_comp_list.find (i) == ith_comp_list.end()) {
        //Delete all shock sheets in C from fs_ss_
        vcl_vector<dbsk3d_fs_sheet*>::iterator it = C->fs_sheets().begin();
        for (; it != C->fs_sheets().end(); it++) {
          dbsk3d_fs_sheet* S = (*it);
          fs_ss_->remove_fs_sheet (S);
          delete S;
        }

        //Clear the entry of C in comp_list_ and delete C.
        comp_list_[i] = NULL;
        delete C;
      }
    }
  }

  //Delete all invalid patch, link, and node elements of fs_mesh.
  return fs_ss()->fs_mesh()->del_invalid_FFs_complete ();
}





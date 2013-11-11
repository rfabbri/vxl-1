//: This is dbsk3d/algo/fs_regul.cxx
//  Dec 1, 2006.
//  MingChing Chang

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbsk3d/algo/dbsk3d_fs_regul.h>
#include <dbsk3d/algo/dbsk3d_fs_algo.h>
#include <dbsk3d/algo/dbsk3d_fs_xform.h>

//: Trim out valid A12-2 fs_faces which the A12-2 time < th.  
bool dbsk3d_fs_regul::trim_bnd_A122_FFs (const float rmin_ratio)
{
  const double rmin_th = rmin_ratio * fs_ss_->fs_mesh()->median_A122_dist();    
  vul_printf (vcl_cout, "\ntrim_bnd_A122_FFs(): rmin_th = %f (rmin_ratio = %.2f),\n",
              rmin_th, rmin_ratio);

  vul_printf (vcl_cout, "  Splice xform on valid A12-2 fs_faces (Gabriel edges): ");

  //Find out all boundary fs_faces with Gabriel edges > rmin_th.
  //Prune them and pass genes. to adjacent FF's.
  unsigned int count = 0;
  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_ss_->fs_mesh()->facemap().begin();
  for (; pit != fs_ss_->fs_mesh()->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    if (FF->b_valid() == false)
      continue;

    vcl_vector<dbmsh3d_vertex*> vertices;
    FF->get_bnd_Vs (vertices);
    if (FF->contain_A12_2 (vertices) == false)
      continue; //Skip all that not containing A13-2 pts.

    if (FF->is_on_shock_bnd() == false)
      continue; //Skip all interior fs_faces.

    //fs_face FF is of type A12-2.
    #if DBMSH3D_DEBUG>2
    vul_printf (vcl_cout, "%d ", FF->id());
    #endif
    count++;

    //Prune FF and pass genes. to adjacent fs_faces.
    FF_prune_pass_Gs (FF);
  }

  vul_printf (vcl_cout, "\n    Totally %u A12-2 fs_faces splice-transformed (pruned).\n", count);

  return count == 0;
}

//: Perform 3D Shock Regularization on the fine-medial-scaffold.
//  Run 3d splice transform on all shock sheets whose cost < cost_th.
//  Cost = nFF * deltaA.
void dbsk3d_fs_regul::run_shock_regul (const float reg_th, const bool reasgn_lost_genes)
{
  vul_printf (vcl_cout, "\nrun_shock_regul(): reg_th = %f on %u shock sheets.", 
              reg_th, fs_ss_->sheetmap().size());

  //Initialize the sheet queue for greedy iteration.
  init_regul_sheet_queue (reg_th);

  //Iteratively perform 3D splice transforms on shock-tabs with small cost.
  greedy_shock_regul (reg_th);

  fs_mesh()->detect_valid_FE_type ();

  vul_printf (vcl_cout, "\n  After early regularization, remaining %u shock sheets\n", 
              fs_ss_->sheetmap().size());

  if (reasgn_lost_genes) {
    //Check the assignment of all generators.
    vcl_vector<dbmsh3d_vertex*> unasgn_genes;
    fs_mesh()->check_all_G_asgn (unasgn_genes); 

    if (unasgn_genes.size() != 0) {
      //Try to recover assignment of 'lost' genes.
      //kd-tree has mem-limit of around 1M points.
      bool result;
      if (fs_mesh()->vertexmap().size() < 1000000) 
        result = reasgn_lost_Gs_closest_FV (fs_mesh(), unasgn_genes);
      else
        result = reasgn_lost_Gs_via_FF (fs_mesh(), unasgn_genes);

      //Check the assignment of all generators again.
      if (result) {
        unasgn_genes.clear();
        bool result = fs_mesh()->check_all_G_asgn (unasgn_genes); 
        assert (result);
      }
    }
  }

  bool r = fs_ss_->check_integrity();
  if (r)
    vul_printf (vcl_cout, "\t  Check fs_ss integrity: success!\n\n");
  else
    vul_printf (vcl_cout, "\t  Check fs_ss integrity: fail!\n\n");
}

// #################################################################

//: Initialize the sheet queue for greedy regularization.
void dbsk3d_fs_regul::init_regul_sheet_queue (const float reg_th)
{
  vul_printf (vcl_cout, "\ninit_regul_sheet_queue(): reg_th %f.\n", reg_th);
  vul_printf (vcl_cout, "\ttotal shock sheets: %u\n", fs_ss_->sheetmap().size());

  //Put all shock-sheet-tabs with cost less then cost_th to the queue.
  unsigned int n_S_interior = 0;
  unsigned int n_S_unbounded = 0;

  vcl_map<int, dbsk3d_fs_sheet*>::iterator it = fs_ss_->sheetmap().begin();
  for (; it != fs_ss_->sheetmap().end(); it++) {
    dbsk3d_fs_sheet* S = (*it).second;

    if (S->type() == FS_TYPE_INTERIOR) {
      assert (S->cost() == FS_COST_UNDEFINED);
      n_S_interior++;
    }
    else {  //Put UNBOUNDED and TAB shock sheets into the queue.
      if (S->type() == FS_TYPE_UNBOUNDED)
        n_S_unbounded++;
      else
        assert (S->type() == FS_TYPE_TAB);

      S->compute_splice_cost ();
      if (S->cost() <= reg_th)
        _add_FS_to_Q (S);
    }
  }

  vul_printf (vcl_cout, "\t# interior shock sheets: %u\n", n_S_interior);
  vul_printf (vcl_cout, "\t# unbounded outside shock sheets: %u\n", n_S_unbounded);
  vul_printf (vcl_cout, "\t# fs_sheets in queue to be regularized: %u\n", fs_sheet_queue_.size());
}

void dbsk3d_fs_regul::greedy_shock_regul (const float reg_th)
{
  vul_printf (vcl_cout, "\nrun_greedy_regularization(): queue size %u.\n", fs_sheet_queue_.size());

  unsigned int n_splice_xform = 0;  
  vcl_multimap<float, dbsk3d_fs_sheet*>::iterator it = fs_sheet_queue_.begin();
  while (it != fs_sheet_queue_.end()) {    
    dbsk3d_fs_sheet* S = (*it).second;
    fs_sheet_queue_.erase (it); //Remove link from queue.

    //Perform 3D Splice Transform on shock sheet S.
    assert (S->id() >= 0);
    assert (S->cost() >= 0);

    //Go through each shock patch FF and each A13-shock-link FE of S, 
    //and check if S is valid for a splice transform.
    //Make a list of sheets to splice (S with smaller id in the front).
    vcl_list<vcl_pair<int, int> > S_to_splice;
    vcl_vector<dbsk3d_fs_edge*> C_Lset;
    if (S_valid_splice_xform (S, S_to_splice, C_Lset)) {
      S_3d_splice_xform (S, S_to_splice, C_Lset, reg_th);
      n_splice_xform++;
    }

    it = fs_sheet_queue_.begin(); //For the next iteration.
  }

  vul_printf (vcl_cout, "  %u early greedy splice xforms on fs_sheets performed.\n\n", n_splice_xform);
}

// #################################################################

//: 3D Splice Transform on a fs_sheet.
//  Assumption: S has at least one boundary A3-shock-link.
//  On the shock sheet: remove S and splice adjacent shock sheets.
//    - for each adjacent A13 shock-link FE, splice the other two shock sheets.
//  On the boundary:
//    - detect corresponding boundary region to change.
//    - remove old boundary region.
//    - create new boundary surface to fill the hole.


//  This function is incomplete since it rejects a suitable S 
//  that possibly contains self-A5 swallow tail.

void dbsk3d_fs_regul::S_3d_splice_xform (dbsk3d_fs_sheet* S, 
                                         vcl_list<vcl_pair<int, int> >& S_to_splice,
                                         vcl_vector<dbsk3d_fs_edge*>& C_Lset,
                                         const float reg_th)
{  
  if (C_Lset.size() == 0) {
    //Skip passing generators for unbounded sheets that has no incident A13 curves  
    //Just remove the assigned genes from all incident FE's and N's of S.
    S->clear_incident_LN_asgn_Gs();
  }
  else {
    //Pass all S'generators of S to the remaining fs_edges.
    S->S_pass_Gs (C_Lset);
  }

  //Remove S and mark all S's fs_face invalid.
  fs_ss_->remove_del_fs_sheet (S);

  //Splicing each pair (S1, S2) of the S_to_splice list. 
  while (S_to_splice.size() != 0) {
    vcl_list<vcl_pair<int, int> >::iterator lit = S_to_splice.begin();
    int S1id = (*lit).first;
    int S2id = (*lit).second;
    dbsk3d_fs_sheet* S1 = fs_ss_->sheetmap (S1id);
    dbsk3d_fs_sheet* S2 = fs_ss_->sheetmap (S2id);
    S_to_splice.erase (lit);
    lit = S_to_splice.begin();

    if (S1id == S2id)
      continue; //Ignore if the two sheets are the same.

    //Splicing S1 and S2 together.
    #if DBMSH3D_DEBUG > 2
    vul_printf (vcl_cout, "\tS %d: splice sheets %d and %d together.\n", 
                S->id(), S1->id(), S2->id());
    #endif

    //Handle the case if S1 is in the queue.
    bool S1_in_queue = _remove_S_from_Q (S1); 

    //Handle the case if S2 is in the sheet_to_splice<int,int>
    //Replace all instance of S2id by S1id.
    replace_S2_in_list (S1id, S2id, S_to_splice);

    //Merge S2 to S1
    _remove_S_from_Q (S2); 
    fs_ss_->merge_fs_sheets (S1, S2);  
    
    if (S1_in_queue && S1->cost() <= reg_th)
      _add_FS_to_Q (S1);
  }
}



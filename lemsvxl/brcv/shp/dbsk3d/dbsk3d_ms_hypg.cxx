//: This is dbsk3d_ms_hypg.cxx
//  MingChing Chang
//  Oct 20, 2004.

#include <vcl_set.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/dbsk3d_sg_sa.h>


void dbsk3d_ms_hypg::add_virtual_curve (dbmsh3d_vertex* sV, dbmsh3d_vertex* eV,
                                        dbmsh3d_curve* sC, dbmsh3d_curve* eC)
{
  dbsk3d_ms_curve* MCV = (dbsk3d_ms_curve*) _new_edge (sV, eV);
  MCV->set_c_type (C_TYPE_VIRTUAL);
  MCV->sup_curves().push_back (sC);
  MCV->sup_curves().push_back (eC);
  MCV->setup_virtual_curve ();
  add_edge_incidence (MCV); //add the virtual curve into the hypg.
}

void dbsk3d_ms_hypg::add_virtual_curve (dbmsh3d_vertex* sV, dbmsh3d_vertex* eV,
                                        vcl_vector<dbmsh3d_curve*>& sup_curves)
{
  dbsk3d_ms_curve* MCV = (dbsk3d_ms_curve*) _new_edge (sV, eV);
  MCV->set_c_type (C_TYPE_VIRTUAL);
  MCV->sup_curves().insert (MCV->sup_curves().begin(), sup_curves.begin(), sup_curves.end());
  MCV->setup_virtual_curve ();
  add_edge_incidence (MCV); //add the virtual curve into the hypg.
}

//: Graph traversing uses dbsk3d_fs_mesh's flag
const unsigned int dbsk3d_ms_hypg::i_traverse_flag() const
{
  return fs_mesh()->i_traverse_flag();
}

void dbsk3d_ms_hypg::select_salient_ms_curves (const int min_elms, const int n_curves, const int verbose)
{
  if (verbose)
    vul_printf (vcl_cout, "select_salient_ms_curves():\n");

  //Go through each curve and compute length, add to a map.
  //All ms_curves should be sorted in MCmap in order of -len.
  vcl_map<float, dbsk3d_ms_curve*> MCmap;
  vcl_map<int, dbmsh3d_edge*>::iterator eit = edgemap_.begin();
  for (; eit != edgemap_.end(); eit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*eit).second;
    float len = MC->get_length();
    MCmap.insert (vcl_pair<float, dbsk3d_ms_curve*>(-len, MC));
  }

  //Go through MCmap and select ms_curves in order of decreasing length.
  n_selected_ms_curves_ = 0;
  vcl_map<float, dbsk3d_ms_curve*>::iterator MCit = MCmap.begin();
  for (; MCit != MCmap.end(); MCit++) {
    dbsk3d_ms_curve* MC = (*MCit).second;
    const unsigned int sz = MC->E_vec().size();
    if (sz > min_elms) {
      MC->set_selected (true);
      MC->s_MN()->set_selected (true);
      MC->e_MN()->set_selected (true);
      n_selected_ms_curves_++;
    }

    if (n_selected_ms_curves_ >= n_curves)
      break; //reach the limit.
  }

  n_selected_ms_nodes_ = 0;
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*vit).second;
    if (MN->selected())
      n_selected_ms_nodes_++;
  }

  if (verbose) {
    vul_printf (vcl_cout, "  totally %d ms_curves, %d selected.\n",
                edgemap_.size(), n_selected_ms_curves_);
    vul_printf (vcl_cout, "  totally %d ms_nodes, %d selected.\n",
                vertexmap_.size(), n_selected_ms_nodes_);
  }
}

void dbsk3d_ms_hypg::select_all ()
{
  vcl_map<int, dbmsh3d_edge*>::iterator eit = edgemap_.begin();
  for (; eit != edgemap_.end(); eit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*eit).second;
    MC->set_selected (true);
    MC->s_MN()->set_selected (true);
    MC->e_MN()->set_selected (true);
  }
}

//######################################################################

//Replicate dbmsh3d_hypg::check_integrity() with a proper shock element type.
bool dbsk3d_ms_hypg::check_integrity ()
{  
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*vit).second;
    if (MN->id() != (*vit).first) {
      assert (0);
      return false;
    }
    if (MN->check_integrity() == false) {
      assert (0);
      return false;
    }
  }
  
  //Prepare a multi-set of all E's in the ms_hypg.
  vcl_multiset<dbmsh3d_edge*> Emset;
  vcl_map<int, dbmsh3d_edge*>::iterator eit = edgemap_.begin();
  for (; eit != edgemap_.end(); eit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*eit).second;
    if (MC->c_type() != C_TYPE_VIRTUAL) {
      for (unsigned int i=0; i<MC->E_vec().size(); i++) {
        dbmsh3d_edge* E = MC->E_vec(i);
        Emset.insert (E);
      }
    }

    if (MC->id() != (*eit).first) {
      assert (0);
      return false;
    }
    if (MC->check_integrity() == false) {
      assert (0);
      return false;
    }
  }
  unsigned int Emset_size = Emset.size();
  
  //Prepare a set of all F's in the ms_hypg.
  vcl_set<dbmsh3d_face*> Fmset;
  vcl_map<int, dbmsh3d_sheet*>::iterator sit = sheetmap_.begin();
  for (; sit != sheetmap_.end(); sit++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*sit).second;

    vcl_map<int, dbmsh3d_face*>::iterator it = MS->facemap().begin();
    for (; it != MS->facemap().end(); it++) {
      dbmsh3d_face* F = (*it).second;
      Fmset.insert (F);
    }

    if (MS->id() != (*sit).first) {
      assert (0);
      return false;
    }
    if (MS->check_integrity() == false) {
      assert (0);
      return false;
    }
  }
  unsigned int Fmset_size = Fmset.size();

  //Validate that E's not in shared_E[] are not used by other ms_curve components.
  eit = edgemap_.begin();
  for (; eit != edgemap_.end(); eit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*eit).second;
    if (MC->data_type() != C_DATA_TYPE_EDGE)
      continue;
    if (MC->c_type() == C_TYPE_VIRTUAL)
      continue; //skip the virtual curve.

    //Remove MC.Evec from Emset.
    for (unsigned int i=0; i<MC->E_vec().size(); i++) {
      dbmsh3d_edge* E = MC->E_vec(i);
      Emset.erase (Emset.find (E));
    }

    //Check that all MC's non-shared E are not in Emset.
    for (unsigned int i=0; i<MC->E_vec().size(); i++) {
      dbmsh3d_edge* E = MC->E_vec(i);
      if (MC->is_E_shared (E))
        continue;

      if (Emset.find (E) != Emset.end()) {
        assert (0);
        return false;
      }
    }

    //Add MC.Evec back to Emset.
    for (unsigned int i=0; i<MC->E_vec().size(); i++) {
      dbmsh3d_edge* E = MC->E_vec(i);
      Emset.insert (E);
    }
    assert (Emset_size == Emset.size());
  }

  //Validate that F' not in shared_F[] are not used by other ms_sheet components.
  sit = sheetmap_.begin();
  for (; sit != sheetmap_.end(); sit++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*sit).second;

    //Remove MS.Fmap from Fmset.
    vcl_map<int, dbmsh3d_face*>::iterator it = MS->facemap().begin();
    for (; it != MS->facemap().end(); it++) {
      dbmsh3d_face* F = (*it).second;
      Fmset.erase (Fmset.find(F));
    }

    //Check that all MS's non-shared F are not in Fmset.
    it = MS->facemap().begin();
    for (; it != MS->facemap().end(); it++) {
      dbmsh3d_face* F = (*it).second;
      if (MS->is_F_shared (F))
        continue;

      if (Fmset.find (F) != Fmset.end()) {
        assert (0);
        return false;
      }
    }

    //Add MS.Fmap back to Fmset.
    it = MS->facemap().begin();
    for (; it != MS->facemap().end(); it++) {
      dbmsh3d_face* F = (*it).second;
      Fmset.insert (F);
    }
    assert (Fmset_size == Fmset.size());
  }

  //assert that all fs_faces are valid.
  if (fs_mesh()->check_all_FFs_valid() == false) {
    assert (0);
    return false;
  }

  return true;
}

dbsk3d_ms_hypg* dbsk3d_ms_hypg::clone (dbsk3d_fs_mesh* FS2)
{
  dbsk3d_ms_hypg* MHG2 = new dbsk3d_ms_hypg (FS2);

  //deep-copy all medial nodes.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*vit).second;
    dbsk3d_ms_node* MN2 = (dbsk3d_ms_node*) MN->clone (FS2);
    MHG2->_add_vertex (MN2);
  }
  MHG2->set_vertex_id_counter (vertex_id_counter_);
  assert (MHG2->vertexmap().size() == vertexmap_.size());

  //deep-copy all medial curves.
  vcl_map<int, dbmsh3d_edge*>::iterator eit = edgemap_.begin();
  for (; eit != edgemap_.end(); eit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*eit).second;
    dbsk3d_ms_curve* MC2 = (dbsk3d_ms_curve*) MC->clone (MHG2, FS2);
    MHG2->_add_edge (MC2);
  }
  MHG2->set_edge_id_counter (edge_id_counter_);
  assert (MHG2->edgemap().size() == edgemap_.size());

  //deep-copy all medial sheets.
  vcl_map<int, dbmsh3d_sheet*>::iterator sit = sheetmap_.begin();
  for (; sit != sheetmap_.end(); sit++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*sit).second;
    dbsk3d_ms_sheet* MS2 = (dbsk3d_ms_sheet*) MS->clone (MHG2, FS2);
    MHG2->_add_sheet (MS2);
  }
  MHG2->set_sheet_id_counter (sheet_id_counter_);
  assert (MHG2->sheetmap().size() == sheetmap_.size());

  return MHG2;
}

dbmsh3d_pt_set* dbsk3d_ms_hypg::clone ()
{
  //deep-copy fs_mesh and bnd_mesh.
  dbsk3d_fs_mesh* FS2 = (dbsk3d_fs_mesh*) fs_mesh()->clone ();
  dbsk3d_ms_hypg* MHG2 = clone (FS2);
  return MHG2;
}

void dbsk3d_ms_hypg::print_MS_info ()
{
  vul_printf (vcl_cout, "\nprint_MS_info() on %d MS nodes.\n", vertexmap_.size());
  int n_A1A3 = 0, n_A1A3_d = 0;
  int n_A14 = 0, n_A14_d = 0;
  int n_A1A3_2 = 0, n_A1A3_2_d = 0;

  int n_A12A3 = 0, n_A12A3_d = 0;
  int n_A15 = 0, n_A15_d = 0;
  int n_A1A5 = 0, n_A1A5_d = 0;

  vcl_set<vcl_pair<int, int> > A1nA3_set; //n>=2, storing <n_rib, n_axial>.
  vcl_set<vcl_pair<int, int> > A1nA3_d_set; 
  vcl_set<int> A1n_set; //n>=5, storing n_axial.
  vcl_set<int> A1n_d_set;
  
  int n_LoopNode = 0;
  int n_LoopNode_2 = 0;
  int nN_degeA_2p = 0; //# nodes with 2 or more dege. axials.
  int exception1 = 0, exception2 = 0, exception3 = 0;

  vcl_map<int, dbmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*vit).second;
    int nRib, nAxial, nDegeAxial, nVirtual;
    int nLoop = MN->count_incident_Cs (nRib, nAxial, nDegeAxial, nVirtual);
    if (nLoop) {
      n_LoopNode++;
      continue;
    }

    //Determine type.
    if (nDegeAxial == 0) {
      if (nRib == 1 && nAxial == 1) //A1A3
        n_A1A3++;
      else if (nRib == 0 && nAxial == 4) //A14
        n_A14++;
      else if (nRib == 2 && nAxial == 2) //A1A3 node in the A1A3-II case.
        n_A1A3_2++;
      else if (nRib == 1 && nAxial == 3) //A12A3
        n_A12A3++;
      else if (nRib == 0 && nAxial == 6) //A15
        n_A15++;
      else if (nRib == 3 && nAxial == 1) //A1A5
        n_A1A5++;
      else {
        if (nAxial != 0) {
          if (nRib != 0) //A1nA3, n>=2
            A1nA3_set.insert (vcl_pair<int, int> (nRib, nAxial));
          else
            A1n_set.insert (nAxial);      
        }
        else {
          if (nRib >=2 && nRib % 2 == 0) { //intersection of two or more loops.
            n_LoopNode_2++;
          }
          else {
            vul_printf (vcl_cout, "exception1!(%d) ", MN->id());
            exception1++;
            assert (0);
          }
        }
      }
    }
    else if (nDegeAxial == 1) {      
      if (nRib == 1 && nAxial + nDegeAxial == 1) //A1A3_d
        n_A1A3_d++;
      else if (nRib == 0 && nAxial + nDegeAxial == 4) //A14_d
        n_A14_d++;
      else if (nRib == 2 && nAxial + nDegeAxial == 2) //A1A3_d node in the A1A3-II case.
        n_A1A3_2_d++;
      else if (nRib == 1 && nAxial + nDegeAxial == 3) //A12A3_d
        n_A12A3_d++;
      else if (nRib == 0 && nAxial + nDegeAxial == 6) //A15_d
        n_A15_d++;
      else if (nRib == 3 && nAxial + nDegeAxial == 1) //A1A5_d
        n_A1A5_d++;
      else {
        if (nAxial + nDegeAxial != 0) {
          if (nRib != 0) //A1nA3, n>=2
            A1nA3_d_set.insert (vcl_pair<int, int> (nRib, nAxial + nDegeAxial));
          else
            A1n_d_set.insert (nAxial + nDegeAxial);      
        }
        else {
          if (nRib >=2 && nRib % 2 == 0) { //intersection of two or more loops.
            n_LoopNode_2++;
          }
          else {
            vul_printf (vcl_cout, "exception2!(%d) ", MN->id());
            exception2++;
            assert (0);
          }
        }
      }
    }
    else {
      nN_degeA_2p++; 
      vul_printf (vcl_cout, "exception3!(%d) ", MN->id());
      exception3++;
    }
  }
  
  //Print statistics of node types:  
  vul_printf (vcl_cout, "\n");
  vul_printf (vcl_cout, "  n_A1A3      = %d, n_A1A3_d    = %d.\n", n_A1A3, n_A1A3_d);
  vul_printf (vcl_cout, "  n_A14       = %d, n_A14_d     = %d.\n", n_A14, n_A14_d);
  vul_printf (vcl_cout, "  n_A1A3_2    = %d, n_A1A3_2_d  = %d.\n", n_A1A3_2, n_A1A3_2_d);
  vul_printf (vcl_cout, "  n_A12A3     = %d, n_A12A3_d   = %d.\n", n_A12A3, n_A12A3_d);
  vul_printf (vcl_cout, "  n_A15       = %d, n_A15_d     = %d.\n", n_A15, n_A15_d);
  vul_printf (vcl_cout, "  n_A1A5      = %d, n_A1A5_d    = %d.\n", n_A1A5, n_A1A5_d);
  
  vul_printf (vcl_cout, "  n_LoopNode  = %d, n_LoopNode2 = %d.\n", n_LoopNode, n_LoopNode_2);
  vul_printf (vcl_cout, "  nN_degeA_2p = %d.\n", nN_degeA_2p);

  vul_printf (vcl_cout, "  A1nA3_set   = %d.", A1nA3_set.size());
  if (A1nA3_set.size())
    vul_printf (vcl_cout, " (n_rib, n_axial): ");
  vcl_set<vcl_pair<int, int> >::iterator iit = A1nA3_set.begin();
  for (; iit != A1nA3_set.end(); iit++) {
    vul_printf (vcl_cout, "(%d, %d), ", (*iit).first, (*iit).second);
  }
  vul_printf (vcl_cout, "\n");

  vul_printf (vcl_cout, "  A1nA3_d_set = %d.", A1nA3_d_set.size());
  if (A1nA3_d_set.size())
    vul_printf (vcl_cout, " (n_rib, n_axial, n_dege): ");
  iit = A1nA3_d_set.begin();
  for (; iit != A1nA3_d_set.end(); iit++) {
    vul_printf (vcl_cout, "(%d, %d, 1), ", (*iit).first, (*iit).second-1);
  }
  vul_printf (vcl_cout, "\n");

  vul_printf (vcl_cout, "  A1n_set     = %d.", A1n_set.size());
  if (A1n_set.size())
    vul_printf (vcl_cout, " (n_axial): ");   
  vcl_set<int>::iterator it = A1n_set.begin();
  for (; it != A1n_set.end(); it++) {
    vul_printf (vcl_cout, "(%d), ", (*it));
  }
  vul_printf (vcl_cout, "\n");

  vul_printf (vcl_cout, "  A1n_d_set   = %d.", A1n_d_set.size());
  if (A1n_d_set.size())
    vul_printf (vcl_cout, " (b_rib, n_axial, n_dege): ");
  it = A1n_d_set.begin();
  for (; it != A1n_d_set.end(); it++) {
    vul_printf (vcl_cout, "(0, %d, 1), ", (*it)-1);
  }
  vul_printf (vcl_cout, "\n");

  vul_printf (vcl_cout, "  exception1  = %d, exception2  = %d, exception3  = %d.\n", 
              exception1, exception2, exception3);
}










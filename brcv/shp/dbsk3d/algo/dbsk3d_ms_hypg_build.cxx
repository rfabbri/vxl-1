//: This is dbsk3d_build_ms_hypg.cxx
//  MingChing Chang
//  Oct 20, 2004.

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbsk3d/algo/dbsk3d_ms_hypg_build.h>

//: Construct the Medial Scaffold Hypergraph from the remaining shock components.
dbsk3d_ms_hypg* dbsk3d_ms_hypg_build::build_ms_hypg (const int MS_topo_opt)
{
  vul_printf (vcl_cout, "\nbuild_ms_hypg()\n");

  if (ms_hypg_->vertexmap().size() != 0)
    ms_hypg_->clear();

  //To speed-up the construction of the ms_hypg, the coarse-scale element's id
  //is cached in their fine-scale elements, e.g.:
  //  MN.id in FV->i_value_,
  //  MC.id in FE->s_value_ (max. number of allowable shock curve is 65535).
  //The values are set in MN.set_V() and MC.add_E_to_back()
  //So a brute-force searching from fine-scale elm to coarse-scale elm is prevented.
  //Note: don't use FV->i_value_ in traversing the hypergraph/mesh.

  //Initialize all valid FV's vid_ to -1.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = fs_mesh()->vertexmap().begin();
  for (; vit != fs_mesh()->vertexmap().end(); vit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*vit).second;
    FV->set_vid (-1);
  }

  //Initialize all valid fs_edge's s_value_ to -1.
  vcl_map<int, dbmsh3d_edge*>::iterator eit = fs_mesh()->edgemap().begin();
  for (; eit != fs_mesh()->edgemap().end(); eit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*eit).second;
    FE->set_s_value (-1);
  }

  build_ms_nodes ();
  
  fs_mesh()->reset_traverse_e ();

  build_ms_curves ();
  build_ms_loops ();
  
  fs_mesh()->reset_traverse_f ();

  build_ms_sheets (MS_topo_opt);

  if (fs_comp_set_->n_total_fs_sheets() != 0) {
    //If the MS is built from the component data structure in the computation pipeline
    //(not loading from file, where the component structure is NOT used).
    //Print all valid ms_sheets built in the coarse-scale MS hypergraph.
    vul_printf (vcl_cout, "%u ms_sheets <-> %u fs_sheets (in %d components).\n", 
                ms_hypg_->sheetmap().size(), fs_comp_set_->n_total_fs_sheets(), 
                fs_comp_set_->n_valid_comps());
  }

  //Delete all invalid fine-scale FF's from fs_mesh.
  vul_printf (vcl_cout, "Delete invalid fine-scale fs_faces:\n");
  fs_mesh()->del_invalid_FFs_complete();

  if (MS_topo_opt != 2) {
    if (ms_hypg_->check_integrity() == false)
      vul_printf (vcl_cout, "\tms_hypg.check_integrity() fail!\n\n");
    else
      vul_printf (vcl_cout, "\tms_hypg.check_integrity() pass.\n\n");
  }

  return ms_hypg_;
}


//###############################################################
//    Create Scaffold Vertices
//###############################################################

//: In order to maintain an order of the ScaffoldVertices,
//  we create A1A3 Vertices first, then A14, then the Degenerate Vertices.
void dbsk3d_ms_hypg_build::build_ms_nodes ()
{
  vul_printf (vcl_cout, "  build_ms_nodes(): from %u candidate fs_vertices.\n",
              fs_mesh()->vertexmap().size());

  int n_A1A3 = 0;
  int n_A14 = 0;
  int n_Dege_A1mA3 = 0;
  int n_Dege_A1n = 0;

  //The shock scaffold vertices can be built in arbitrary order (in IDs), but here
  //we maintain the order of A1A3, A14, and degenerate ones.

  //1) Create A1A3 scaffold vertices 
  vcl_map<int, dbmsh3d_vertex*>::iterator nit = fs_mesh()->vertexmap().begin();
  for (; nit != fs_mesh()->vertexmap().end(); nit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*nit).second;
    if (FV->get_v_type()==V_TYPE_RIB_END) {
      dbsk3d_ms_node* MN = (dbsk3d_ms_node*) ms_hypg_->_new_vertex ();
      MN->set_n_type (N_TYPE_RIB_END);
      MN->set_V (FV);
      MN->compute_radius();
      ms_hypg_->_add_vertex (MN);
      n_A1A3++;
    }
  }
  vul_printf (vcl_cout, "    # A1A3 Nodes created: %d\n", n_A1A3);
  
  //2) Create A14 scaffold vertices 
  nit = fs_mesh()->vertexmap().begin();
  for (; nit != fs_mesh()->vertexmap().end(); nit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*nit).second;
    if (FV->get_v_type()==V_TYPE_AXIAL_END) {
      dbsk3d_ms_node* MN = (dbsk3d_ms_node*) ms_hypg_->_new_vertex ();
      MN->set_n_type (N_TYPE_AXIAL_END);
      MN->set_V (FV);
      MN->compute_radius();
      ms_hypg_->_add_vertex (MN);
      n_A14++;
    }
  }
  vul_printf (vcl_cout, "    # A14 Nodes created: %d\n", n_A14);

  //3) Create denegerate A1mA3 scaffold vertices
  vul_printf (vcl_cout, "    Degenerate A1mA3 Nodes created: ");
  nit = fs_mesh()->vertexmap().begin();
  for (; nit != fs_mesh()->vertexmap().end(); nit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*nit).second;
    if (FV->get_v_type()==V_TYPE_DEGE_RIB_END) {
      dbsk3d_ms_node* MN = (dbsk3d_ms_node*) ms_hypg_->_new_vertex ();
      MN->set_n_type (N_TYPE_DEGE_RIB_END);
      MN->set_V (FV);
      MN->compute_radius();
      ms_hypg_->_add_vertex (MN);
      vul_printf (vcl_cout, "%d (FV %d), ", MN->id(), FV->id());
      n_Dege_A1mA3++;
    }
  }
  vul_printf (vcl_cout, "\n      totally %d degenerate A1mA3 nodes created.\n", n_Dege_A1mA3);

  //4) Create denegerate A1n scaffold vertices
  vul_printf (vcl_cout, "    Degenerate A1n Nodes created: ");
  nit = fs_mesh()->vertexmap().begin();
  for (; nit != fs_mesh()->vertexmap().end(); nit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*nit).second;
    if (FV->get_v_type()==V_TYPE_DEGE_AXIAL_END) {
      dbsk3d_ms_node* MN = (dbsk3d_ms_node*) ms_hypg_->_new_vertex ();
      MN->set_n_type (N_TYPE_DEGE_AXIAL_END);
      MN->set_V (FV);
      MN->compute_radius();
      ms_hypg_->_add_vertex (MN);
      vul_printf (vcl_cout, "%d (FV %d), ", MN->id(), FV->id());
      n_Dege_A1n++;
    }
  }
  vul_printf (vcl_cout, "\n      totally %d degenerate A1n nodes created.\n", n_Dege_A1n);
}

//###############################################################
//    Build Scaffold Curves
//###############################################################

void dbsk3d_ms_hypg_build::build_ms_curves ()
{
  vul_printf (vcl_cout, "  build_ms_curves(): checking %u candidate scaffold vertices.\n",
              ms_hypg_->vertexmap().size());

  //The shock scaffold curves can be built in arbitrary order (in IDs).
  vcl_map<int, dbmsh3d_vertex*>::iterator it = ms_hypg_->vertexmap().begin();
  for (; it != ms_hypg_->vertexmap().end(); it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*it).second;
    if (MN->n_type() == N_TYPE_RIB_END) {
      trace_build_A3_curves (MN);
      trace_build_A13_curves (MN);
      ///trace_build_Dege_curves (MN);
    }
    else if (MN->n_type() == N_TYPE_AXIAL_END) {
      trace_build_A13_curves (MN);
      ///trace_build_Dege_curves (MN);
    }
    else if (MN->n_type() == N_TYPE_DEGE_RIB_END) {
      trace_build_A3_curves (MN);
      trace_build_A13_curves (MN);
      trace_build_Dege_curves (MN);
    }
    else if (MN->n_type() == N_TYPE_DEGE_AXIAL_END) {
      trace_build_A13_curves (MN);
      trace_build_Dege_curves (MN);
    }
    else
      assert (0);
  }
}

//###############################################################
//    Degenerate Cases: Build Scaffold Loops
//###############################################################

//: After all shock scaffold curves have been created,
//  search for any valid fs_edge that's NOT visited.
//  They have to form a loop (without any incident scaffold node)
//  We make an arbitrary fs_vertex from them a loop_vertex 
//  and build this scaffold_loop_curve as a special case of dbsk3d_ms_curve.
void dbsk3d_ms_hypg_build::build_ms_loops ()
{
  vul_printf (vcl_cout, "  build_ms_loops(): from %u candidate fs_edges.\n",
              fs_mesh()->edgemap().size());
  vcl_map<int, dbmsh3d_edge*>::iterator lit = fs_mesh()->edgemap().begin();
  for (; lit != fs_mesh()->edgemap().end(); lit++) {
    dbsk3d_fs_edge* startFE = (dbsk3d_fs_edge*) (*lit).second;
    if (startFE->is_visited(ms_hypg_->i_traverse_flag()))
      continue;
    if (startFE->e_type() == E_TYPE_MANIFOLD)
      continue;
    assert (startFE->is_valid_via_F() == true);

    //Construct the curve.
    dbmsh3d_edge* FE = startFE;
    dbmsh3d_vertex* FV = (dbsk3d_fs_vertex*) startFE->s_FV();

    //Create a new scaffold_loop_curve from this FE
    dbsk3d_ms_node* loopMN = (dbsk3d_ms_node*) ms_hypg_->_new_vertex ();
    loopMN->set_n_type (N_TYPE_LOOP_END);
    loopMN->set_V (FV);
    loopMN->compute_radius();
    dbsk3d_ms_curve* loopMC = (dbsk3d_ms_curve*) ms_hypg_->_new_edge (NULL, NULL);
    
    loopMC->connect_loop_V (loopMN);
    ms_hypg_->_add_vertex (loopMN);
    ms_hypg_->_add_edge (loopMC);

    if (startFE->e_type() == E_TYPE_RIB) {
      loopMC->set_c_type (C_TYPE_RIB);
      do {
        assert (!FE->is_visited(ms_hypg_->i_traverse_flag()));
        loopMC->add_E_to_back (FE);
        FE->set_i_visited (ms_hypg_->i_traverse_flag());

        FV = FE->other_V (FV);
        FE = FV->other_rib_E (FE);
        assert (FE != NULL);
      }
      while (FV->get_v_type() == V_TYPE_RIB && FE->e_type() == E_TYPE_RIB && FE != startFE);
    }
    else if (startFE->e_type() == E_TYPE_AXIAL) {
      loopMC->set_c_type (C_TYPE_AXIAL);
      do {
        assert (!FE->is_visited(ms_hypg_->i_traverse_flag()));
        loopMC->add_E_to_back (FE);
        FE->set_i_visited (ms_hypg_->i_traverse_flag());

        FV = FE->other_V (FV);
        FE = FV->other_axial_E (FE);
        assert (FE != NULL);
      }
      while (FV->get_v_type() == V_TYPE_AXIAL && FE->e_type() == E_TYPE_AXIAL && FE != startFE);
    }
    else {
      assert (startFE->e_type() == E_TYPE_DEGE_AXIAL);
      loopMC->set_c_type (C_TYPE_DEGE_AXIAL);
      do {
        assert (!FE->is_visited(ms_hypg_->i_traverse_flag()));
        loopMC->add_E_to_back (FE);
        FE->set_i_visited (ms_hypg_->i_traverse_flag());

        FV = FE->other_V (FV);
        FE = FV->other_dege_E (FE);
        assert (FE != NULL);
      }
      while (FV->get_v_type() == V_TYPE_AXIAL && FE->e_type() == E_TYPE_DEGE_AXIAL && FE != startFE);
    }

    //Check that the connectivity is correct.
    assert (FE == startFE);
  }
}

void dbsk3d_ms_hypg_build::trace_build_A3_curves (dbsk3d_ms_node* A1A3MN)
{
  dbsk3d_fs_vertex* startMN = (dbsk3d_fs_vertex*) A1A3MN->V();

  for (dbmsh3d_ptr_node* cur = startMN->E_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* startFE = (dbsk3d_fs_edge*) cur->ptr();
    if (startFE->e_type() != E_TYPE_RIB)
      continue;
    if (startFE->is_visited(ms_hypg_->i_traverse_flag()))
      continue;

    //Create a new A3 scaffold curve
    dbsk3d_ms_curve* A3Curve = (dbsk3d_ms_curve*) ms_hypg_->_new_edge (NULL, NULL);
    A3Curve->set_c_type (C_TYPE_RIB);
    A3Curve->connect_V (0, A1A3MN);
    ms_hypg_->_add_edge (A3Curve);

    //Construct the curve. The MC can end at either A1A3 or Dege.
    dbmsh3d_vertex* FV = startMN;
    dbmsh3d_edge* FE = startFE;
    do {
      assert (FE != NULL);
      FV = FE->other_V (FV);
      A3Curve->add_E_to_back (FE);
      assert (FE->is_visited(ms_hypg_->i_traverse_flag()) == false);
      FE->set_i_visited (ms_hypg_->i_traverse_flag());
      FE = FV->other_rib_E (FE);
    }
    while (FV->get_v_type() != V_TYPE_RIB_END && FV->get_v_type() != V_TYPE_DEGE_RIB_END);

    //The ending dbsk3d_ms_node.
    assert (FV->get_v_type() == V_TYPE_RIB_END || FV->get_v_type() == V_TYPE_DEGE_RIB_END);
    dbsk3d_ms_node* EV = (dbsk3d_ms_node*) ms_hypg_->vertexmap (FV->i_value());
    assert (EV->id() == FV->i_value());
    if (EV != A1A3MN)
      A3Curve->connect_V (1, EV);
    else { //The unlikely A3 loop case
      A3Curve->_disconnect_V_idx (0);
      A3Curve->connect_loop_V (A1A3MN);
    }
  }
}

void dbsk3d_ms_hypg_build::trace_build_A13_curves (dbsk3d_ms_node* A14MN)
{
  dbsk3d_fs_vertex* startMN = (dbsk3d_fs_vertex*) A14MN->V();

  for (dbmsh3d_ptr_node* cur = startMN->E_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* startFE = (dbsk3d_fs_edge*) cur->ptr();
    if (startFE->e_type() != E_TYPE_AXIAL)
      continue;
    if (startFE->is_visited(ms_hypg_->i_traverse_flag()))
      continue;

    //Create a new A3ScaffoldCurve
    dbsk3d_ms_curve* A13MC = (dbsk3d_ms_curve*) ms_hypg_->_new_edge (NULL, NULL);
    A13MC->set_c_type (C_TYPE_AXIAL);
    A13MC->connect_V (0, A14MN);
    ms_hypg_->_add_edge (A13MC);

    //Construct the curve. The MC can end at either A1A3, A14, or Dege.
    dbmsh3d_vertex* FV = startMN;
    dbmsh3d_edge* FE = startFE;
    do {
      assert (FE != NULL);
      FV = FE->other_V (FV);
      assert (FV->id() != 0);
      A13MC->add_E_to_back (FE);
      assert (FE->is_visited(ms_hypg_->i_traverse_flag()) == false);
      FE->set_i_visited (ms_hypg_->i_traverse_flag());
      FE = FV->other_axial_E (FE);
    }
    while (FV->get_v_type() != V_TYPE_AXIAL_END && FV->get_v_type() != V_TYPE_RIB_END && 
           FV->get_v_type() != V_TYPE_DEGE_AXIAL_END && FV->get_v_type() != V_TYPE_DEGE_RIB_END);
    
    //The ending dbsk3d_ms_node.
    assert (FV->get_v_type() == V_TYPE_RIB_END || FV->get_v_type() == V_TYPE_AXIAL_END || 
            FV->get_v_type() == V_TYPE_DEGE_RIB_END || FV->get_v_type() == V_TYPE_DEGE_AXIAL_END);
    dbsk3d_ms_node* eMN = (dbsk3d_ms_node*) ms_hypg_->vertexmap (FV->i_value());
    assert (eMN->id() == FV->i_value());
    if (eMN != A14MN)
      A13MC->connect_V (1, eMN);
    else { //The unlikely A13 loop case
      A13MC->_disconnect_V_idx (0);
      A13MC->connect_loop_V (A14MN);
    }
  }
}

void dbsk3d_ms_hypg_build::trace_build_Dege_curves (dbsk3d_ms_node* DegeMN)
{
  dbsk3d_fs_vertex* startMN = (dbsk3d_fs_vertex*) DegeMN->V();

  for (dbmsh3d_ptr_node* cur = startMN->E_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* startFE = (dbsk3d_fs_edge*) cur->ptr();
    if (startFE->e_type() != E_TYPE_DEGE_AXIAL)
      continue;
    if (startFE->is_visited(ms_hypg_->i_traverse_flag()))
      continue;

    //Create a new DegeScaffoldCurve
    dbsk3d_ms_curve* degeMC = (dbsk3d_ms_curve*) ms_hypg_->_new_edge (NULL, NULL);
    degeMC->set_c_type (C_TYPE_DEGE_AXIAL);
    degeMC->connect_V (0, DegeMN);
    ms_hypg_->_add_edge (degeMC);

    //Construct the curve.
    //The MC can only end at a DegeMN.
    dbmsh3d_vertex* FV = startMN;
    dbmsh3d_edge* FE = startFE;
    do {
      assert (FE != NULL);
      FV = FE->other_V (FV);
      assert (FV->id() != 0);
      degeMC->add_E_to_back (FE);
      assert (FE->is_visited(ms_hypg_->i_traverse_flag()) == false);
      FE->set_i_visited (ms_hypg_->i_traverse_flag());
      FE = FV->other_dege_E (FE);
    }
    while (FV->get_v_type() != V_TYPE_DEGE_RIB_END && FV->get_v_type() != V_TYPE_DEGE_AXIAL_END);
    
    //The ending dbsk3d_ms_node.
    assert (FV->get_v_type() == V_TYPE_DEGE_RIB_END || FV->get_v_type() == V_TYPE_DEGE_AXIAL_END);
    dbsk3d_ms_node* eMN = (dbsk3d_ms_node*) ms_hypg_->vertexmap (FV->i_value());
    assert (eMN->id() == FV->i_value());
    assert (eMN != DegeMN);
    degeMC->connect_V (1, eMN);
  }
}

//###############################################################
//    Build Scaffold Sheets
//###############################################################

//: The sheet component is connected through regular A13LinkElements.
//  Go through each A12PatchElements and build the component.
//  Visited patchElms are marked visited.
void dbsk3d_ms_hypg_build::build_ms_sheets (const int MS_topo_opt)
{
  vul_printf (vcl_cout, "  build_ms_sheets(): from %u candidate fs_faces.\n",
              fs_mesh()->facemap().size());
  fs_mesh()->init_traverse();  

  //Propagate and build a ms_sheet from the first available fs_face of the MS.
  vul_printf (vcl_cout, "\t");
  int prev_per=0, count=0;
  vcl_set<dbsk3d_ms_sheet*> SS_to_del;
  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_mesh()->facemap().begin();
  for (; pit != fs_mesh()->facemap().end(); pit++, count++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;

    if (!FF->b_valid())
      continue;
    if (FF->is_visited(ms_hypg_->i_traverse_flag()))
      continue;

    //Show progress percentage.
    float percentage = (float) (count * 100.0 / fs_mesh()->facemap().size());
    int per = (int) vcl_ceil (percentage);
    if (per - prev_per > 9) {
      vul_printf (vcl_cout, "%2d%% ", per);
      prev_per = per;
    }

    //Visit this FF. Construct a shock scaffold sheet component.
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) ms_hypg_->_new_sheet ();

    //propagate to build the Scaffold Sheet
    bool result = prop_build_sheet_comp (MS, FF, MS_topo_opt);
    if (result == false)
      SS_to_del.insert (MS);
    ms_hypg_->_add_sheet (MS);
  }

  vul_printf (vcl_cout, "100%%.\n");

  //Delete the invalid MS
  vcl_set<dbsk3d_ms_sheet*>::iterator it = SS_to_del.begin();
  while (it != SS_to_del.end()) {
    dbsk3d_ms_sheet* MS = (*it);
    SS_to_del.erase (it);

    //Mark all fs_faces of MS to be invalids.
    MS->mark_all_FFs_invalid (); 
    //Delete the medial sheet MS and modify types of remaining MC and MN.
    ms_hypg_->remove_S_complete_hypg (MS);

    it = SS_to_del.begin();
  }
}

//: return the next MC that serves as the boundary of the MS
dbsk3d_ms_curve* get_next_MC_from_set (dbsk3d_ms_curve* inputMC, dbmsh3d_vertex* inputMN, 
                                       vcl_set<dbsk3d_ms_curve*>& MCset)
{
  vcl_set<dbsk3d_ms_curve*>::iterator it = MCset.begin();
  for (; it != MCset.end(); it++) {
    dbsk3d_ms_curve* MC = (*it);
    if (MC != inputMC && MC->is_V_incident (inputMN)) {
      //found, remove from set and return it.
      MCset.erase (it);
      return MC;
    }
  }
  return NULL;
}

//: 1)Build the dbsk3d_ms_sheet and
//  2)Setup dbsk3d_ms_sheet <-> dbsk3d_ms_curve Connectivity
//  3)Setup dbsk3d_ms_sheet <-> dbsk3d_ms_node Connectivity
//
//-ssopt : shock sheet topology option for building ms_hypg (def 0).
//  0: do not allow shock sheet to have 2+ rib boundary.
//     outside shocks with such topology will be deleted.
//  1: allow a general case that shock sheet can have 2+ rib boundary.
//  2: turn off MS sheet canonicalization.
//
bool dbsk3d_ms_hypg_build::prop_build_sheet_comp (dbsk3d_ms_sheet* MS, dbsk3d_fs_face* FF,
                                                  const int MS_topo_opt)
{
  //1) Build the dbsk3d_ms_sheet by propagating from this FF (through manifold fs_edges)
  //   Also detect incident shock scaffold curves.
  vcl_queue<dbsk3d_fs_face*> frontFF_queue;
  frontFF_queue.push (FF);  
  vcl_set<dbsk3d_ms_curve*> incident_MCs; //The set of boundary MCs connected to this MS.

  while (frontFF_queue.size() > 0) {
    dbsk3d_fs_face* frontFF = frontFF_queue.front();
    frontFF_queue.pop();
    //Needed, because a FF can be on the front for multiple times.
    if (frontFF->is_visited(ms_hypg_->i_traverse_flag()))
      continue;

    MS->add_F (frontFF);
    frontFF->set_i_visited (ms_hypg_->i_traverse_flag());

    //propagate the frontFF (through regular A12 manifold fs_edge)
    dbmsh3d_halfedge* HE = frontFF->halfedge();
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
      if (FE->b_inf()) { //Skip the FE that's marked as 'infinity'
        HE = HE->next();
        continue;
      }
      if (FE->e_type() == E_TYPE_RIB || FE->e_type() == E_TYPE_AXIAL || FE->e_type() == E_TYPE_DEGE_AXIAL) {
        dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) ms_hypg_->edgemap (FE->s_value());
        assert (MC->id() == FE->s_value());
        incident_MCs.insert (MC);
      }
      else {
        assert (FE->e_type() == E_TYPE_MANIFOLD);
        dbsk3d_fs_face* otherFF = (dbsk3d_fs_face*) FE->other_valid_F (frontFF);
        assert (otherFF != NULL);
        if (!otherFF->is_visited(ms_hypg_->i_traverse_flag()))
          frontFF_queue.push (otherFF);
      }
      HE = HE->next();
    }
    while (HE != frontFF->halfedge());
  }

  //2) For each internal_curve MC, setup the both-way-pointers 
  //   for the sheet-curve & curve-sheet incidence.
  //   And, remove it from the incident_MCs set.
  vcl_set<dbsk3d_ms_curve*>::iterator MCsit = incident_MCs.begin();
  while (MCsit!=incident_MCs.end()) {
    dbsk3d_ms_curve* MC = (*MCsit);
    assert (MC);
    int n_incidence = n_incidence_MC_MS (MC, MS);

    //1) For each internal curve
    if (n_incidence == 2) {
      //Create a new icurve entry of the 2-incidence.
      MS->connect_icurve_pair_E (MC);

      //Remove MC from the temporary set.
      // if deleting the head, next one to look at is the head.
      if (MCsit == incident_MCs.begin()) {
        incident_MCs.erase (MCsit);
        MCsit = incident_MCs.begin();
      } 
      else { //else, go for the next one.
        vcl_set<dbsk3d_ms_curve*>::iterator SCset_tmp = MCsit;
        MCsit--;
        incident_MCs.erase (SCset_tmp);
        MCsit++;
      }
    }
    else {
      MCsit++;
    }
  }

  //3) The MS must have a (non-interior) boundary shock curve.
  if (incident_MCs.size() == 0) {
    //In some rare case, the incident_MCs can be empty, resulting in
    //MS without any boundary curve. The MS should be deleted in this case.    
    vul_printf (vcl_cout, "    Delete MS %d with non-exterior shock curve!\n", MS->id());
    return false; //Return false to delete this MS later
  }

  //4) The remaining MCs in the incident_MCs set are the real boundary curves.
  //   Try to trace and build one or many boundary chains of halfedges from them.
  //   Only two possibilities here, n_incidence = 1 or 3, where
  //        n_incidence = 1 : the boundary case, put in one incidence,
  //        n_incidence = 3 : the A5 transition link case, a triple-incidence.
  //   Start from the head for each chain, loop until the incident_MCs is empty.
  MCsit = incident_MCs.begin();
  dbsk3d_ms_curve* startMC = (*MCsit);
  incident_MCs.erase (MCsit); //Remove it from the set.
  
  //For each boundary curve, create a new boundary chain to store the halfedges.
  int n_incidence = n_incidence_MC_MS (startMC, MS);
  if (n_incidence == 1)
    MS->connect_bnd_E_end (startMC);
  else if (n_incidence == 3)
    MS->connect_bnd3_E_end (startMC);
  else {
    vul_printf (vcl_cout, "ERROR: n_incidence > 3 ");
    assert (0);
  }

  //Now start to trace from this incident_MCs to build one boundary chain.
  dbsk3d_ms_curve* curMC = startMC;
  dbmsh3d_vertex* curMN = curMC->s_MN();
  do {
    dbsk3d_ms_curve* nextMC = get_next_MC_from_set (curMC, curMN, incident_MCs);
    if (nextMC == NULL)
      break;

    int n_incidence = n_incidence_MC_MS (nextMC, MS);
    if (n_incidence == 1)
      MS->connect_bnd_E_end (nextMC);
    else if (n_incidence == 3)
      MS->connect_bnd3_E_end (nextMC);
    else {
      vul_printf (vcl_cout, "ERROR: n_incidence > 3 ");
      assert (0);
    }

    //Go to the next.
    curMC = nextMC;
    curMN = curMC->other_V (curMN);
  }
  while (1);
  //Make sure we get a closed loop
  assert (startMC->is_V_incident (curMN));

  //5) Additional boundary curve loop to add to the main boundary loop.
  //   This is for the rare case of merging a boundary loop (created during tracing) 
  //   to the main boundary halfedge chain.
  vcl_list<vcl_vector<dbmsh3d_edge*> > list_loop_Es;
  list_loop_Es.clear();
  int count = 0;
  while (incident_MCs.size() != 0) {
    vul_printf (vcl_cout, "\n  Trace bnd-loop for ms_sheet %d: remaining %d ms_curves.\n", 
                MS->id(), incident_MCs.size());
    MCsit = incident_MCs.begin();
    dbsk3d_ms_curve* loop_startMC = (*MCsit);
    incident_MCs.erase (MCsit);

    //insert an entry for the loop_bnd_Es.
    vcl_vector<dbmsh3d_edge*> loop_bnd_Es_place_holder;
    list_loop_Es.push_back (loop_bnd_Es_place_holder);
    vcl_list<vcl_vector<dbmsh3d_edge*> >::reverse_iterator rit = list_loop_Es.rbegin();
    vcl_vector<dbmsh3d_edge*>* loop_bnd_Es = &(*rit);

    //For each boundary curve, create a new boundary chain to store the halfedges.
    int n_incidence = n_incidence_MC_MS (loop_startMC, MS);
    if (n_incidence == 1 || n_incidence == 3) {
      loop_bnd_Es->push_back (loop_startMC);
      count++;
    }
    else
      assert (0);

    //Now start to trace from this incident_MCs to build one boundary chain.
    curMC = loop_startMC;
    curMN = curMC->sV();
    do {
      dbsk3d_ms_curve* nextMC = get_next_MC_from_set (curMC, curMN, incident_MCs);
      if (nextMC == NULL)
        break;

      int n_incidence = n_incidence_MC_MS (nextMC, MS);
      if (n_incidence == 1 || n_incidence == 3) {
        loop_bnd_Es->push_back (nextMC); 
        count++;
      }
      else
        assert (0);

      //Go to the next.
      curMC = nextMC;
      curMN = curMC->other_V (curMN);
    }
    while (1);
    //Make sure we get a closed loop
    assert (loop_startMC->is_V_incident (curMN));
  }

  //6) Insert the list_loop_Es[][] to MS's boundary chain.
  bool change;
  vcl_list<vcl_vector<dbmsh3d_edge*> >::iterator lve_it;
  do {
    //Go through the loop_bnd_Scs_set[][] to insert boundary chains.
    change = false;
    lve_it = list_loop_Es.begin();
    while (lve_it != list_loop_Es.end()) {
      vcl_vector<dbmsh3d_edge*> loop_bnd_Es = (*lve_it);

      bool result = _insert_bnd_loop (MS, loop_bnd_Es);      
      if (result) { 
        //If insertion to bnd loop is successful, remove from the list.
        if (lve_it == list_loop_Es.begin()) {
          list_loop_Es.erase (lve_it);
          lve_it = list_loop_Es.begin();
        }
        else {
          vcl_list<vcl_vector<dbmsh3d_edge*> >::iterator lve_tmp = lve_it;
          lve_it--;
          list_loop_Es.erase (lve_tmp);
          lve_it++;
        }
        loop_bnd_Es.clear();
        change = true;
      }
      else
        lve_it++;
    }    
  }
  while (change == true && list_loop_Es.size() != 0);

  //7) Insert remaining list_loop_Es[][] to i-curve chains.
  //   Be careful on the rare case of a rib loop_bnd.
  int rib_loop_cnt = 0;
  lve_it = list_loop_Es.begin();
  for (; lve_it != list_loop_Es.end(); lve_it++) {
    vcl_vector<dbmsh3d_edge*> loop_bnd_Es = (*lve_it);
    assert (loop_bnd_Es.size());

    //If any of the loop_bnd_Es is of type RIB, mark the flag.
    for (unsigned j=0; j<loop_bnd_Es.size(); j++) {
      dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) loop_bnd_Es[j];
      if (MC->c_type() == C_TYPE_RIB) {
        rib_loop_cnt++;
        break;
      }
    }

    //Create a new i-curve chain and insert loop_bnd_Es to it.
    MS->connect_icurve_loop_Es (loop_bnd_Es);
  }

  //8) a hack to fix possible 3-incidence MC's in the list_loop_Es[][].
  lve_it = list_loop_Es.begin();
  for (; lve_it != list_loop_Es.end(); lve_it++) {
    vcl_vector<dbmsh3d_edge*> loop_bnd_Es = (*lve_it);
    for (unsigned j=0; j<loop_bnd_Es.size(); j++) {
      dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) loop_bnd_Es[j];
      if (MC->c_type() == C_TYPE_AXIAL && n_incidence_MC_MS (MC, MS) == 3 && MC->n_incident_Fs() ==1) {
        //make another two incidence to MS as an i-curve.
        vul_printf (vcl_cout, "  Hack fixing 3-incidence MC in remaining list_loop_Es[][] MC %d.\n", MC->id());
        MS->connect_icurve_pair_E (MC);
      }
    }
  }


  //8) For the case of MS with a single rib_loop_bnd, make it the main boundary.
  if (rib_loop_cnt==1) {
    //Assert that the current bnd_chain has no rib.
    bool bnd_chain_has_rib = MC_chain_contains_rib (MS->halfedge());

    if (bnd_chain_has_rib) {
      //This MS has rib_bnd_chain and rib_bnd_loop. 
      //Should make the larger boundary loop as the main bnd.

      ///vul_printf (vcl_cout, "    Delete MS %d with 2+ rib_bnd_chain!\n", MS->id());       
      ///return false; //Return false to delete this MS later
    }
    else {
      //Swap the halfedge_ and the i-curve entry.
      for (dbmsh3d_ptr_node* cur = MS->icurve_chain_list(); cur != NULL; cur = cur->next()) {
        dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) cur->ptr();
        if (MC_chain_contains_rib (HE)) {
          vul_printf (vcl_cout, " MS%d: Swap rib MC_chain to be bnd_chain! ", MS->id());
          cur->set_ptr (MS->halfedge());
          MS->set_halfedge (HE);
          break;
        }
      }
    }
  }

  //Avoid MS->halfedge to be on the 3-incidence edges.
  fix_headHE_3_incidence (MS->halfedge());

  //9)Canonicalization: re-organize MS's bnd_HE_chain and icurve_HE_chain.
  // -ssopt 2: turn off MS sheet canonicalization.
  if (MS_topo_opt != 2) 
    MS->canonicalization ();

  if (list_loop_Es.size() != 0)
    vul_printf (vcl_cout, "MS%d: remaining list_loop_Es[%d][] not empty! ", 
                MS->id(), list_loop_Es.size());
  list_loop_Es.clear();

  assert (MS->halfedge());
  return true;
}

void prop_add_Fs_from_seed (dbsk3d_ms_sheet* MSn, dbsk3d_fs_face* seedFF)
{
  vcl_set<dbmsh3d_face*> Fset;
  _prop_label_Fs_e_conn (seedFF, Fset);

  //Add Fset to MS.
  vcl_set<dbmsh3d_face*>::iterator it = Fset.begin();
  for (; it != Fset.end(); it++) {
    dbmsh3d_face* F = (*it);
    MSn->add_F (F);
  }
}



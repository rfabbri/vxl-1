//: This is 3DShock_Scaffold_Element_Curve.cxx
//  MingChing Chang
//  Nov 23, 2004.

#include <vcl_cstdio.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbsk3d/dbsk3d_ms_node.h>
#include <dbsk3d/dbsk3d_ms_curve.h>
#include <dbsk3d/dbsk3d_ms_sheet.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/dbsk3d_sg_sa.h>


const bool dbsk3d_ms_curve::self_loop_on_LV () const 
{
  return is_self_loop() && s_MN()->n_type() == N_TYPE_LOOP_END;
}

//##########################################################
//  Query functions
//##########################################################

double dbsk3d_ms_curve::get_sum_node_radius () const
{
  double sum = 0;
  for (unsigned int i=0; i<E_vec_.size(); i++) {
    const dbsk3d_fs_edge* FE = (const dbsk3d_fs_edge*) E_vec_[i];
    const dbmsh3d_vertex* G = FE->_get_one_G_via_FF ();
    vgl_point_3d<double> M = FE->mid_pt();
    double r = vgl_distance (G->pt(), M);
    sum += r;
  }
  return sum;
}

//##########################################################
//  Modification functions
//##########################################################

//##########################################################
//####### Generator handling functions #######
//##########################################################

void dbsk3d_ms_curve::get_asso_Gs (vcl_map<int, dbmsh3d_vertex*>& asso_Gs) const
{
  assert (data_type_ == C_DATA_TYPE_EDGE);

  for (unsigned int i=0; i<E_vec_.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) E_vec_[i];
    FE->get_asso_Gs (asso_Gs);
  }
}

void dbsk3d_ms_curve::get_asso_Gs_incld_FFs (vcl_map<int, dbmsh3d_vertex*>& asso_Gs) const
{
  assert (data_type_ == C_DATA_TYPE_EDGE);

  for (unsigned int i=0; i<E_vec_.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) E_vec_[i];
    FE->get_asso_Gs_incld_FFs (asso_Gs);
  }
}

//##########################################################
//###### Other functions ######
//##########################################################

bool dbsk3d_ms_curve::check_integrity ()
{
  if (dbmsh3d_curve::check_integrity() == false) {
    assert (0);
    return false;
  }

  //Check the integrity of fine-scale edge types.
  //Skip checking type of shared elements.
  unsigned int i;

  if (data_type_ == C_DATA_TYPE_EDGE) {
    //Check the RIB, AXIAL, DEGE_AXIAL element types
    if (c_type_ == C_TYPE_RIB) {
      if (n_incident_Fs() != 1) {
        assert (0);
        return false;
      }
      for (i=0; i<E_vec_.size(); i++) {
        dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) E_vec_[i];
        if (is_E_shared (FE) || FE->e_type() == E_TYPE_SPECIAL)
          continue; //Skip shared and special elements
        if (FE->n_incident_Fs() != 1) {
          assert (0);
          return false;
        }
        if (FE->e_type() != E_TYPE_RIB) {
          assert (0);
          return false;
        }
      }
    }
    else if (c_type_ == C_TYPE_AXIAL) {
      if (n_incident_Fs() != 3) {
        assert (0);
        return false;
      }
      for (i=0; i<E_vec_.size(); i++) {
        dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) E_vec_[i];
        if (is_E_shared (FE) || FE->e_type() == E_TYPE_SPECIAL)
          continue; //Skip shared and special elements        
        if (FE->n_incident_Fs() != 3) {
          assert (0);
          return false;
        }
        if (FE->e_type() != E_TYPE_AXIAL) {
          assert (0);
          return false;
        }
      }
    }
    else if (c_type_ == C_TYPE_DEGE_AXIAL) {
      if (n_incident_Fs() < 4) {
        assert (0);
        return false;
      }
      for (i=0; i<E_vec_.size(); i++) {
        dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) E_vec_[i];
        if (is_E_shared (FE) || FE->e_type() == E_TYPE_SPECIAL)
          continue; //Skip shared and special elements
        if (FE->n_incident_Fs() < 4) {
          assert (0);
          return false;
        }
        if (FE->e_type() != E_TYPE_DEGE_AXIAL) {
          assert (0);
          return false;
        }
      }
    }
    else if (c_type_ == C_TYPE_VIRTUAL) {
      //skip
    }
    else {
      assert (0);
    }
  }
  else if (data_type_ == C_DATA_TYPE_VERTEX) {
    //Not yet implemented.
  }
  else {
    assert (0);
    return false;
  }

  return true;
}

//: The clone function clones everything except the connectivity
//  when cloning a graph, need to build the connectivity separetely.
//  The pointer to the fs_vertices and fs_edges are not cloned.
dbmsh3d_edge* dbsk3d_ms_curve::clone () const
{
  dbsk3d_ms_curve* MC2 = new dbsk3d_ms_curve (NULL, NULL, id_);
  MC2->set_length (length_);
  MC2->set_c_type (c_type_);
  return MC2;
}

dbsk3d_ms_curve* dbsk3d_ms_curve::clone (dbmsh3d_hypg* HG2, dbmsh3d_mesh* M2) const
{
  dbsk3d_ms_curve* MC2 = (dbsk3d_ms_curve*) clone ();  
  _clone_E_V_conn (MC2, HG2);
  _clone_C_EV_conn (MC2, M2);  
  return MC2;
}

#define MAX_ELMS_INFO  10

void dbsk3d_ms_curve::getInfo (vcl_ostringstream& ostrm)
{
  char s[1024];

  vcl_string type;
  switch (c_type_) {
  case C_TYPE_RIB:
     type = "A3";
  break;
  case C_TYPE_AXIAL:
     type = "A13";
  break;
  case C_TYPE_DEGE_AXIAL:
     type = "Dege";
  break;
  default:
  break;
  }

  vcl_sprintf (s, "==============================\n"); ostrm<<s;
  vcl_sprintf (s, "Medial Curve (%s) %d (s_MN %d -> e_MN %d) ", 
               type.c_str(), id_, sV()->id(), eV()->id()); ostrm<<s;
  compute_length ();
  vcl_sprintf (s, "length: %f\n", length_); ostrm<<s;  
  bool result = check_integrity();
  vcl_sprintf (s, "check_integrity: %s\n\n", result ? "pass." : "fail!"); ostrm<<s;
  
  //: Incident Shock Sheets
  unsigned int n_incidence = n_incident_Fs();
  vcl_sprintf (s, "# incident ms_sheet %u:", n_incidence); ostrm<<s;
  if (n_incidence == 0) {
  }
  else if (n_incidence == 1) {
    vcl_sprintf (s, " %d", halfedge_->face()->id()); ostrm<<s;
  }
  else {
    dbmsh3d_halfedge* he = halfedge_;
    do {
      vcl_sprintf (s, " %d", he->face()->id()); ostrm<<s;
      he = he->pair();
    }
    while (he != halfedge_);
  }
  vcl_sprintf (s, "\n"); ostrm<<s;

  //fs_edges
  vcl_sprintf (s, "# E_vec %u:", E_vec_.size()); ostrm<<s;
  if (E_vec_.size() < MAX_ELMS_INFO*2) {
    for (unsigned int i=0; i<E_vec_.size(); i++) {
      const dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) E_vec_[i];
      vcl_sprintf (s, " %d", FE->id()); ostrm<<s;
    }
  }
  else {
    for (int i=0; i<MAX_ELMS_INFO; i++) {
      const dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) E_vec_[i];
      vcl_sprintf (s, " %d", FE->id()); ostrm<<s;
    }
    vcl_sprintf (s, " ..."); ostrm<<s;
    for (int i=int(E_vec_.size())-1-MAX_ELMS_INFO; i<int(E_vec_.size()); i++) {
      const dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) E_vec_[i];
      vcl_sprintf (s, " %d", FE->id()); ostrm<<s;
    }
  }
  vcl_sprintf (s, "\n\n"); ostrm<<s;

  //V_vec
  vcl_vector<dbmsh3d_vertex*> V_vec;
  get_V_vec (V_vec);

  vcl_sprintf (s, "# V_vec %u:", V_vec.size()); ostrm<<s;
  if (V_vec.size() < MAX_ELMS_INFO*2) {
    for (unsigned int i=0; i<V_vec.size(); i++) {
      const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) V_vec[i];
      vcl_sprintf (s, " %d", FV->id()); ostrm<<s;
    }
  }
  else {
    for (int i=0; i<MAX_ELMS_INFO; i++) {
      const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) V_vec[i];
      vcl_sprintf (s, " %d", FV->id()); ostrm<<s;
    }
    vcl_sprintf (s, " ..."); ostrm<<s;
    for (int i=int(V_vec.size())-1-MAX_ELMS_INFO; i<int(V_vec.size()); i++) {
      const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) V_vec[i];
      vcl_sprintf (s, " %d", FV->id()); ostrm<<s;
    }
  }
  vcl_sprintf (s, "\n"); ostrm<<s;

  vcl_sprintf (s, "\n\n"); ostrm<<s;
}

//###############################################################

//: if length_ != 0, no need to re-compute.
double dbsk3d_ms_curve::get_length ()
{
  if (length_ == 0)
    length_ = (float) compute_length ();
  return length_;
}

void dbsk3d_ms_curve::setup_virtual_curve ()
{
  int j;
  assert (c_type_ == C_TYPE_VIRTUAL);
  assert (halfedge_ == NULL);

  //Setup E_vec_[] from sup_curves_[]
  dbmsh3d_node* N = s_N();
  for (unsigned int i=0; i<sup_curves_.size(); i++) {
    dbmsh3d_curve* C = sup_curves_[i];
    //add C.E_vec into this E_vec_[] following the order.
    if (C->s_N() == N) {
      for (j=0; j<int(C->E_vec().size()); j++)
        E_vec_.push_back (C->E_vec(j));
    }
    else {
      for (j=int(C->E_vec().size())-1; j>=0; j--)
        E_vec_.push_back (C->E_vec(j));
    }
    N = (dbmsh3d_node*) C->other_V (N);
  }
}

C_TYPE dbsk3d_ms_curve::get_C_VC_c_type() const
{
  //handle the case of non-virtual curve.
  if (c_type_ != C_TYPE_VIRTUAL)
    return c_type_;

  bool rib_found = false;
  bool dege_axial_found = false;

  for (unsigned int i=0; i<sup_curves_.size(); i++) {
    //if any of the sup_curve[] is of rib type, it is a rib.
    if (sup_curves_[i]->c_type() == C_TYPE_RIB)
        rib_found = true;
    //if any of the sup_curve[] is of dege axial type, it is a dege axial.
    if (sup_curves_[i]->c_type() == C_TYPE_DEGE_AXIAL)
        dege_axial_found = true;
  }

  if (rib_found)
    return C_TYPE_RIB;
  else if (dege_axial_found)
    return C_TYPE_DEGE_AXIAL;
  else
    return C_TYPE_AXIAL;
}

//###############################################################

void clone_ms_curve_sg_sa (dbsk3d_ms_curve* targetSC, dbsk3d_ms_curve* inputSC,
                           dbsk3d_sg_sa* targetSG)
{
  assert (inputSC->data_type() == C_DATA_TYPE_VERTEX);
  assert (inputSC->E_vec().size() == 0);
  assert (inputSC->V_vec().size() != 0);
  assert (targetSC->E_vec().size() == 0);
  assert (targetSC->V_vec().size() == 0);
    
  targetSC->set_length (inputSC->length());
  targetSC->set_c_type (inputSC->c_type());
  targetSC->set_data_type (inputSC->data_type());

  //Clone all fs_vertices.
  for (unsigned int i=0; i<inputSC->V_vec().size(); i++) { 
    const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) inputSC->V_vec (i);
    dbsk3d_fs_vertex* FV2 = targetSG->FV_map (FV->id());
    assert (FV2->id() == FV->id());
    targetSC->add_V_to_back (FV2);
  }  
}

//: for A12A3-II (FE-C) transition.
//  start from the MN, look for the closest A13 fs_vertex
//  (exclude the end points of A1A3 or A14 node element)
dbsk3d_fs_vertex* closest_MN_MC (dbsk3d_ms_node* MN, dbsk3d_ms_curve* MC)
{
  double min_dist = HUGE;
  dbsk3d_fs_vertex* closestN = NULL;
  vcl_vector<dbmsh3d_vertex*> N_vec;
  MC->get_V_vec (N_vec);
  assert (N_vec.size() > 1);

  //Loop through each shock node element of MC and compute distance
  for (unsigned int i=1; i<N_vec.size()-1; i++) {
    dbmsh3d_vertex* V = N_vec[i];
    double dist = vgl_distance (MN->V()->pt(), V->pt());
    if (dist < min_dist) {
      min_dist = dist;
      closestN = (dbsk3d_fs_vertex*) V;
    }
  }
  return (dbsk3d_fs_vertex*) closestN;
}

//: for A1A3-II (FE-C) transition.
//  start from the first MC, look for the closest FV on the next MC.
//  (exclude the end points of them)
double closest_MC_MC (dbsk3d_ms_curve* SC1, dbsk3d_ms_curve* SC2, 
                      dbsk3d_fs_vertex* & closestN1, dbsk3d_fs_vertex* & closestN2)
{
  double min_dist = HUGE;  
  vcl_vector<dbmsh3d_vertex*> SC1_FVs;
  SC1->get_V_vec (SC1_FVs);
  assert (SC1_FVs.size() > 1);
  vcl_vector<dbmsh3d_vertex*> SC2_FVs;
  SC2->get_V_vec (SC2_FVs);
  assert (SC2_FVs.size() > 1);

  //Loop through each FV and compute distance
  for (unsigned int i=1; i<SC1_FVs.size()-1; i++) {
    dbmsh3d_vertex* N1 = SC1_FVs[i];
    for (unsigned int j=1; j<SC2_FVs.size()-1; j++) {
      dbmsh3d_vertex* N2 = SC2_FVs[j];
      double dist = vgl_distance (N1->pt(), N2->pt());
      if (dist < min_dist) {
        min_dist = dist;
        closestN1 = (dbsk3d_fs_vertex*) N1;
        closestN2 = (dbsk3d_fs_vertex*) N2;
      }
    }//end j
  }//end i
  return min_dist;
}

//###############################################################
//       dbsk3d_ms_curve TEXT FILE I/O FUNCTIONS
//###############################################################

void mc_save_text_file (vcl_FILE* fp, dbsk3d_ms_curve* MC)
{
  //For each dbsk3d_ms_curve
  switch (MC->c_type()) {
  case C_TYPE_RIB:
    vcl_fprintf(fp, "A3");
  break;
  case C_TYPE_AXIAL:
    vcl_fprintf(fp, "A13");
  break;
  case C_TYPE_DEGE_AXIAL:
    vcl_fprintf(fp, "Dege");
  break;
  case C_TYPE_VIRTUAL:
    vcl_fprintf(fp, "Virtual");
  break;
  default:
    assert (0);
  break;
  }

  vcl_fprintf (fp, " %d: (%d, %d)\n", MC->id(), MC->sV()->id(), MC->eV()->id());

  //for virtual curve only
  if (MC->c_type() == C_TYPE_VIRTUAL) {
    vcl_fprintf (fp, "\tsup_curves %u:", MC->sup_curves().size());
    for (unsigned int j=0; j<MC->sup_curves().size(); j++) {
      dbmsh3d_curve* C = MC->sup_curves(j);
      vcl_fprintf (fp, " %d", C->id());
    }
    vcl_fprintf (fp, "\n");
  }

  //fine-scale edge elements.
  vcl_fprintf (fp, "\tE %u:", MC->E_vec().size());
  for (unsigned int j=0; j<MC->E_vec().size(); j++) {
    dbmsh3d_edge* E = MC->E_vec(j);
    vcl_fprintf (fp, " %d", E->id());
  }
  vcl_fprintf (fp, "\n");

  //shared edge elements.
  int n_shared_Es = MC->n_shared_Es();
  vcl_fprintf (fp, "\tshared_E %d:", n_shared_Es);
  for (dbmsh3d_ptr_node* cur = MC->shared_E_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    vcl_fprintf (fp, " %d", E->id());
  }
  vcl_fprintf (fp, "\n");

  //fine-scale vertex elements.
  vcl_fprintf (fp, "\tV %u:", MC->V_vec().size());
  for (unsigned int j=0; j<MC->V_vec().size(); j++) {
    const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) MC->V_vec(j);
    vcl_fprintf (fp, " %d", FV->id());
  }
  vcl_fprintf (fp, "\n");

}

void mc_load_text_file (vcl_FILE* fp, dbsk3d_ms_curve* MC, 
                        dbsk3d_fs_mesh* fs_mesh, dbsk3d_ms_hypg* ms_hypg)
{
  char c_type[128];
  vcl_fscanf(fp, "%s", c_type);
  vcl_string type (c_type);
  if (type == "A3")
    MC->set_c_type (C_TYPE_RIB);
  else if (type == "A13")
    MC->set_c_type (C_TYPE_AXIAL);
  else if (type == "Dege")
    MC->set_c_type (C_TYPE_DEGE_AXIAL);
  else if (type == "Virtual")
    MC->set_c_type (C_TYPE_VIRTUAL);
  else
    assert (0);

  //For each dbsk3d_ms_curve
  int id;
  int svid, evid;
  vcl_fscanf (fp, " %d: (%d, %d)\n", &id, &svid, &evid);    
  MC->set_id (id); //Use the id from the file

  //Recover the connectivity.
  assert (MC->c_type() == C_TYPE_RIB || MC->c_type() == C_TYPE_AXIAL ||
          MC->c_type() == C_TYPE_DEGE_AXIAL || MC->c_type() == C_TYPE_VIRTUAL);
  dbsk3d_ms_node* MN = (dbsk3d_ms_node*) ms_hypg->vertexmap (svid);
  assert (MN->id() == svid);
  MC->_set_vertex (0, MN);
  MN = (dbsk3d_ms_node*) ms_hypg->vertexmap(evid);
  assert (MN->id() == evid);
  MC->_set_vertex (1, MN);

  //for virtual curve only
  if (MC->c_type() == C_TYPE_VIRTUAL) {
    int nC;
    vcl_fscanf (fp, "\tsup_curves %d:", &nC);
    for (int j=0; j<nC; j++) {
      vcl_fscanf (fp, " %d", &id);
      dbmsh3d_curve* C = (dbmsh3d_curve*) ms_hypg->edgemap (id);
      MC->add_sup_curve (C);
    }  
    vcl_fscanf (fp, "\n");
  }

  //fine-scale edge elements.
  int nE;
  vcl_fscanf (fp, "\tE %d:", &nE);
  for (int j=0; j<nE; j++) {
    vcl_fscanf (fp, " %d", &id);
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) fs_mesh->edgemap(id);
    MC->add_E_to_back (FE);
  }  
  vcl_fscanf (fp, "\n");

  //shared edge elements.
  int n_shared_Es;
  vcl_fscanf (fp, "\tshared_E %d:", &n_shared_Es);
  for (int j=0; j<n_shared_Es; j++) {
    vcl_fscanf (fp, " %d", &id);
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) fs_mesh->edgemap(id);
    MC->add_shared_E (FE);
  }  
  vcl_fscanf (fp, "\n");

  //fine-scale vertex elements.
  int nV;
  vcl_fscanf (fp, "\tV %d:", &nV);
  for (int j=0; j<nV; j++) {
    vcl_fscanf (fp, " %d", &id);
    const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) fs_mesh->vertexmap(id);
    MC->add_V_to_back (FV);
  }  
  vcl_fscanf (fp, "\n");
}





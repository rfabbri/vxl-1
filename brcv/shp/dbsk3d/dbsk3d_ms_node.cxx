//: This is dbsk3d_scaffold_element_vertex.cxx
//  MingChing Chang
//  Nov 23, 2004.

#include <vcl_cstdio.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vul/vul_printf.h>

#include <dbsk3d/dbsk3d_ms_node.h>
#include <dbsk3d/dbsk3d_ms_curve.h>
#include <dbsk3d/dbsk3d_ms_sheet.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/dbsk3d_sg_sa.h>

//###############################################################
//       dbsk3d_ms_node
//###############################################################

bool dbsk3d_ms_node::check_integrity ()
{
  if (dbmsh3d_node::check_integrity() == false) {
    assert (0);
    return false;
  }

  return true;
}

//: The clone function clones everything except the connectivity
//  when cloning a graph, need to build the connectivity separetely.
dbmsh3d_vertex* dbsk3d_ms_node::clone () const
{
  dbsk3d_ms_node* MN2 = new dbsk3d_ms_node (id_);
  MN2->set_n_type (n_type_);
  MN2->set_radius (radius_);
  return MN2;
}

dbsk3d_ms_node* dbsk3d_ms_node::clone (dbmsh3d_mesh* M) const
{
  dbsk3d_ms_node* MN2 = (dbsk3d_ms_node*) clone ();
  dbmsh3d_vertex* FV2 = M->vertexmap (V_->id());
  MN2->set_V (FV2);
  return MN2;
}

void dbsk3d_ms_node::getInfo (vcl_ostringstream& ostrm)
{
  char s[1024];

  //node type.
  char stype[128];
  switch (n_type_) {
  case N_TYPE_BOGUS: vcl_sprintf (stype, "BUGUS"); break;
  case N_TYPE_RIB_END: vcl_sprintf (stype, "Rib_End"); break;
  case N_TYPE_AXIAL_END: vcl_sprintf (stype, "Axial_End"); break;
  case N_TYPE_DEGE_RIB_END: vcl_sprintf (stype, "Dege_Rib_End"); break;
  case N_TYPE_DEGE_AXIAL_END: vcl_sprintf (stype, "Dege_Axial_End"); break;
  case N_TYPE_LOOP_END: vcl_sprintf (stype, "LoopEnd"); break;
  default: assert(0); break;
  }

  //count incident shock curves.
  int nA3, nA13, nDege, nVirtual;
  count_incident_Cs (nA3, nA13, nDege, nVirtual);

  //node type from incident shock curves.
  char stypec[16];
  int m, n, f;
  dbmsh3d_get_canonical_type (nA3, nA13, nDege, m, n, f);
  if (f!=0) {
    if (nA13==1)
      vcl_sprintf (stypec, "A5(corner)");
    else
      vcl_sprintf (stypec, "A1A3_m%d", f);
  }
  else if (m==0) {
    assert (n>3);
    vcl_sprintf (stypec, "A1%d", n);
  }
  else {
    assert (n==0);
    if (m==1)
      vcl_sprintf (stypec, "A1A3");
    else
      vcl_sprintf (stypec, "A1%dA3", m);
  }

  vcl_sprintf (s, "==============================\n"); ostrm<<s;
  vcl_sprintf (s, "dbsk3d_ms_node (%s %s) id: %d    ", 
               stype, stypec, id_); ostrm<<s;
  bool result = check_integrity();
  vcl_sprintf (s, "check_integrity: %s\n", result ? "pass." : "fail!"); ostrm<<s;
  
  vcl_sprintf (s, " fs_vertex id: %d, radius: %lf\n", V_->id(), radius()); ostrm<<s;  
  
  vcl_set<void*> incident_Es;
  get_incident_Es (incident_Es);

  vcl_sprintf (s, "  # incident edges: %u, total # of edge incidence: %d.\n", 
               incident_Es.size(), n_E_incidence()); ostrm<<s;

  //A3 Rib Curves
  vcl_sprintf (s, "# A3 Rib Curves %d:", nA3); ostrm<<s;
  vcl_set<void*>::iterator it = incident_Es.begin();
  for (; it != incident_Es.end(); it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*it);
    if (MC->c_type()!=C_TYPE_RIB)
      continue;
    vcl_sprintf (s, " %d", MC->id()); ostrm<<s;
  }
  vcl_sprintf (s, "\n"); ostrm<<s;

  //A13 Axial Curves
  vcl_sprintf (s, "# A13 Axial Curves %d:", nA13); ostrm<<s;
  it = incident_Es.begin();
  for (; it != incident_Es.end(); it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*it);
    if (MC->c_type() != C_TYPE_AXIAL)
      continue;
    vcl_sprintf (s, " %d", MC->id()); ostrm<<s;
  }
  vcl_sprintf (s, "\n"); ostrm<<s;

  //Dege Axial Curves
  vcl_sprintf (s, "# Dege Axial Curves %d:", nDege); ostrm<<s;
  it = incident_Es.begin();
  for (; it != incident_Es.end(); it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*it);
    if (MC->c_type() != C_TYPE_DEGE_AXIAL)
      continue;
    vcl_sprintf (s, " %d", MC->id()); ostrm<<s;
  }
  vcl_sprintf (s, "\n"); ostrm<<s;
  
  //Virtual Curves
  vcl_sprintf (s, "# Virtual Curves %d:", nDege); ostrm<<s;
  it = incident_Es.begin();
  for (; it != incident_Es.end(); it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*it);
    if (MC->c_type() != C_TYPE_VIRTUAL)
      continue;
    vcl_sprintf (s, " %d", MC->id()); ostrm<<s;
  }
  vcl_sprintf (s, "\n"); ostrm<<s;

  vcl_sprintf (s, "\n"); ostrm<<s;
}

//########################################################
//  Connectivity Query Functions
//########################################################

/*void dbsk3d_ms_node::count_ms_curves (int& nA3, int& nA13, int& nDege, int& nLC) const
{
  nA3 = 0;
  nA13 = 0;
  nDege = 0;

  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) cur->ptr();

    switch (MC->type()) {
    case C_TYPE_RIB:
      nA3++;
    break;
    case C_TYPE_AXIAL:
      nA13++;
    break;
    case C_TYPE_DEGE_AXIAL:
      nDege++;
    break;
    default:
      assert (0);
    break;
    }
  }
}*/

//: Given two ms_curves, find the other two ms_curves for an A14 ms_vertex.
//  Return false if it is more singular than an A14.
bool dbsk3d_ms_node::A14_get_other_2_MCs (const dbsk3d_ms_curve* MC1, const dbsk3d_ms_curve* MC2,
                                          dbsk3d_ms_curve* & MC3, dbsk3d_ms_curve* & MC4) const
{
  vcl_set<void*> incident_Es;
  get_incident_Es (incident_Es);

  int n_E_inc = n_E_incidence();
  if (n_E_inc > 4)
    return false;

  if (n_E_inc == 2 || n_E_inc == 3)
    return false;

  assert (n_E_inc == 4);
  incident_Es.erase ((void*) MC1);
  incident_Es.erase ((void*) MC2);
  n_E_inc = n_E_V_incidence (incident_Es, this);
  if (n_E_inc != 2)
    return false;

  //For the rare case of MC3 == MC4 in a loop, return false.
  if (incident_Es.size() == 1)
    return false;

  vcl_set<void*>::iterator it = incident_Es.begin();
  MC3 = (dbsk3d_ms_curve*) (*it);
  if (incident_Es.size() == 2) { 
    it++;
    MC4 = (dbsk3d_ms_curve*) (*it);
  }
  else {
    vul_printf (vcl_cout, " ERROR in A14_get_other_2_MCs(). ");
    assert (0);
  }
  return true;
}

bool dbsk3d_ms_node::compute_node_prop (float& grad_r_max, float& grad_r_min,
                                        float& corner_a_max, float& corner_a_min)
{
  grad_r_max = 0;
  grad_r_min = FLT_MAX;
  corner_a_max = 0;
  corner_a_min = FLT_MAX;

  //get all incident sheets.
  vcl_set<dbmsh3d_face*> face_set;
  get_incident_Fs (face_set);

  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) cur->ptr();
    if (MC->c_type() == C_TYPE_VIRTUAL)
      continue;

    //find the edge FE incident to this node.
    dbsk3d_fs_edge* FE;
    assert (MC->E_vec().empty() == false);
    if (MC->s_MN() == this)
      FE = (dbsk3d_fs_edge*) MC->E_vec(0);
    else
      FE = (dbsk3d_fs_edge*) MC->E_vec(MC->E_vec().size()-1);
    assert (FE);

    //get the generator from the incident sheet.
    const dbmsh3d_vertex* G = FE->_get_one_G_via_FF ();
    
    //compute grad_r = dr /ds.
    double dr = vgl_distance (G->pt(), FE->s_FV()->pt()) - vgl_distance (G->pt(), FE->e_FV()->pt());
    double ds = vgl_distance (FE->s_FV()->pt(), FE->e_FV()->pt());
    float grad_r = (float) vcl_fabs (dr / ds);

    if (grad_r < grad_r_min)
      grad_r_min = grad_r;
    if (grad_r > grad_r_max)
      grad_r_max = grad_r;

    vcl_set<dbmsh3d_face*>::iterator it = face_set.begin();
    for (; it != face_set.end(); it++) {
      dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*it);

      //get the first found otherC of MS incident to this node via F.
      dbmsh3d_face* F;
      dbmsh3d_curve* otherC;
      if (MS->get_otherC_via_F (MC, this, otherC, F) == false)
        continue;

      //now we have MC, otherC intersect at fine-scale face F.
      //note that this loop will repeatly visit each corner for 2 times (but this's ok).

      //Find FE2 of otherC.
      dbsk3d_fs_edge* FE2;
      assert (otherC->E_vec().empty() == false);
      if (otherC->s_N() == this)
        FE2 = (dbsk3d_fs_edge*) otherC->E_vec(0);
      else
        FE2 = (dbsk3d_fs_edge*) otherC->E_vec(otherC->E_vec().size()-1);
      assert (FE2);

      //The angle is the vector between (FE, FE2).
      dbmsh3d_vertex* v1 = FE->other_V (V_);
      dbmsh3d_vertex* v2 = FE2->other_V (V_);
      double a = vgl_distance (v1->pt(), v2->pt());
      double b = FE->length();
      double c = FE2->length();
      double angle = vcl_acos ( (b*b + c*c - a*a)/(b*c*2) );
      ///could be wrong: double angle = F->angle_at_bnd_V (V_);

      if (angle < corner_a_min)
        corner_a_min = angle;
      if (angle > corner_a_max)
        corner_a_max = angle;
    }
  }

  return true;
}

//###############################################################
//  Handle Incident Virtual Curves
//###############################################################

//: Given two ms_curves, find the other two ms_curves for an A14 ms_vertex.
//  Return false if it is more singular than an A14.
bool dbsk3d_ms_node::A14_get_other_2_MCs_nv (const dbsk3d_ms_curve* MC1, const dbsk3d_ms_curve* MC2,
                                             dbsk3d_ms_curve* & MC3, dbsk3d_ms_curve* & MC4) const
{
  vcl_set<void*> incident_Es;
  get_incident_Es_nv (incident_Es);

  int n_E_inc = n_E_incidence_nv();
  if (n_E_inc > 4)
    return false;

  if (n_E_inc == 2 || n_E_inc == 3)
    return false;

  assert (n_E_inc == 4);
  incident_Es.erase ((void*) MC1);
  incident_Es.erase ((void*) MC2);
  n_E_inc = n_E_V_incidence (incident_Es, this);
  if (n_E_inc != 2)
    return false;

  //For the rare case of MC3 == MC4 in a loop, return false.
  if (incident_Es.size() == 1)
    return false;

  vcl_set<void*>::iterator it = incident_Es.begin();
  MC3 = (dbsk3d_ms_curve*) (*it);
  if (incident_Es.size() == 2) { 
    it++;
    MC4 = (dbsk3d_ms_curve*) (*it);
  }
  else {
    vul_printf (vcl_cout, " ERROR in A14_get_other_2_MCs(). ");
    assert (0);
  }
  return true;
}

//###############################################################
//  dbsk3d_ms_node TEXT FILE I/O FUNCTIONS
//###############################################################

void mn_save_text_file (vcl_FILE* fp, dbsk3d_ms_node* MN)
{
  switch (MN->n_type()) {
  case N_TYPE_RIB_END:
    vcl_fprintf(fp, "A1A3");
  break;
  case N_TYPE_AXIAL_END:
    vcl_fprintf(fp, "A14");
  break;
  case N_TYPE_DEGE_RIB_END:
    vcl_fprintf(fp, "Dege_Rib_End");
  break;
  case N_TYPE_DEGE_AXIAL_END:
    vcl_fprintf(fp, "Dege_Axial_End");
  break;
  case N_TYPE_LOOP_END:
    vcl_fprintf(fp, "Loop_End");
  break;
  default:
    assert (0);
  break;
  }
  vcl_fprintf(fp, " %d: %d, %.16f\n", MN->id(), MN->V()->id(), MN->radius());

  //Save incident shock scaffold curves.
  vcl_fprintf(fp, "\tcurves %u:", MN->n_incident_Es());
  for (dbmsh3d_ptr_node* cur = MN->E_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) cur->ptr();
    switch (MC->c_type()) {
    case C_TYPE_RIB:
      vcl_fprintf (fp, " R%d", MC->id());
    break;
    case C_TYPE_AXIAL:
      vcl_fprintf (fp, " A%d", MC->id());
    break;
    case C_TYPE_DEGE_AXIAL:
      vcl_fprintf (fp, " D%d", MC->id());
    break;
    case C_TYPE_VIRTUAL:
      vcl_fprintf (fp, " V%d", MC->id());
    break;
    default:
      assert (0);
    break;
    }
  }
  vcl_fprintf(fp, "\n");
}

void mn_load_text_file (vcl_FILE* fp, dbsk3d_ms_node* MN, 
                        ms_node_file_read* mn_fileread, dbsk3d_fs_mesh* fullshock)
{
  char c_type[128];
  vcl_fscanf(fp, "%s", c_type);
  vcl_string type (c_type);
  if (type == "A1A3")
    MN->set_n_type (N_TYPE_RIB_END);
  else if (type == "A14")
    MN->set_n_type (N_TYPE_AXIAL_END);
  else if (type == "Dege_Rib_End")
    MN->set_n_type (N_TYPE_DEGE_RIB_END);
  else if (type == "Dege_Axial_End")
    MN->set_n_type (N_TYPE_DEGE_AXIAL_END);
  else if (type == "Loop_End")
    MN->set_n_type (N_TYPE_LOOP_END);
  else
    assert (0);

  int id, nid;
  float radius;
  vcl_fscanf (fp, " %d: %d, %f\n", &id, &nid, &radius);
  MN->set_id (id);
  mn_fileread->id_ = id;
  
  dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) fullshock->vertexmap(nid);
  assert (FV->id() == nid);
  MN->set_V (FV);
  MN->set_radius (radius);

  //Load incident shock scaffold curves
  int nSC;
  vcl_fscanf (fp, "\tcurves %d:", &nSC);
  for (int j=0; j<nSC; j++) {
    char type;
    int id;
    vcl_fscanf(fp, " %c%d", &type, &id);
    mn_fileread->SCids_.push_back (id);
  }
  vcl_fscanf (fp, "\n");
}

void mn_recover_pointers (dbsk3d_ms_node* MN, ms_node_file_read* mn_fileread,
                          dbsk3d_ms_hypg* ms_hypg)
{
  //Recover the dbsk3d_ms_curve connectivity from SCids_[]
  for (unsigned int j=0; j<mn_fileread->SCids_.size(); j++) {
    int id = mn_fileread->SCids_[j];
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) ms_hypg->edgemap (id);
    MN->add_incident_E (MC);
  }
  mn_fileread->SCids_.clear();
}

void mn_save_text_file_sg (vcl_FILE* fp, dbsk3d_ms_node* MN)
{
  switch (MN->n_type()) {
  case N_TYPE_RIB_END:
    vcl_fprintf(fp, "A1A3");
  break;
  case N_TYPE_AXIAL_END:
    vcl_fprintf(fp, "A14");
  break;
  case N_TYPE_DEGE_RIB_END:
    vcl_fprintf(fp, "Dege_Rib_End");
  break;
  case N_TYPE_DEGE_AXIAL_END:
    vcl_fprintf(fp, "Dege_Axial_End");
  break;
  case N_TYPE_LOOP_END:
    vcl_fprintf(fp, "Loop_End");
  break;
  default:
    assert (0);
  break;
  }
  vcl_fprintf(fp, " %d: %d, %.16f\n", MN->id(), MN->V()->id(), MN->radius());

  //Save incident shock scaffold curves.
  vcl_fprintf(fp, "\tshock curves %u:", MN->n_incident_Es());
  for (dbmsh3d_ptr_node* cur = MN->E_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) cur->ptr();
    switch (MC->c_type()) {
    case C_TYPE_RIB:
      vcl_fprintf (fp, " R%d", MC->id());
    break;
    case C_TYPE_AXIAL:
      vcl_fprintf (fp, " A%d", MC->id());
    break;
    case C_TYPE_DEGE_AXIAL:
      vcl_fprintf (fp, " D%d", MC->id());
    break;
    case C_TYPE_VIRTUAL:
      vcl_fprintf (fp, " V%d", MC->id());
    break;
    }
  }
  vcl_fprintf(fp, "\n");
}

void mn_load_text_file_sg (vcl_FILE* fp, dbsk3d_ms_node* MN, 
                           ms_node_file_read* mn_fileread, dbsk3d_sg_sa* sg_sa)
{
  char c_type[16];
  vcl_fscanf(fp, "%s", c_type);
  vcl_string type (c_type);
  if (type == "A1A3")
    MN->set_n_type (N_TYPE_RIB_END);
  else if (type == "A14")
    MN->set_n_type (N_TYPE_AXIAL_END);
  else if (type == "Dege_Rib_End")
    MN->set_n_type (N_TYPE_DEGE_RIB_END);
  else if (type == "Dege_Axial_End")
    MN->set_n_type (N_TYPE_DEGE_AXIAL_END);
  else if (type == "Loop_End")
    MN->set_n_type (N_TYPE_LOOP_END);
  else
    assert (0);

  int id, nid;
  float radius;
  vcl_fscanf (fp, " %d: %d, %f\n", &id, &nid, &radius);
  MN->set_id (id);
  mn_fileread->id_ = id;
  
  dbsk3d_fs_vertex* FV = sg_sa->FV_map (nid);
  assert (FV->id() == nid);
  MN->set_V (FV);
  MN->set_radius (radius);

  //Load incident shock scaffold curves
  int nSC;
  vcl_fscanf (fp, "\tshock curves %d:", &nSC);
  for (int j=0; j<nSC; j++) {
    char type;
    vcl_fscanf(fp, " %c%d", &type, &id);
    mn_fileread->SCids_.push_back (id);
  }
  vcl_fscanf (fp, "\n");
}


void mn_load_text_file_sg_old (FILE* fp, dbsk3d_ms_node* MN, 
                               ms_node_file_read* mn_fileread,
                               dbsk3d_sg_sa* sg_sa)
{
  char c_type[16];
  vcl_fscanf(fp, "%s", c_type);
  vcl_string type (c_type);
  if (type == "A1A3")
    MN->set_n_type (N_TYPE_RIB_END);
  else if (type == "A14")
    MN->set_n_type (N_TYPE_AXIAL_END);
  else if (type == "Dege")
    MN->set_n_type (N_TYPE_DEGE_AXIAL_END);
  else if (type == "LV")
    MN->set_n_type (N_TYPE_LOOP_END);
  else
    assert (0);

  int id, nid;
  float radius;
  vcl_fscanf (fp, " %d: %d, %f\n", &id, &nid, &radius);
  MN->set_id (id);
  mn_fileread->id_ = id;
  
  dbsk3d_fs_vertex* FV = sg_sa->FV_map (nid);
  assert (FV->id() == nid);
  MN->set_V (FV);
  MN->set_radius (radius);

  //A3RibCurves
  int numA13RibCurves;
  vcl_fscanf (fp, "\tRib    %d:", &numA13RibCurves);
  for (int j=0; j<numA13RibCurves; j++) {
    int id;
    vcl_fscanf(fp, " %d", &id);
    mn_fileread->SCids_.push_back (id);
  }
  vcl_fscanf (fp, "\n");

  //A13AxialCurves
  int numA13AxialCurves;
  vcl_fscanf (fp, "\tAxial  %d:", &numA13AxialCurves);
  for (int j=0; j<numA13AxialCurves; j++) {
    int id;
    vcl_fscanf (fp, " %d", &id);
    mn_fileread->SCids_.push_back (id);
  }
  vcl_fscanf (fp, "\n");
  
  //DegeAxialCurves
  int numDegeAxialCurves;
  vcl_fscanf (fp, "\tDege   %d:", &numDegeAxialCurves);
  for (int j=0; j<numDegeAxialCurves; j++) {
    int id;
    vcl_fscanf (fp, " %d", &id);
    mn_fileread->SCids_.push_back (id);
  }
  vcl_fscanf (fp, "\n");
}

void mn_recover_pointers_sg (dbsk3d_ms_node* MN, ms_node_file_read* mn_fileread,
                             dbsk3d_sg_sa* sg_sa)
{
  //Recover the dbsk3d_ms_curve connectivity from SCids_[]
  for (unsigned int j=0; j<mn_fileread->SCids_.size(); j++) {
    int id = mn_fileread->SCids_[j];
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) sg_sa->edgemap (id);
    MN->check_add_incident_E (MC); //add_incident_E
  }
  mn_fileread->SCids_.clear();
}

//###############################################################
//  Clone functions 
//###############################################################

void clone_ms_vertex_sg_sa (dbsk3d_ms_node* targetSV, dbsk3d_ms_node* inputMN,
                            dbsk3d_sg_sa* targetSG)
{
  assert (targetSV->V() == NULL);
  assert (inputMN->V() != NULL);
    
  targetSV->set_V (inputMN->V());
  targetSV->set_radius (inputMN->radius());
  targetSV->set_n_type (inputMN->n_type());
}


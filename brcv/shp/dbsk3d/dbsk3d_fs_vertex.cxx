// This is dbsk3d_element_node.cxx
//: MingChing Chang
//  Nov. 14, 2004        Creation

#include <vcl_utility.h>
#include <vcl_cstdio.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <vgl/vgl_distance.h>

#include <dbmsh3d/dbmsh3d_pt_set.h>

#include <dbsk3d/dbsk3d_fs_vertex.h>
#include <dbsk3d/dbsk3d_fs_edge.h>
#include <dbsk3d/dbsk3d_fs_face.h>

//###############################################################
//####### Graph connectivity query functions #######
//###############################################################

//: traverse to incident patchElms to see if it's valid
bool dbsk3d_fs_vertex::is_valid_via_FF ()
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) cur->ptr();
    if (FE->is_valid_via_F())
      return true;
  }
  return false;
}

//: Return if this shock node belongs to valid finite (bounded) fs_face
bool dbsk3d_fs_vertex::is_valid_finite_via_FF ()
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) cur->ptr();
    if (FE->is_valid_finite_via_F())
      return true;
  }
  return false;
}

//: get the set of associated generators via FE_set
vcl_set<dbmsh3d_vertex*> dbsk3d_fs_vertex::get_Gs_from_FFs ()
{
  vcl_set<dbmsh3d_vertex*> gene_set;
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) cur->ptr();
    dbmsh3d_halfedge* HE = FE->halfedge();
    do {      
      if (HE == NULL)
        break;
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
      gene_set.insert (FF->genes(0));
      gene_set.insert (FF->genes(1));
      HE = HE->pair();
    }
    while (HE != FE->halfedge());
  }
  return gene_set;
}

int dbsk3d_fs_vertex::n_Gs_from_FFs()
{
  vcl_set<dbmsh3d_vertex*> gene_set = get_Gs_from_FFs();
  return gene_set.size();
}

unsigned int dbsk3d_fs_vertex::count_valid_FEs () const
{
  unsigned int count = 0;
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) cur->ptr();
    if (FE->is_valid_via_F())
      count++;
  }
  return count;
}

unsigned int dbsk3d_fs_vertex::count_valid_FFs () const
{
  vcl_set<dbsk3d_fs_face*> Pset;
  return count_valid_FFs (Pset);
}

unsigned int dbsk3d_fs_vertex::count_valid_FFs (vcl_set<dbsk3d_fs_face*>& Pset) const
{
  Pset.clear();
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) cur->ptr();
    //Loop through each incident FF's.
    dbmsh3d_halfedge* HE = FE->halfedge();
    if (HE == NULL)
      continue;
    else if (HE->pair() == NULL) {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
      if (FF->b_valid())
        Pset.insert (FF);
    }
    else {
      do {
        dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
        if (FF->b_valid())
          Pset.insert (FF);
        HE = HE->pair();
      }
      while (HE != FE->halfedge());
    }
  }
  return Pset.size();
}

//###############################################################
//####### Flow type computation functions #######
//###############################################################

//: Pt_ given from QHull
//  Compute distance from any of the genes
double dbsk3d_fs_vertex::compute_time()
{
  //get the associated Generator from one of the connecting edges
  dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) get_1st_incident_E();
  const dbmsh3d_vertex* G = FE->_get_one_G_via_FF ();
  double time = vgl_distance (pt_, G->pt());
  return time;
}

//  Compute distance from any of the genes
double dbsk3d_fs_vertex::compute_time_ve ()
{
  //get the associated Generator from one of the connecting edges
  dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) get_valid_incident_E();
  const dbmsh3d_vertex* G = FE->_get_one_G_via_FF ();
  assert (G);
  double time = vgl_distance (pt_, G->pt());
  return time;
}

bool dbsk3d_fs_vertex::is_inf_from_FE() const
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) cur->ptr();
    if (FE->b_inf())
      return true;
  }
  return false;
}

//: Go through each connected FE_set and decide type.
int dbsk3d_fs_vertex::n_out_flow_FEs () const
{
  int numInLink = 0;
  int numOutLink = 0;
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) cur->ptr();
    
    vgl_point_3d<double> C;
    FE->compute_circum_cen (C);
    const int L_bidirect = FE->is_flow_bidirect (C);

    if (L_bidirect == 0) { //FE uni-directional
      //FE flows in a single direction.
      //Need to decide FE is inward or outward to this vertex,
      //by comparing time of FE->V[0] and FE->V[1]
      const dbmsh3d_vertex* G = FE->_get_one_G_via_FF ();
      double t = vgl_distance (pt_, G->pt());
      const dbmsh3d_vertex* Vo = FE->other_V (this);
      double to = vgl_distance (Vo->pt(), G->pt());

      if (t < to)
        numOutLink++;
      else {
        assert (t > to);
        numInLink++;
      }
    }
    else      
      numInLink++;
  }

  return numOutLink;
}

FV_FLOW_TYPE dbsk3d_fs_vertex::detect_flow_type ()
{
  if (is_inf_from_FE()) {
    flow_type_ = FV_FT_INF;
    return flow_type_;
  }

  if (n_Gs_from_FFs() == 4)
    return detect_flow_type_A14 ();
  else
    return detect_flow_type_A1n ();
}

FV_FLOW_TYPE dbsk3d_fs_vertex::detect_flow_type_A14 ()
{
  assert (n_incident_Es() == 4);
  dbsk3d_fs_face *FF1, *FF2, *FF3;
  
  vcl_set<dbmsh3d_face*> FF_set;
  get_incident_Fs (FF_set);

  int s1 = compute_s1 (FF_set);
  int s2 = compute_s2 ();

  if (s1 == 6) {
    if (s2 == 4)
      flow_type_ = FV_FT_4641;
    else {
      assert (s2 == 3);
      flow_type_ = FV_FT_4631;
    }
  }
  else if (s1 == 5) {
    if (s2 == 3)
      flow_type_ = FV_FT_4531;
    else {
      assert (s2 == 2);
      flow_type_ = FV_FT_4520;
    }
  }
  else if (s1 == 4) {
    //Check the 2 non-Gabriel edges.
    if (get_2_non_Gabriel_FFs (FF_set, FF1, FF2)) {
      //if (FF1, FF2) sharing a generator G,
      if (FFs_sharing_G (FF1, FF2) != NULL) {
        if (s2==2)
          flow_type_ = FV_FT_4421_O;
        else
          flow_type_ = FV_FT_4410_O;
      }
      else {
        assert (s2==1);
        flow_type_ = FV_FT_4410_P;
      }
    }
    else
      assert (0);
  }
  else if (s1 == 3) {
    //Check the 3 Gabriel edges.
    if (get_3_Gabriel_edges (FF_set, FF1, FF2, FF3)) {
      //if (FF1, FF2, FF3) sharing a generator G,
      if (FFs_sharing_G (FF1, FF2) == FFs_sharing_G (FF2, FF3)) {
        flow_type_ = FV_FT_4300_T;
        assert (s2==0);
      }
      else {
        flow_type_ = FV_FT_4300_L;
        ///assert (s2==0);
      }
    }
    else
      assert (0);
  }
  else {
    ///assert (0);
    flow_type_ = FV_FT_UNCLASSIFIED;
    ///vul_printf (vcl_cout, "  FV %d flow type unclassified. s1 %d, s2 %d.\n", id_, s1, s2);
  }

  return flow_type_;
}

FV_FLOW_TYPE dbsk3d_fs_vertex::detect_flow_type_A1n ()
{
  assert (n_incident_Es() > 4);
  assert (0);
  return flow_type_;
}

//: Compute s1: # shock sheets containing A12-2 sources
int dbsk3d_fs_vertex::compute_s1 (vcl_set<dbmsh3d_face*>& face_set)
{
  int s1 = 0;
  vcl_set<dbmsh3d_face*>::iterator it = face_set.begin();
  for (; it != face_set.end(); it++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it);
    if (FF->flow_type() == FF_FT_I_A12_2)
      s1++;
  }
  return s1;
}

//: Compute s2: # shock curves containing A13-2 saddles.
int dbsk3d_fs_vertex::compute_s2 ()
{
  int s2 = 0;
  //Go through all incident shock curves and count s2.
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) cur->ptr();
    if (FE->flow_type() == FE_FT_I_A13_2_ACUTE)
      s2++;
  }
  return s2;
}

//: Return the two first found Gabriel edges (represented as fs_faces) of the A14 shock node.
bool dbsk3d_fs_vertex::get_2_non_Gabriel_FFs (vcl_set<dbmsh3d_face*>& FF_set,
                                              dbsk3d_fs_face*& FF1, dbsk3d_fs_face*& FF2)
{
  int count = 0;
  vcl_set<dbmsh3d_face*>::iterator it = FF_set.begin();
  for (; it != FF_set.end(); it++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it);
    if (FF->flow_type() != FF_FT_I_A12_2) { //found non-Gabriel edge
      count++;

      if (count==1)
        FF1 = FF;
      else if (count==2) {
        FF2 = FF;
        return true;
      }
    }
  }

  return false;
}

bool dbsk3d_fs_vertex::get_3_Gabriel_edges (vcl_set<dbmsh3d_face*>& FF_set, dbsk3d_fs_face*& FF1, 
                                            dbsk3d_fs_face*& FF2, dbsk3d_fs_face*& FF3)
{
  int count = 0;
  vcl_set<dbmsh3d_face*>::iterator it = FF_set.begin();
  for (; it != FF_set.end(); it++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it);
    if (FF->flow_type() == FF_FT_I_A12_2) { //found Gabriel edge
      count++;

      if (count==1)
        FF1 = FF;
      else if (count==2)
        FF2 = FF;
      else if (count==3) {
        FF3 = FF;
        return true;
      }
    }
  }

  return false;
}

//###############################################################
//###### Handle the assigned generators ######
//###############################################################

//: Get all current associated G's excluding the inputFE and FF's incident ot FE.
void dbsk3d_fs_vertex::get_asso_Gs_excld_FE (const dbsk3d_fs_edge* inputFE, 
                                             vcl_map<int, dbmsh3d_vertex*>& asso_genes)
{
  //Loop through all incident FE of this FV (except inputFE), add FE's asso_genes.
  assert (inputFE);
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) cur->ptr();
    if (FE == inputFE)
      continue;
    FE->get_asso_Gs_incld_FFs (asso_genes);
  }

  //Add all assigned genes of this fs_vertex.
  for (dbmsh3d_ptr_node* cur = asgn_G_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
    asso_genes.insert (vcl_pair<int, dbmsh3d_vertex*> (G->id(), G));
  }
}

//: After undoing a gap xform, need to delete genes asgn. to FE's valid FF's.
void dbsk3d_fs_vertex::del_asgn_G_of_FE (const dbsk3d_fs_edge* FE)
{
  //Loop through FE's valid FF's and remove FF.G[0] and FF.G[1] from FV.asgn_genes.
  dbmsh3d_halfedge* HE = FE->halfedge();
  do {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
    for (unsigned int i=0; i<2; i++)
      del_asgn_G (FF->genes(i));

    HE = HE->pair();
  }
  while (HE != FE->halfedge() && HE != NULL);
}

//: Return true if this fs_vertex is still valid after gap xform on its incident FE.
bool dbsk3d_fs_vertex::valid_after_gap_xform_FE (const dbsk3d_fs_edge* inputFE)
{
  //Loop through all incident fs_edges (except inputFE) and look for valid FF.  
  vcl_set<dbmsh3d_face*> valid_Fset;
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) cur->ptr();
    if (FE==inputFE)
      continue;
    FE->get_valid_Fs (valid_Fset);
  }

  if (valid_Fset.size() == 0)
    return false;

  //Remove all inputFE's FF's from valid_FF_set.
  dbmsh3d_halfedge* HE = inputFE->halfedge();
  do {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
    valid_Fset.erase (FF);
    HE = HE->pair();
  }
  while (HE != inputFE->halfedge() && HE != NULL);

  //If valid_FF_set is non-empty, return true.
  return valid_Fset.size() != 0;
}

//###############################################################
//####### Other functions #######
//###############################################################

//: The clone function clones everything except the connectivity
//  when cloning a graph, need to build the connectivity separetely.
dbmsh3d_vertex* dbsk3d_fs_vertex::clone () const
{
  dbsk3d_fs_vertex* FV2 = new dbsk3d_fs_vertex (id_);
  FV2->set_v_type (v_type());
  FV2->set_pt (pt());
  return FV2;
}

void dbsk3d_fs_vertex::_clone_FV_G_conn (dbsk3d_fs_vertex* FV2, dbmsh3d_pt_set* BND2) const
{
  //deep-copy the gene_list to FV2.
  vcl_vector<dbmsh3d_vertex*> G_list;
  for (dbmsh3d_ptr_node* cur = asgn_G_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
    dbmsh3d_vertex* G2 = BND2->vertexmap (G->id());
    G_list.push_back (G2);
  }
  for (int i=G_list.size()-1; i>=0; i--)
    FV2->add_asgn_G (G_list[i]);
}

dbsk3d_fs_vertex* dbsk3d_fs_vertex::clone (dbmsh3d_pt_set* BND2) const
{
  dbsk3d_fs_vertex* FV2 = (dbsk3d_fs_vertex*) clone ();
  _clone_FV_G_conn (FV2, BND2);
  return FV2;
}

void dbsk3d_fs_vertex::getInfo (vcl_ostringstream& ostrm)
{
  char s[1024];

  vcl_sprintf (s, "\n==============================\n"); ostrm<<s;
  vcl_sprintf (s, "dbsk3d_fs_vertex id: %d (%.16f %.16f %.16f), Time: %lf\n", id_,
               pt_.x(), pt_.y(), pt_.z(), compute_time()); ostrm<<s;
  bool result = check_integrity();
  vcl_sprintf (s, "check_integrity: %s\n\n", result ? "pass." : "fail!"); ostrm<<s;

  char shock_type[128] = "";
  V_TYPE vtype = compute_v_type ();
  switch (vtype) {
  case V_TYPE_MANIFOLD: vcl_sprintf (shock_type, "A12");
  case V_TYPE_RIB_END: vcl_sprintf (shock_type, "A1A3");
  case V_TYPE_AXIAL_END: vcl_sprintf (shock_type, "A14");
  case V_TYPE_RIB: vcl_sprintf (shock_type, "A3");
  case V_TYPE_AXIAL: vcl_sprintf (shock_type, "A13");
  case V_TYPE_DEGE_RIB_END: vcl_sprintf (shock_type, "Dege_Rib_End");
  case V_TYPE_DEGE_AXIAL_END: vcl_sprintf (shock_type, "Dege_Axial_End");
  }
  vcl_set<dbmsh3d_vertex*> geneset = get_Gs_from_FFs ();
  vcl_sprintf (s, "# G: %d, shock tpye: %s\n", geneset.size(), shock_type); ostrm<<s;
  
  //point_genes
  vcl_sprintf (s, "%u asgn. genes.:\n", n_asgn_Gs()); ostrm<<s;
  vcl_set<void*> asgn_genes;
  get_asgn_Gs (asgn_genes);
  vcl_set<void*>::iterator ait = asgn_genes.begin();
  for (; ait != asgn_genes.end(); ait++) {
    dbmsh3d_vertex* G = (dbmsh3d_vertex*) (*ait);
    vcl_sprintf (s, " %d", G->id()); ostrm<<s;
  }

  //point_genes
  vcl_sprintf (s, "\n%u genes. from fs_faces:\n", geneset.size()); ostrm<<s;
  vcl_set<dbmsh3d_vertex*>::iterator git = geneset.begin();
  for (; git != geneset.end(); git++) {
    vcl_sprintf (s, " %d", (*git)->id()); ostrm<<s;
  }

  //fs_edges
  vcl_set<void*> incident_Es;
  get_incident_Es (incident_Es);
  vcl_sprintf (s, "\n%u fs_edges:", incident_Es.size()); ostrm<<s;
  vcl_set<void*>::iterator eit = incident_Es.begin();
  for (; eit != incident_Es.end(); eit++) {  
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*eit);

    vcl_sprintf (s, " %d", FE->id()); ostrm<<s;
    if (FE->is_valid_via_F())
      ostrm<<"v";
  }
  vcl_sprintf (s, "\n"); ostrm<<s;

  //fs_faces
  vcl_set<dbmsh3d_face*> incident_faces;
  get_incident_Fs (incident_faces);

  vcl_sprintf (s, "%u fs_faces:", (unsigned int) incident_faces.size()); ostrm<<s;

  vcl_set<dbmsh3d_face*>::iterator pit = incident_faces.begin();
  for (; pit != incident_faces.end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit);
    vcl_sprintf (s, " %d", FF->id()); ostrm<<s;
    if (FF->b_valid())
      ostrm<<"v";
  }
  vcl_sprintf (s, "\n"); ostrm<<s;

  //: A12, A13, A3, A14, or A1A3 node
  vtype = get_v_type ();
  vcl_sprintf (s, "Shock Type: %c\n", vtype); ostrm<<s;

  vcl_sprintf (s, "\n"); ostrm<<s;
}

//: Old: need to re-write!!
void dbsk3d_fs_vertex::get_incident_elms (vcl_set<dbsk3d_fs_edge*>& FE_set,
                                          vcl_set<dbsk3d_fs_face*>& FF_set)
{
  FE_set.clear();
  FF_set.clear();

  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) cur->ptr();
    FE_set.insert (FE);
    dbmsh3d_halfedge* HE = FE->halfedge();
    do {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
      FF_set.insert (FF); 

      HE = HE->pair();
    }
    while (HE != FE->halfedge());
  }
}

//###############################################################
//####### dbsk3d_fs_vertex TEXT FILE I/O FUNCTIONS #######
//###############################################################

void fv_save_text_file (vcl_FILE* fp, const dbsk3d_fs_vertex* FV)
{
  vcl_fprintf (fp, "v %d (%.16f %.16f %.16f) %c\n", FV->id(), 
               FV->pt().x(), FV->pt().y(), FV->pt().z(),
               (char) FV->v_type());

  //Assigned genes
  unsigned int nG = FV->n_asgn_Gs();
  if (nG == 0) {
    vcl_fprintf (fp, "\t0:");
  }
  else {
    vcl_fprintf (fp, "\t%u:", nG);
    for (dbmsh3d_ptr_node* cur = FV->asgn_G_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
      vcl_fprintf (fp, " %d", G->id());
    }
  }
  vcl_fprintf(fp, "\n");
}

void fv_load_text_file (vcl_FILE* fp, dbsk3d_fs_vertex* FV, 
                        vcl_map <int, dbmsh3d_vertex*>& genemap)
{
  int id;
  double x, y, z;
  V_TYPE type;
  vcl_fscanf (fp, "v %d (%lf %lf %lf) %c\n", &id, &x, &y, &z, &type);
  FV->set_id (id);
  FV->get_pt().set (x, y, z);
  FV->set_v_type (type);

  //Read associated genes  
  unsigned int nG;
  vcl_vector<dbmsh3d_vertex*> asgn_G;
  vcl_fscanf (fp, "\t%u:", &nG);
  for (unsigned int j=0; j<nG; j++) {
    int gid;
    vcl_fscanf (fp, " %d", &gid);
    dbmsh3d_vertex* G = NULL;
    vcl_map <int, dbmsh3d_vertex*>::iterator git = genemap.find (gid);
    if (git != genemap.end()) {
      G = (*git).second;    
      asgn_G.push_back (G);
    }
  }
  vcl_fscanf(fp, "\n");

  //Note that the adding order of FV.asgn_G_ is inversed.
  for (int j=int(asgn_G.size()-1); j>=0; j--)
    FV->add_asgn_G (asgn_G[j]); //Assign generator G to shock node FV
}

void fv_load_text_file_sg (vcl_FILE* fp, dbsk3d_fs_vertex* FV)
{
  int id;
  double x, y, z;
  V_TYPE type;
  vcl_fscanf (fp, "v %d (%lf %lf %lf) %c\n", &id, &x, &y, &z, &type);
  FV->set_id (id);
  FV->get_pt().set (x, y, z);
  FV->set_v_type (type);

  //Read associated genes and ignore them.
  unsigned int nG;
  int gid;
  vcl_fscanf (fp, "\t%u:", &nG);
  for (unsigned int j=0; j<nG; j++)    
    vcl_fscanf (fp, " %d", &gid);
  vcl_fscanf(fp, "\n");
}

void fv_load_text_file_sg_old (vcl_FILE* fp, dbsk3d_fs_vertex* FV)
{
  int id;
  double x, y, z;
  V_TYPE type;
  vcl_fscanf (fp, "n %d (%lf %lf %lf) %c\n", &id, &x, &y, &z, &type);
  FV->set_id (id);
  FV->get_pt().set (x, y, z);
  FV->set_v_type (type);
}


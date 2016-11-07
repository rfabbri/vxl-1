// This is dbsk3d_fs_sheet.cxx
//: MingChing Chang
//  Nov 30, 2006        Creation

#include <vcl_cstdio.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vul/vul_printf.h>

#include <dbsk3d/dbsk3d_fs_sheet.h>
#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_edge.h>
#include <dbsk3d/dbsk3d_fs_vertex.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

//: Get all associated generators of this fs_sheet.
void dbsk3d_fs_sheet::get_asso_Gs (vcl_set<dbmsh3d_vertex*>& Gset,
                                   const bool remove_G_from_L_N)
{
  //Collect genes from all fs_faces and all FEs and FVs.
  vcl_set<dbsk3d_fs_edge*> FE_set;
  vcl_set<dbsk3d_fs_vertex*> FV_set;

  for (dbmsh3d_ptr_node* cur = FF_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();

    Gset.insert (FF->genes(0));
    Gset.insert (FF->genes(1));

    //Go through all incident FE's and FV's of FF.
    dbmsh3d_halfedge* he = FF->halfedge();
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) he->edge();
      FE_set.insert (FE);
      FV_set.insert ((dbsk3d_fs_vertex*) FE->sV());
      FV_set.insert ((dbsk3d_fs_vertex*)FE->eV());
      he = he->next();
    }
    while (he != FF->halfedge());
  }

  //Go through FE_set and add all genes to Gset.
  vcl_set<dbsk3d_fs_edge*>::iterator lit = FE_set.begin();
  for (; lit != FE_set.end(); lit++) {
    dbsk3d_fs_edge* FE = (*lit);      
    for (dbmsh3d_ptr_node* cur = FE->asgn_G_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
      Gset.insert(G);
    }
    if (remove_G_from_L_N)
      FE->clear_asgn_G_list ();
  }

  //Go through FV_set and add all genes to Gset.
  vcl_set<dbsk3d_fs_vertex*>::iterator nit = FV_set.begin();
  for (; nit != FV_set.end(); nit++) {
    dbsk3d_fs_vertex* FV = (*nit);    
    for (dbmsh3d_ptr_node* cur = FV->asgn_G_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
      Gset.insert(G);
    }
    if (remove_G_from_L_N)
      FV->clear_asgn_G_list ();
  }
}

//: Get the associated boundary mesh faces (triangles) of this fs_sheet.
void dbsk3d_fs_sheet::get_bnd_mesh_Fs (vcl_set<dbmsh3d_vertex*>& Gset, vcl_set<dbmsh3d_face*>& Gfaces,
                                       vcl_set<dbmsh3d_face*>& Gfaces2, vcl_set<dbmsh3d_face*>& Gfaces1)
{
  vcl_vector<dbsk3d_fs_face*> fs_faces;
  for (dbmsh3d_ptr_node* cur = FF_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
    fs_faces.push_back (FF);
  }
  FFs_get_bnd_mesh_Fs (fs_faces, Gset, Gfaces, Gfaces2, Gfaces1);
}

//: Collect all boundary A3 and A13 (and higher order) fs_edges.
//  The interior fs_edges are ignored, even if they are of type A13.
void dbsk3d_fs_sheet::get_bnd_FEs (vcl_set<dbsk3d_fs_edge*>& A3_fs_edges, 
                                   vcl_set<dbsk3d_fs_edge*>& A13_fs_edges)
{
  //Make a temporary FF_set for efficient query.
  vcl_set<dbmsh3d_face*> Fset;
  for (dbmsh3d_ptr_node* cur = FF_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
    Fset.insert (FF);      
  }

  //Go through all fs_faces and search for A3 and A13 incident fs_edges.  
  for (dbmsh3d_ptr_node* cur = FF_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
    assert (FF->b_valid());

    //Go through all fs_lik_elms of the current FF.      
    dbmsh3d_halfedge* HE = FF->halfedge();
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
      unsigned int n = FE->count_valid_Fs ();
      if (n==1)
        A3_fs_edges.insert (FE);
      else if (n>2) {
        //Need to check if FE incident to other shock sheets or not.
        if (FE->all_incident_Fs_in_set (Fset) == false)
          A13_fs_edges.insert (FE);
      }

      HE = HE->next();
    }
    while (HE != FF->halfedge());
  }
}

//###################################################################

void dbsk3d_fs_sheet::set_all_FFs_sid ()
{
  for (dbmsh3d_ptr_node* cur = FF_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
    FF->set_sid (id_);
  }
}

void dbsk3d_fs_sheet::set_all_FFs_invalid ()
{
  for (dbmsh3d_ptr_node* cur = FF_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
    FF->set_valid (false);
  }
}

//: Mark all shock patch, link, and node elements to valid.
void dbsk3d_fs_sheet::mark_all_FFs_valid ()
{
  for (dbmsh3d_ptr_node* cur = FF_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
    assert (FF->b_valid());

    //Loop through all incident shock links and set to valid.
    dbmsh3d_halfedge* HE = FF->halfedge();
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
      //Mark shock link FE and its two end nodes to valid
      FE->set_valid (true);
      dbsk3d_fs_vertex* Ls = (dbsk3d_fs_vertex*) FE->sV();
      Ls->set_valid (true);
      dbsk3d_fs_vertex* Le = (dbsk3d_fs_vertex*) FE->eV();
      Le->set_valid (true);      
      HE = HE->next();
    }
    while (HE != FF->halfedge());
  }
}

//: Mark all shock patch, link, and node elements to invalid.
void dbsk3d_fs_sheet::mark_all_FFs_invalid ()
{
  for (dbmsh3d_ptr_node* cur = FF_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
    FF->set_valid (false);

    //Loop through all incident shock links and set invalid.
    dbmsh3d_halfedge* HE = FF->halfedge();
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
      //Mark shock link FE and its two end nodes invalid
      FE->set_valid (false);
      dbsk3d_fs_vertex* Ls = (dbsk3d_fs_vertex*) FE->sV();
      Ls->set_valid (false);
      dbsk3d_fs_vertex* Le = (dbsk3d_fs_vertex*) FE->eV();
      Le->set_valid (false);      
      HE = HE->next();
    }
    while (HE != FF->halfedge());
  }
}

//: Pass all S'generators of S to the remaining fs_edges.
void dbsk3d_fs_sheet::S_pass_Gs (vcl_vector<dbsk3d_fs_edge*>& C_Ls)
{
  assert (C_Ls.size());

  //Collect all genes of S and un-assign them.
  vcl_set<dbmsh3d_vertex*> Gset;
  get_asso_Gs (Gset, true);

  //Loop through each G in GSet and assign to closest FE or FV of C
  vcl_set<dbmsh3d_vertex*>::iterator it = Gset.begin();
  while (it != Gset.end()) {
    dbmsh3d_vertex* G = *it;
    Gset.erase (it);

    //Look for closest FE or FV from G.
    dbsk3d_fs_edge* closestL = NULL;
    dbsk3d_fs_vertex* closestN = NULL;
    get_closest_L_from_G (G, C_Ls, &closestL, &closestN);

    if (closestN != NULL) { //Assign G to closestN.
      closestN->add_asgn_G (G);
    }
    else if (closestL != NULL) { //Assign G to closestL.
      closestL->add_asgn_G (G);
    }
    else
      assert (0);

    it = Gset.begin();
  }
}

void dbsk3d_fs_sheet::clear_incident_LN_asgn_Gs ()
{
  unsigned int n_LG_asgn = 0;
  unsigned int n_NG_asgn = 0;

  //Loop through all fs_faces of this S
  int count = 0;
  for (dbmsh3d_ptr_node* cur = FF_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
    count++;
    //Go through all incident FE's and FV's of FF and clear asgn. genes.
    dbmsh3d_halfedge* he = FF->halfedge();
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) he->edge();
      n_LG_asgn += FE->clear_asgn_G_list ();
      n_NG_asgn += FE->s_FV()->clear_asgn_G_list();
      n_NG_asgn += FE->e_FV()->clear_asgn_G_list();
      he = he->next();
    }
    while (he != FF->halfedge());
  }
  #if DBMSH3D_DEBUG>2
  vul_printf (vcl_cout, "S %d (%u fs_faces): %u L_G_asgn and %u N_G_asgn cleared.\n", 
              id_, count, n_LG_asgn, n_NG_asgn);
  #endif
}

//: Compute the splice cost of the fs_sheet.
//  Cost = # of assigned genes.
void dbsk3d_fs_sheet::compute_splice_cost ()
{
  //Go through all assigned genes and count their number.
  vcl_set<const dbmsh3d_vertex*> genes;

  for (dbmsh3d_ptr_node* cur = FF_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
    assert (FF->b_valid());

    //Skip the fs_face element with zero cost in approximation.
    if (FF->is_splice_cost_zero())
      continue;

    genes.insert (FF->genes(0));
    genes.insert (FF->genes(1));
  }

  cost_ = float (genes.size());
}

bool dbsk3d_fs_sheet::check_integrity ()
{
  //Check the integrity of the coarse-scale and fine-scale elements.  
  for (dbmsh3d_ptr_node* cur = FF_list_; cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
    if (FF == NULL)
      return false;
  }
  return true;
}

void dbsk3d_fs_sheet::getInfo (vcl_ostringstream& ostrm)
{
  char s[1024];

  vcl_sprintf (s, "==============================\n"); ostrm<<s;
  vcl_sprintf (s, "dbsk3d_fs_sheet id: %d    ", id_); ostrm<<s;
  bool result = check_integrity();
  vcl_sprintf (s, "check_integrity: %s\n\n", result ? "pass." : "fail!"); ostrm<<s;

  //Query the boundary A3 and A13 shock curves.
  vcl_set<dbsk3d_fs_edge*> A3_fs_edges; 
  vcl_set<dbsk3d_fs_edge*> A13_fs_edges;
  get_bnd_FEs (A3_fs_edges, A13_fs_edges);

  unsigned int n_total_A3_genes = 0;
  vcl_sprintf (s, "\n# boundary A3 links %u: ", A3_fs_edges.size()); ostrm<<s;
  vcl_set<dbsk3d_fs_edge*>::iterator lit = A3_fs_edges.begin();
  for (; lit != A3_fs_edges.end(); lit++) {
    dbsk3d_fs_edge* FE = *lit;
    vcl_sprintf (s, " %d", FE->id()); ostrm<<s;
    n_total_A3_genes += FE->n_asgn_Gs();
  }
  vcl_sprintf (s, "\n  with %u generators.\n", 
               A3_fs_edges.size(), n_total_A3_genes); ostrm<<s;

  vcl_sprintf (s, "\n# boundary A13 links %u: ", A13_fs_edges.size()); ostrm<<s;
  lit = A13_fs_edges.begin();
  for (; lit != A13_fs_edges.end(); lit++) {
    dbsk3d_fs_edge* FE = *lit;
    vcl_sprintf (s, " %d", FE->id()); ostrm<<s;
  }
  vcl_sprintf (s, "\n"); ostrm<<s;

  //fs_faces
  vcl_set<void*> FF_set;
  unsigned int n_FFs = get_FFs (FF_set);
  vcl_sprintf (s, "\n# fs_faces %u: ", n_FFs); ostrm<<s;
  vcl_set<void*>::iterator pit = FF_set.begin();
  for (; pit != FF_set.end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit);
    vcl_sprintf (s, " %d", FF->id()); ostrm<<s;
  }
  vcl_sprintf (s, "\n"); ostrm<<s;
  
  //All assigned generators.
  vcl_set<dbmsh3d_vertex*> Gset;
  get_asso_Gs (Gset, false);
  vcl_sprintf (s, "\nTotal asso genes %u: ", Gset.size()); ostrm<<s;
  vcl_set<dbmsh3d_vertex*>::iterator git = Gset.begin();
  for (; git != Gset.end(); git++) {
    dbmsh3d_vertex* G = *git;
    vcl_sprintf (s, " %d", G->id()); ostrm<<s;
  }
  vcl_sprintf (s, "\n"); ostrm<<s;

  //All associated bnd_mesh triangles.
  vcl_set<dbmsh3d_face*> Gfaces, Gfaces2, Gfaces1;
  get_bnd_mesh_Fs (Gset, Gfaces, Gfaces2, Gfaces1);
  vcl_sprintf (s, "\nGfaces %u: \n", Gfaces.size()); ostrm<<s;
  vcl_sprintf (s, "Gfaces2 %u: \n", Gfaces2.size()); ostrm<<s;
  vcl_sprintf (s, "Gfaces1 %u: \n", Gfaces1.size()); ostrm<<s;

  vcl_sprintf (s, "\n"); ostrm<<s;
}

//##########################################################################

void get_ifs_faces_pts (vcl_set<dbmsh3d_face*>& Gfaces, vcl_set<dbmsh3d_vertex*>& Gset)
{
  vcl_set<dbmsh3d_face*>::iterator fit = Gfaces.begin();
  for (; fit != Gfaces.end(); fit++) {
    dbmsh3d_face* F = *fit;
    vcl_vector<dbmsh3d_vertex*> vertices;
    F->get_bnd_Vs (vertices);
    for (unsigned int i=0; i<vertices.size(); i++) {
      dbmsh3d_vertex* V = vertices[i];
      Gset.insert (V);
    }
  }
}

//: Given a generator G and a set of fs_edges on a curve,
//  find the closest fs_edge or fs_vertex to G.
void get_closest_L_from_G (const dbmsh3d_vertex* G,
                           const vcl_vector<dbsk3d_fs_edge*>& C_Ls,
                           dbsk3d_fs_edge** closestL,
                           dbsk3d_fs_vertex** closestN)
{
  double min_d = DBL_MAX;

  //Loop through each FE in C_Ls.
  for (unsigned int i=0; i<C_Ls.size(); i++) {
    dbsk3d_fs_edge* FE = C_Ls[i];
    vgl_point_3d<double> S = FE->s_FV()->pt();
    vgl_point_3d<double> E = FE->e_FV()->pt();

    //Project G to FE and measure the foot point F and param. t.
    vgl_vector_3d<double> SG = G->pt() - S;
    vgl_vector_3d<double> SE = E - S;
    double l = SE.length();
    assert (l > 0);
    double t = dot_product (SG, SE) / l;
    double d;

    if (t <= 0) { //closest point is S, d = SG.
      d = vgl_distance (S, G->pt());
      if (d < min_d) {
        min_d = d;
        *closestN = (dbsk3d_fs_vertex*) FE->s_FV();
        *closestL = NULL;
      }
    }
    else if (t >= l) { //closest point is E, d = EG.
      d = vgl_distance (E, G->pt());
      if (d < min_d) {
        min_d = d;
        *closestN = (dbsk3d_fs_vertex*) FE->e_FV();
        *closestL = NULL;
      }
    }
    else { //closest point is F, d = FG.
      vgl_point_3d<double> F = S + SE / l * t;
      d = vgl_distance (F, G->pt());
      if (d < min_d) {
        min_d = d;
        *closestL = FE;
        *closestN = NULL;
      }
    }
  }
}


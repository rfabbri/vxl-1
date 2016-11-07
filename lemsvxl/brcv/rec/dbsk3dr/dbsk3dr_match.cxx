//: This is dbsk3dr/dbsk3dr_match.cxx.
// Graduated Assignment Shock Matching
// MingChing Chang

#include <vcl_ctime.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vul/vul_printf.h>

#include <dbgl/dbgl_rigid_align.h>

#include <dbmsh3dr/dbmsh3dr_match.h>
#include <dbsk3dr/dbsk3dr_match.h>

#include <dbsk3dr/dbsk3dr_ms_curve.h>
#include <dbsk3dr/dbsk3dr_dpmatch.h>

//###### Matching Setup Functions ######  

//: allocate the ga_hypg_G from ms_hypg_G
//
void dbsk3dr_match::alloc_ga_hypg_G (const int verbose)
{
  assert (ms_hypg_G_);
  if (verbose>1) {
    vul_printf (vcl_cout, "alloc_ga_hypg_G():\n");
    vul_printf (vcl_cout, "  allocating %d out of %u nodes.\n", 
                ms_hypg_G_->n_selected_ms_nodes(), ms_hypg_G_->vertexmap().size());
    vul_printf (vcl_cout, "  MN_sid (GA_nid): ");
  }

  dbasnh_hypg_aug* G = new dbasnh_hypg_aug ();
  ga_match_->set_G (G);

  //Add each ms_hypg_G nodes into ga_hypg_G.
  //Also compute hypg node attributes for compatibility match here.
  unsigned int id_counter = 0;
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = ms_hypg_G_->vertexmap().begin();
  for (; vit != ms_hypg_G_->vertexmap().end(); vit++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*vit).second;
    if (MN->selected() == false)
      continue;

    double node_cost = MN->radius();

    int n_rib, n_axial, n_dege, n_virtual;
    MN->count_incident_Cs (n_rib, n_axial, n_dege, n_virtual);
    
    float grad_r_max, grad_r_min, corner_a_max, corner_a_min;
    MN->compute_node_prop (grad_r_max, grad_r_min, corner_a_max, corner_a_min);

    dbasn_node_aug* NA = new dbasn_node_aug (id_counter++, node_cost, MN->n_type(),
                                             n_rib, n_axial, n_dege, MN->pt(),
                                             grad_r_max, grad_r_min, corner_a_max, corner_a_min);
    G_add_ga_node (NA, MN->id());
    if (verbose>1)
      vul_printf (vcl_cout, "%d(%d) ", MN->id(), NA->nid());
  }

  ///assert (ms_hypg_G_->vertexmap().size() == ga_hypg_G()->n_nodes());
  assert (ms_hypg_G_->n_selected_ms_nodes() == ga_hypg_G()->n_nodes());

  //Allocate link[][] space and the corner[][][] table
  if (verbose>1)
    vul_printf (vcl_cout, "\n  allocate links[][] and corners[][][], nN=%d.\n", 
                ga_hypg_G()->n_nodes());

  ga_hypg_G()->alloc_links ();
  ga_hypg_G()->alloc_links_type ();
  ga_hypg_G()->alloc_link_idx ();
  ga_hypg_G()->alloc_corners_angle ();
  ga_hypg_G()->alloc_corner_nf_grad_r ();
}

//: allocate the ga_hypg_g from ms_hypg_g
//
void dbsk3dr_match::alloc_ga_hypg_g (const int verbose)
{
  assert (ms_hypg_g_);
  if (verbose>1) {
    vul_printf (vcl_cout, "alloc_ga_hypg_g():\n");
    vul_printf (vcl_cout, "  allocating %d out of %u nodes.\n", 
                ms_hypg_g_->n_selected_ms_nodes(), ms_hypg_g_->vertexmap().size());
    vul_printf (vcl_cout, "  MN_sid (GA_nid): ");
  }

  dbasnh_hypg_aug* g = new dbasnh_hypg_aug ();
  ga_match_->set_g (g);

  //Add each ms_hypg_g nodes into ga_hypg_g.
  unsigned int id_counter = 0;
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = ms_hypg_g_->vertexmap().begin();
  for (; vit != ms_hypg_g_->vertexmap().end(); vit++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*vit).second;
    if (MN->selected() == false)
      continue;

    double node_cost = MN->radius();
    
    int n_rib, n_axial, n_dege, n_virtual;
    MN->count_incident_Cs (n_rib, n_axial, n_dege, n_virtual);

    float grad_r_max, grad_r_min, corner_a_max, corner_a_min;
    MN->compute_node_prop (grad_r_max, grad_r_min, corner_a_max, corner_a_min);

    dbasn_node_aug* NA = new dbasn_node_aug (id_counter++, node_cost, MN->n_type(),
                                             n_rib, n_axial, n_dege, MN->pt(),
                                             grad_r_max, grad_r_min, corner_a_max, corner_a_min);
    g_add_ga_node (NA, MN->id());
    if (verbose>1)
      vul_printf (vcl_cout, "%d(%d) ", MN->id(), NA->nid());
  }
  ///assert (ms_hypg_g_->vertexmap().size() == ga_hypg_g()->n_nodes());
  assert (ms_hypg_g_->n_selected_ms_nodes() == ga_hypg_g()->n_nodes());

  //Allocate link[][] space and the corner[][][] table
  if (verbose>1)
    vul_printf (vcl_cout, "\n  allocate links[][] and corners[][][], nN=%d.\n", 
                ms_hypg_g_->vertexmap().size());

  ga_hypg_g()->alloc_links ();
  ga_hypg_g()->alloc_links_type ();
  ga_hypg_g()->alloc_link_idx ();
  ga_hypg_g()->alloc_corners_angle ();
  ga_hypg_g()->alloc_corner_nf_grad_r ();
}

void dbsk3dr_match::setup_G_g_dp_curve_dist (const int option, const int verbose)
{
  if (verbose)
    vul_printf (vcl_cout, "setup_G_g_dp_curve_dist(): ");
  int i, j;
  double ed_cost, Eu_cost;
  ///int Gne = (int) ms_hypg_G_->edgemap().size();
  const int Gne = ms_hypg_G_->n_selected_ms_curves();
  ///int gne = (int) ms_hypg_g_->edgemap().size();
  const int gne = ms_hypg_g_->n_selected_ms_curves();

  vcl_map<int, dbmsh3d_edge*>::iterator Geit, geit;
  dbsk3d_ms_curve *GMC, *gMC;

  ga_match_->clear ();
  ga_match_->init_table (Gne, gne);
    
  //option 1: shock graph matching.
  //option 2: shock hypergraph matching using curve length as compatibility.
  //option 3: shock hypergraph matching using curve sum radius over length as compatibility.
  //option 4: shock hypergraph matching using D.P. curve distance as compatibility.
  //option 5: shock hypergraph matching using D.P. shock curve distance as compatibility.
  //option 6: option 5 + plus integration of the global R in node compatibility.
  //option 7: option 3 plus integration of the global (R, T) in node compatibility.
  dbsk3dr_dpmatch dpm;
  if (option==5 || option==6)
    dpm.set_use_radius_DP (true);
  else
    dpm.set_use_radius_DP (false);

  //Fill in distance values in the matrix.
  Geit = ms_hypg_G_->edgemap().begin();
  int cnt = -1;
  for (i=0; Geit != ms_hypg_G_->edgemap().end(); Geit++, i++) {
    GMC = (dbsk3d_ms_curve*) (*Geit).second;
    if (GMC->selected() == false)
      continue;

    cnt++;
    dbsk3dr_ms_curve* RGMC  = new dbsk3dr_ms_curve (GMC, false);
    RGMC->computeProperties();

    geit = ms_hypg_g_->edgemap().begin();
    for (j=0; geit != ms_hypg_g_->edgemap().end(); geit++, j++) {
      gMC = (dbsk3d_ms_curve*) (*geit).second;
      if (gMC->selected() == false)
        continue;

      //Rule out shock curves of different type.
      C_TYPE GMC_type = GMC->get_C_VC_c_type ();
      C_TYPE gMC_type = gMC->get_C_VC_c_type ();
      bool same_c_type = is_same_c_type (GMC_type, gMC_type);

      /*N_TYPE GMC_sN_type = GMC->s_MN()->n_type();
      N_TYPE GMC_eN_type = GMC->e_MN()->n_type();
      N_TYPE gMC_sN_type = gMC->s_MN()->n_type();
      N_TYPE gMC_eN_type = gMC->e_MN()->n_type();
      bool same_n_type = is_same_n_type (GMC_sN_type, gMC_sN_type) &&
                         is_same_n_type (GMC_eN_type, gMC_eN_type);
      bool same_n_type_f = is_same_n_type (GMC_sN_type, gMC_eN_type) &&
                           is_same_n_type (GMC_eN_type, gMC_sN_type);*/

      //Should not penalize for diff. node type ??
      // - already considered in node compatibility.
      // - not all transition done.
      // - curve orientation considered using shock radius.
      //However, the result improves if rule out diff. node type.

      //1) Compute the Gg_ed_dist[][] and Gg_Eu_dist[][].
      //if (same_c_type == false || same_n_type == false) {
      if (same_c_type == false) {
        //Put in DP_DIST_HUGE as dist (GMC, gMC).
        ga_match_->Gg_ed_dist(cnt).push_back (DP_DIST_HUGE);        
        ga_match_->Gg_Eu_dist(cnt).push_back (DP_DIST_HUGE);

        #if DBMSH3D_DEBUG > 2
        vul_printf (vcl_cout, "  Gg_ed_dist_[%d][%d]: HUGE (diff c_type), Ge %d (%d,%d), ge %d (%d,%d).\n", 
                    i, j, GMC->id(), GMC->s_MN()->id(), GMC->e_MN()->id(), gMC->id(), gMC->s_MN()->id(), gMC->e_MN()->id());
        vul_printf (vcl_cout, "  Gg_Eu_dist_[%d][%d]: HUGE (diff c_type).\n", i, j);
        #else
        ///vcl_cout << ".";
        #endif
      }
      else {
        //Compute dist (GMC, gMC) by D.P. matching.      
        dbsk3dr_ms_curve* RgMC  = new dbsk3dr_ms_curve (gMC, false);
        RgMC->computeProperties();
        
        dpm.clear ();
        dpm.setCurve1 (RGMC);
        dpm.setCurve2 (RgMC);
        dpm.Match ();
        
        //edit distance: match G <-> g curves
        ed_cost = dpm.finalCost();

        //Euclidean distance: match G <-> g curves            
        vcl_vector<vgl_point_3d<double> > cor_PS1, cor_PS2;
        Eu_cost = get_curve_align_avg_Eu_dist (GMC, gMC, *(dpm.finalMap()), cor_PS1, cor_PS2);  

        ga_match_->Gg_ed_dist(cnt).push_back (ed_cost);    
        ga_match_->Gg_Eu_dist(cnt).push_back (Eu_cost);

        #if DBMSH3D_DEBUG > 2
        vul_printf (vcl_cout, "  Gg_ed_dist_[%d][%d]: %f, Ge %d (%d,%d), ge %d (%d,%d).\n", 
                    i, j, ed_cost, GMC->id(), GMC->s_MN()->id(), GMC->e_MN()->id(), gMC->id(), gMC->s_MN()->id(), gMC->e_MN()->id());
        vul_printf (vcl_cout, "  Gg_Eu_dist_[%d][%d]: %f.\n", i, j, Eu_cost);
        #else
        //if (verbose)
          //vcl_cout << ".";
        #endif
        
        delete RgMC;
      }
      
      //2) Compute the Gg_ed_dist_f[][] and Gg_Eu_dist_f[][].
      //if (same_c_type == false || same_n_type_f == false) {
      if (same_c_type == false) {
        //Put in DP_DIST_HUGE as dist (GMC, gMC).
        ga_match_->Gg_ed_dist_f(cnt).push_back (DP_DIST_HUGE);
        ga_match_->Gg_Eu_dist_f(cnt).push_back (DP_DIST_HUGE);

        #if DBMSH3D_DEBUG > 2
        vul_printf (vcl_cout, "  Gg_ed_dist_f_[%d][%d]: HUGE (diff c_type).\n", i, j);
        vul_printf (vcl_cout, "  Gg_Eu_dist_f_[%d][%d]: HUGE (diff c_type).\n", i, j);
        #else
        ///vcl_cout << ".";
        #endif
      }
      else {
        dbsk3dr_ms_curve* RgMCf = new dbsk3dr_ms_curve (gMC, true);
        RgMCf->computeProperties();
        
        dpm.clear ();
        dpm.setCurve1 (RGMC);
        dpm.setCurve2 (RgMCf);
        dpm.Match ();
        
        //edit distance: match G <-> g curves flip
        ed_cost = dpm.finalCost();
        
        //Euclidean distance: match G <-> g curves flip        
        vcl_vector<vgl_point_3d<double> > cor_PS1, cor_PS2;
        Eu_cost = get_curve_align_avg_Eu_dist (GMC, gMC, *(dpm.finalMap()), cor_PS1, cor_PS2); 
        
        ga_match_->Gg_ed_dist_f(cnt).push_back (ed_cost);
        ga_match_->Gg_Eu_dist_f(cnt).push_back (Eu_cost);

        #if DBMSH3D_DEBUG > 2
        vul_printf (vcl_cout, "  Gg_ed_dist_f_[%d][%d]: %f.\n", i, j, ed_cost);          
        vul_printf (vcl_cout, "  Gg_Eu_dist_f_[%d][%d]: %f.\n", i, j, Eu_cost);
        #else
        //if (verbose)
          //vcl_cout << ".";
        #endif

        delete RgMCf;
      }
    }

    if (verbose)
      vul_printf (vcl_cout, "%d", cnt);

    delete RGMC;
  }

  if (verbose) {
    vul_printf (vcl_cout, "\n\n");

    //Todo: move these functions into ga_match_ class.
    //Print table:
    assert (ga_match_->Gg_ed_dist().size() == Gne);
    vul_printf (vcl_cout, "Gg_ed_dist[%d][%d]:\n", Gne, gne);
    for (i=0; i<Gne; i++) {
      assert (ga_match_->Gg_ed_dist(i).size() == gne);
      for (j=0; j<gne; j++) {
        if (ga_match_->Gg_ed_dist(i,j) == DP_DIST_HUGE)
          vul_printf (vcl_cout, "HUG ");
        else
          vul_printf (vcl_cout, "%3.0f ", ga_match_->Gg_ed_dist(i,j));
      }  
      vul_printf (vcl_cout, "\n");
    }
    vul_printf (vcl_cout, "\n");

    assert (ga_match_->Gg_Eu_dist().size() == Gne);
    vul_printf (vcl_cout, "Gg_Eu_dist[%d][%d] * 100:\n", Gne, gne);
    for (i=0; i<Gne; i++) {
      assert (ga_match_->Gg_Eu_dist(i).size() == gne);
      for (j=0; j<gne; j++) {
        if (ga_match_->Gg_Eu_dist(i,j) == DP_DIST_HUGE)
          vul_printf (vcl_cout, "HUG ");
        else
          vul_printf (vcl_cout, "%3.0f ", ga_match_->Gg_Eu_dist(i,j)*100);
      }  
      vul_printf (vcl_cout, "\n");
    }
    vul_printf (vcl_cout, "\n");

    assert (ga_match_->Gg_ed_dist_f().size() == Gne);
    vul_printf (vcl_cout, "Gg_ed_dist_f[%d][%d]:\n", Gne, gne);
    for (i=0; i<Gne; i++) {
      assert (ga_match_->Gg_ed_dist_f(i).size() == gne);
      for (j=0; j<gne; j++) {
        if (ga_match_->Gg_ed_dist_f(i,j) == DP_DIST_HUGE)
          vul_printf (vcl_cout, "HUG ");
        else
          vul_printf (vcl_cout, "%3.0f ", ga_match_->Gg_ed_dist_f(i,j));
      }  
      vul_printf (vcl_cout, "\n");
    }
    vul_printf (vcl_cout, "\n");

    assert (ga_match_->Gg_Eu_dist_f().size() == Gne);
    vul_printf (vcl_cout, "Gg_Eu_dist_f[%d][%d] * 100:\n", Gne, gne);
    for (i=0; i<Gne; i++) {
      assert (ga_match_->Gg_Eu_dist_f(i).size() == gne);
      for (j=0; j<gne; j++) {
        if (ga_match_->Gg_Eu_dist_f(i,j) == DP_DIST_HUGE)
          vul_printf (vcl_cout, "HUG ");
        else
          vul_printf (vcl_cout, "%3.0f ", ga_match_->Gg_Eu_dist_f(i,j)*100);
      }  
      vul_printf (vcl_cout, "\n");
    }
    vul_printf (vcl_cout, "\n");
  }

}


//: Option 2: link_cost is each curve's length.
//  Option 3: link_cost is sum of all node's radius.
//            for A13 the value should be further times 3.
void dbsk3dr_match::setup_G_g_curve_similarity (const int option, const int verbose)
{
  dbsk3d_ms_curve* MC;
  double link_cost = 0;
  int nidA, nidB;
  int idx;

  //G: Setup the links[][] and links_type[][] table.
  //The loop curve is considered as regular curves (with diff. end nodes).
  vcl_map<int, dbmsh3d_edge*>::iterator eit = ms_hypg_G_->edgemap().begin();
  idx=-1;
  for (; eit != ms_hypg_G_->edgemap().end(); eit++) {
    MC = (dbsk3d_ms_curve*) (*eit).second;
    if (MC->selected() == false)
      continue;
    idx++;
    assert (idx < ms_hypg_G_->n_selected_ms_curves());

    nidA = G_nid (MC->sV()->id());
    nidB = G_nid (MC->eV()->id());

    //option 2: shock hypergraph matching using curve length as compatibility.
    //option 3: shock hypergraph matching using curve sum radius over length as compatibility.
    //option 7: option 3 plus integration of the global (R, T) in node compatibility.
    if (option==2)
      link_cost = MC->get_length();
    else
      link_cost = MC->get_sum_node_radius ();

    ga_hypg_G()->add_link (nidA, nidB, link_cost);
    C_TYPE c_type = MC->get_C_VC_c_type();
    ga_hypg_G()->add_link_type (nidA, nidB, c_type);
    ga_hypg_G()->add_link_idx (nidA, nidB, idx);
  }

  //g: Setup the links[][] and links_type[][] table.
  //The loop curve is considered as regular curves (with diff. end nodes).
  eit = ms_hypg_g_->edgemap().begin();
  idx=-1;
  for (; eit != ms_hypg_g_->edgemap().end(); eit++) {
    MC = (dbsk3d_ms_curve*) (*eit).second;
    if (MC->selected() == false)
      continue;
    idx++;
    assert (idx < ms_hypg_g_->n_selected_ms_curves());

    nidA = g_nid (MC->sV()->id());
    nidB = g_nid (MC->eV()->id());

    //option 2: shock hypergraph matching using curve length as compatibility.
    //option 3: shock hypergraph matching using curve sum radius over length as compatibility.
    //option 7: option 3 plus integration of the global (R, T) in node compatibility.
    if (option==2)
      link_cost = MC->get_length();
    else
      link_cost = MC->get_sum_node_radius ();

    ga_hypg_g()->add_link (nidA, nidB, link_cost);
    C_TYPE c_type = MC->get_C_VC_c_type();
    ga_hypg_g()->add_link_type (nidA, nidB, c_type);
    ga_hypg_g()->add_link_idx (nidA, nidB, idx);
  }
  
  if (verbose) {
    float G_l_max, G_l_min;
    ga_hypg_G()->get_link_cost_max_min (G_l_max, G_l_min);
    float g_l_max, g_l_min;
    ga_hypg_g()->get_link_cost_max_min (g_l_max, g_l_min);
    vul_printf (vcl_cout, "\n  G: link_count = %d, max = %f, min = %f.\n",
                ga_hypg_G()->n_links(), G_l_max, G_l_min);
    vul_printf (vcl_cout, "  g: link_count = %d, max = %f, min = %f.\n",
                ga_hypg_g()->n_links(), g_l_max, g_l_min);
  }
}

void dbsk3dr_match::setup_G_g_corner_similarity (const int verbose)
{    
  dbsk3d_ms_curve* MC;
  dbsk3d_ms_node *NA, *NB;
  //G: Setup the corners[][][] table.
  //Note that the corner[][][] is oriented.
  vcl_map<int, dbmsh3d_edge*>::iterator eit = ms_hypg_G_->edgemap().begin();
  for (; eit != ms_hypg_G_->edgemap().end(); eit++) {
    MC = (dbsk3d_ms_curve*) (*eit).second;
    if (MC->selected() == false)
      continue;
    if (MC->c_type() == C_TYPE_VIRTUAL)
      continue;
    NA = MC->s_MN();
    NB = MC->e_MN();
    int nidA = G_nid (NA->id());
    int nidB = G_nid (NB->id());
    if (nidA == nidB)
      continue;

    //Known: NA = nodes_[nidA] connect to NB = nodes_[nidB]
    //Check if there is any link from NA to another node NC.
    update_corner_N (ga_hypg_G(), NA, NB, nidA, nidB, MC);
    //Check if there is any link from NB to another node NC.
    update_corner_N (ga_hypg_G(), NB, NA, nidB, nidA, MC);
  }

  //g: Setup the corners[][][] table.
  //Note that the corner[][][] is not oriented.
  eit = ms_hypg_g_->edgemap().begin();
  for (; eit != ms_hypg_g_->edgemap().end(); eit++) {
    MC = (dbsk3d_ms_curve*) (*eit).second;
    if (MC->selected() == false)
      continue;
    if (MC->c_type() == C_TYPE_VIRTUAL)
      continue;
    NA = MC->s_MN();
    NB = MC->e_MN();
    int nidA = g_nid (NA->id());
    int nidB = g_nid (NB->id());
    if (nidA == nidB)
      continue;

    //Known: NA = nodes_[nidA] connect to NB = nodes_[nidB]
    //Check if there is any link from NA to another node NC.
    update_corner_N (ga_hypg_g(), NA, NB, nidA, nidB, MC);
    //Check if there is any link from NB to another node NC.
    update_corner_N (ga_hypg_g(), NB, NA, nidB, nidA, MC);
  }

  if (verbose>1)
    vul_printf (vcl_cout, "\n  G: corner_count = %d, g: corner_count = %d.\n",
                ga_hypg_G()->n_corners(), ga_hypg_g()->n_corners());
}

void dbsk3dr_match::setup_G_g_corner_vl (const int verbose)
{
  dbsk3d_ms_curve* MC;
  dbsk3d_ms_node *NA, *NB;
  //G: Setup the corners[][][] table for the virtual curves.
  vcl_map<int, dbmsh3d_edge*>::iterator eit = ms_hypg_G_->edgemap().begin();
  for (; eit != ms_hypg_G_->edgemap().end(); eit++) {
    MC = (dbsk3d_ms_curve*) (*eit).second;
    if (MC->selected() == false)
      continue;
    if (MC->c_type() != C_TYPE_VIRTUAL)
      continue;

    NA = MC->s_MN();
    NB = MC->e_MN();
    int nidA = G_nid (NA->id());
    int nidB = G_nid (NB->id());
    if (nidA == nidB)
      continue;

    //Known: NA = nodes_[nidA] connect to NB = nodes_[nidB]
    //Check if there is any link from NA to another node NC.
    update_vl_corner_N (ga_hypg_G(), NA, NB, nidA, nidB, MC);
    //Check if there is any link from NB to another node NC.
    update_vl_corner_N (ga_hypg_G(), NB, NA, nidB, nidA, MC);
  }
  
  //g: Setup the corners[][][] table.
  //Note that the corner[][][] is not oriented.
  eit = ms_hypg_g_->edgemap().begin();
  for (; eit != ms_hypg_g_->edgemap().end(); eit++) {
    MC = (dbsk3d_ms_curve*) (*eit).second;
    if (MC->c_type() != C_TYPE_VIRTUAL)
      continue;
    NA = MC->s_MN();
    NB = MC->e_MN();
    int nidA = g_nid (NA->id());
    int nidB = g_nid (NB->id());
    if (nidA == nidB)
      continue;

    //Known: NA = nodes_[nidA] connect to NB = nodes_[nidB]
    //Check if there is any link from NA to another node NC.
    update_vl_corner_N (ga_hypg_g(), NA, NB, nidA, nidB, MC);
    //Check if there is any link from NB to another node NC.
    update_vl_corner_N (ga_hypg_g(), NB, NA, nidB, nidA, MC);
  }

  if (verbose>1)
    vul_printf (vcl_cout, "\n  G: corner_count (with vlinks) = %d, g: corner_count (with vlinks) = %d.\n",
                ga_hypg_G()->n_corners(), ga_hypg_g()->n_corners());
}

double angle_bwn_vector (const vgl_vector_3d<double>& V1, const vgl_vector_3d<double>& V2)
{
  double dot = dot_product (V1, V2);
  double cos_theta = dot / length (V1) / length (V2);
  double theta = vcl_acos (cos_theta);
  assert (vnl_math_isnan(theta) == false);
  return theta;
}

void dbsk3dr_match::update_corner_N (dbasnh_hypg_aug* H, dbsk3d_ms_node* NA, dbsk3d_ms_node* NB, 
                                     const int nidA, const int nidB, dbsk3d_ms_curve* inputMC)
{
  assert (inputMC->c_type() != C_TYPE_VIRTUAL);
  assert (H->links (nidA, nidB) != 0);
  assert (H->links (nidB, nidA) != 0);

  vcl_vector<dbmsh3d_face*> inc_MS;
  inputMC->get_incident_Fs (inc_MS);

  //find the edge FE incident to this node.
  dbsk3d_fs_edge* FE;
  assert (inputMC->E_vec().empty() == false);
  if (inputMC->s_MN() == NA)
    FE = (dbsk3d_fs_edge*) inputMC->E_vec(0);
  else
    FE = (dbsk3d_fs_edge*) inputMC->E_vec(inputMC->E_vec().size()-1);
  assert (FE);

  assert (H == ga_hypg_G() || H == ga_hypg_g());
  bool b_is_G = (H == ga_hypg_G()) ? true : false;

  for (dbmsh3d_ptr_node* cur = NA->E_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) cur->ptr();
    if (MC->selected() == false)
      continue;
    if (MC->c_type() == C_TYPE_VIRTUAL)
      continue;
    if (inputMC == MC)
      continue; //skip the same link as input.

    //Get the other medial scaffold vertex from this MC
    dbsk3d_ms_node* NC;
    if (MC->sV()==NA)
      NC = MC->e_MN();
    else {
      assert (MC->eV() == NA);
      NC = MC->s_MN();
    }
    assert (NC);

    int nidC;
    if (b_is_G)
      nidC = G_nid (NC->id());
    else
      nidC = g_nid (NC->id());

    assert (H->links (nidA, nidC) != 0);
    assert (H->links (nidC, nidA) != 0);

    if (nidA == nidB || nidB == nidC)
      continue; //No corner.

    //Go through all inputMC's incident medial sheets.
    for (unsigned int i=0;i <inc_MS.size(); i++) {
      dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) inc_MS[i];
      //check if the corner C-MC-A-inputMC-B exists in one of inputMC's incident sheets.

      //Find the bnd or internal-curve HE of the given edge.
      //  Return 1 if found in bnd chain.
      //  Return 2 if found in i-curve pair.
      //  Return 3 if found in i-curve loop.
      //  Return 0 if not found.
      dbmsh3d_halfedge* foundHE;
      int r = MS->find_bnd_IC_HE (MC, foundHE);
      if (r == 0)
        continue; //Skip if MS not incident to MC.

      if (H->corners (nidB, nidA, nidC) == 0) {      
        //determine the fine-scael face FF of MS at A        
        //fine-scale corner angle (B, inputMC, A, MC, C).
        
        //Find FE2 of MC.
        dbsk3d_fs_edge* FE2;
        assert (MC->E_vec().empty() == false);
        if (MC->s_MN() == NA)
          FE2 = (dbsk3d_fs_edge*) MC->E_vec(0);
        else
          FE2 = (dbsk3d_fs_edge*) MC->E_vec(MC->E_vec().size()-1);
        assert (FE2);

        //The angle is the vector between (FE, FE2).
        dbmsh3d_vertex* v1 = FE->other_V (NA->V());
        dbmsh3d_vertex* v2 = FE2->other_V (NA->V());
        double a = vgl_distance (v1->pt(), v2->pt());
        double b = FE->length();
        double c = FE2->length();
        double angle = vcl_acos ( (b*b + c*c - a*a)/(b*c*2) );

        //In case angle is zero if FE == FE2, make it epsilon.
        if (angle == 0)
          angle = 0.1;

        //1)
        H->add_corner_angle (nidB, nidA, nidC, angle);

        //2) cost: sum r_i for each face element.
        //old: cost = MS->facemap().size(); //cost: # face elements in MS.
        double cost = 0;
        vcl_map<int, dbmsh3d_face*>::iterator it = MS->facemap().begin();
        for (; it != MS->facemap().end(); it++) {
          dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it).second;
          double r = FF->compute_center_pt_time ();
          cost += r;
        }
        cost *= 2;      
        H->add_corner_nf (nidB, nidA, nidC, cost);

        //3) grad_r = dr / ds.

        const dbmsh3d_vertex* G = FE->_get_one_G_via_FF ();
        double dr = vgl_distance (G->pt(), FE->s_FV()->pt()) - vgl_distance (G->pt(), FE->e_FV()->pt());
        double ds = vgl_distance (FE->s_FV()->pt(), FE->e_FV()->pt());
        double grad_r1 = vcl_fabs (dr / ds);
        
        G = FE2->_get_one_G_via_FF ();
        dr = vgl_distance (G->pt(), FE2->s_FV()->pt()) - vgl_distance (G->pt(), FE2->e_FV()->pt());
        ds = vgl_distance (FE2->s_FV()->pt(), FE2->e_FV()->pt());
        double grad_r2 = vcl_fabs (dr / ds);

        cost = (grad_r1 + grad_r2)/2;
        H->add_corner_grad_r (nidB, nidA, nidC, float(cost));
      }
    }
  }
}

void dbsk3dr_match::update_vl_corner_N (dbasnh_hypg_aug* H, dbsk3d_ms_node* NA, dbsk3d_ms_node* NV, 
                                        const int nidA, const int nidV, dbsk3d_ms_curve* VMC)
{
  assert (VMC->c_type() == C_TYPE_VIRTUAL);
  assert (H->links (nidA, nidV) != 0);
  assert (H->links (nidV, nidA) != 0);

  //Find the supporting curve of this VMC
  dbmsh3d_node* NB;
  dbmsh3d_curve* supMC;
  if (VMC->s_MN() == NA) {
    supMC = VMC->sup_curves (0);
    NB = supMC->e_N();
  }
  else {
    assert (VMC->e_N() == NA);
    supMC = VMC->sup_curves (VMC->sup_curves().size()-1);
    NB = supMC->e_N();
  }
  assert (supMC->c_type() != C_TYPE_VIRTUAL);

  assert (H == ga_hypg_G() || H == ga_hypg_g());
  bool b_is_G = (H == ga_hypg_G()) ? true : false;
  int nidB;
  if (b_is_G)
    nidB = G_nid (NB->id());
  else
    nidB = g_nid (NB->id());
  
  for (dbmsh3d_ptr_node* cur = NA->E_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) cur->ptr(); 
    if (MC->selected() == false)
      continue;
    if (MC->c_type() == C_TYPE_VIRTUAL)
      continue;  
    if (VMC == MC || supMC == MC)
      continue; //skip the same link as input.

    //Get the other medial scaffold vertex from this MC
    dbsk3d_ms_node* NC;
    if (MC->sV()==NA)
      NC = MC->e_MN();
    else {
      assert (MC->eV() == NA);
      NC = MC->s_MN();
    }
    assert (NC);
    int nidC;
    if (b_is_G)
      nidC = G_nid (NC->id());
    else
      nidC = g_nid (NC->id());

    assert (H->links (nidA, nidC) != 0);
    assert (H->links (nidC, nidA) != 0);

    if (nidA == nidB || nidB == nidC)
      continue; //No corner.

    //if corner NC-MC-A-supMC-B exists, put value in NC-MC-A-VMC-B.
    float cost;
    cost = H->corners (nidC, nidA, nidB);
    if (cost != 0)
      H->add_corner_angle (nidC, nidA, nidV, cost);

    cost = H->corner_nf (nidC, nidA, nidB);
    if (cost != 0)
      H->add_corner_nf (nidC, nidA, nidV, cost);

    cost = H->corner_grad_r (nidC, nidA, nidB);
    if (cost != 0)
      H->add_corner_grad_r (nidC, nidA, nidV, cost);
  }
}


// ######################################################################

//: Debug print the final compatibility in tables.
void dbsk3dr_match::print_C_ai_table ()
{
  ///assert (ms_hypg_G()->vertexmap().size() == ga_hypg_G()->nodes().size());
  assert (ms_hypg_G()->n_selected_ms_nodes() == ga_hypg_G()->nodes().size());
  ///assert (ms_hypg_g()->vertexmap().size() == ga_hypg_g()->nodes().size());
  assert (ms_hypg_g()->n_selected_ms_nodes() == ga_hypg_g()->nodes().size());

  vul_printf (vcl_cout, "  print_C_ai_table(): %d * %d (*100).\n", 
              ms_hypg_G()->vertexmap().size(), ms_hypg_g()->vertexmap().size());

  vcl_map<int, dbmsh3d_vertex*>::iterator Nit;
  vcl_map<int, dbmsh3d_vertex*>::iterator nit;

  vul_printf (vcl_cout, "        ");

  nit = ms_hypg_g()->vertexmap().begin();
  for (; nit != ms_hypg_g()->vertexmap().end(); nit++) {
    dbsk3d_ms_node* n = (dbsk3d_ms_node*) (*nit).second;
    if (n->selected() == false)
      continue;
    int i = g_nid (n->id());
    vul_printf (vcl_cout, "n%02d ", n->id());
  }
  vul_printf (vcl_cout, "\n");

  Nit = ms_hypg_G()->vertexmap().begin();
  for (; Nit != ms_hypg_G()->vertexmap().end(); Nit++) {
    dbsk3d_ms_node* N = (dbsk3d_ms_node*) (*Nit).second;
    if (N->selected() == false)
      continue;
    int a = G_nid (N->id());
    vul_printf (vcl_cout, "N%02d(%02d) ", N->id(), a);

    nit = ms_hypg_g()->vertexmap().begin();
    for (; nit != ms_hypg_g()->vertexmap().end(); nit++) {
      dbsk3d_ms_node* n = (dbsk3d_ms_node*) (*nit).second;
      if (n->selected() == false)
        continue;
      int i = g_nid (n->id());
      vul_printf (vcl_cout, "%3.0f ", ga_match_->C_ai (a, i) * 100);
    }
    vul_printf (vcl_cout, "\n");
  }

  vul_printf (vcl_cout, "\n");
  nit = ms_hypg_g()->vertexmap().begin();
  for (; nit != ms_hypg_g()->vertexmap().end(); nit++) {
    dbsk3d_ms_node* n = (dbsk3d_ms_node*) (*nit).second;
    if (n->selected() == false)
      continue;
    int i = g_nid (n->id());
    vul_printf (vcl_cout, "n%02d(%02d) ", n->id(), i);
  }
  vul_printf (vcl_cout, "\n");
  
}

void dbsk3dr_match::print_C_aibj_table ()
{
  ///assert (ms_hypg_G()->edgemap().size() == ga_hypg_G()->n_links());
  assert (ms_hypg_G()->n_selected_ms_curves() == ga_hypg_G()->n_links());
  ///assert (ms_hypg_g()->edgemap().size() == ga_hypg_g()->n_links());
  assert (ms_hypg_g()->n_selected_ms_curves() == ga_hypg_g()->n_links());

  vul_printf (vcl_cout, "\n  print_C_aibj_table(): %d * %d (*100).\n", 
              ms_hypg_G()->edgemap().size(), ms_hypg_g()->edgemap().size());
  
  int a, b, i, j;
  vcl_map<int, dbmsh3d_edge*>::iterator Cit;
  vcl_map<int, dbmsh3d_edge*>::iterator cit;

  vul_printf (vcl_cout, "           ");

  cit = ms_hypg_g()->edgemap().begin();
  for (; cit != ms_hypg_g()->edgemap().end(); cit++) {
    dbsk3d_ms_curve* c = (dbsk3d_ms_curve*) (*cit).second;
    if (c->selected() == false)
      continue;
    i = g_nid (c->s_N()->id());
    j = g_nid (c->e_N()->id());
    vul_printf (vcl_cout, "c%02d ", c->id());
  }
  vul_printf (vcl_cout, "\n");

  //2) Main loop to output the matrix
  Cit = ms_hypg_G()->edgemap().begin();
  for (; Cit != ms_hypg_G()->edgemap().end(); Cit++) {
    dbsk3d_ms_curve* C = (dbsk3d_ms_curve*) (*Cit).second;
    if (C->selected() == false)
      continue;
    a = G_nid (C->s_N()->id());
    b = G_nid (C->e_N()->id());
    vul_printf (vcl_cout, "C%02d[%02d-%02d]", C->id(), C->s_N()->id(), C->e_N()->id());

    cit = ms_hypg_g()->edgemap().begin();
    for (; cit != ms_hypg_g()->edgemap().end(); cit++) {
      dbsk3d_ms_curve* c = (dbsk3d_ms_curve*) (*cit).second;
      if (c->selected() == false)
        continue;
      i = g_nid (c->s_N()->id());
      j = g_nid (c->e_N()->id());
      double comp = ga_match_->C_aibj (a, b, i, j);
      if (comp > 0)
        vul_printf (vcl_cout, " %3.0f", comp * 100);
      else
        vul_printf (vcl_cout, "%4.0f", comp * 100);
    }
    vul_printf (vcl_cout, "\n");
  }

  vul_printf (vcl_cout, "\n");
  cit = ms_hypg_g()->edgemap().begin();
  for (; cit != ms_hypg_g()->edgemap().end(); cit++) {
    dbsk3d_ms_curve* c = (dbsk3d_ms_curve*) (*cit).second;
    if (c->selected() == false)
      continue;
    i = g_nid (c->s_N()->id());
    j = g_nid (c->e_N()->id());
    vul_printf (vcl_cout, "c%02d[%02d-%02d] ", c->id(), c->s_N()->id(), c->e_N()->id());
  }
  vul_printf (vcl_cout, "\n");

  vul_printf (vcl_cout, "\n  print_C_ajbi_table (flip): %d * %d (*100).\n", 
              ms_hypg_G()->edgemap().size(), ms_hypg_g()->edgemap().size());  
  vul_printf (vcl_cout, "           ");

  cit = ms_hypg_g()->edgemap().begin();
  for (; cit != ms_hypg_g()->edgemap().end(); cit++) {
    dbsk3d_ms_curve* c = (dbsk3d_ms_curve*) (*cit).second;
    if (c->selected() == false)
      continue;
    i = g_nid (c->s_N()->id());
    j = g_nid (c->e_N()->id());
    vul_printf (vcl_cout, "f%02d ", c->id());
  }
  vul_printf (vcl_cout, "\n");

  //3) Main loop to output the flipped matrix
  Cit = ms_hypg_G()->edgemap().begin();
  for (; Cit != ms_hypg_G()->edgemap().end(); Cit++) {
    dbsk3d_ms_curve* C = (dbsk3d_ms_curve*) (*Cit).second;
    if (C->selected() == false)
      continue;
    a = G_nid (C->s_N()->id());
    b = G_nid (C->e_N()->id());
    vul_printf (vcl_cout, "C%02d[%02d-%02d]", C->id(), C->s_N()->id(), C->e_N()->id());

    cit = ms_hypg_g()->edgemap().begin();
    for (; cit != ms_hypg_g()->edgemap().end(); cit++) {
      dbsk3d_ms_curve* c = (dbsk3d_ms_curve*) (*cit).second;
      if (c->selected() == false)
        continue;
      i = g_nid (c->s_N()->id());
      j = g_nid (c->e_N()->id());
      double comp = ga_match_->C_aibj (a, b, j, i);
      if (comp > 0)
        vul_printf (vcl_cout, " %3.0f", comp * 100);
      else
        vul_printf (vcl_cout, "%4.0f", comp * 100);
    }
    vul_printf (vcl_cout, "\n");
  }
}

//: Debug print the final compatibility in tables.
void dbsk3dr_match::print_C_aibjck_table ()
{
  vul_printf (vcl_cout, "\n  print_C_aibjck_table(): %d * %d (*100).\n", 
              ga_match_->hypg_G()->n_corners(), ga_match_->hypg_g()->n_corners());

  int a, b, c, i, j, k;
  int sa, sb, sc, si, sj, sk;
  const int GnN = (int) ga_match_->hypg_G()->nodes().size();
  const int gnN = (int) ga_match_->hypg_g()->nodes().size();

  vul_printf (vcl_cout, "            ");       
  for (int i=0; i<gnN-1; i++) {
    for (int j=0; j<gnN; j++) {
      for (int k=i+1; k<gnN; k++) {
        sj = g_sid (j);
        if (ga_match_->hypg_g()->corners (i, j, k) == 0)
          continue;

        vul_printf (vcl_cout, "c%02d ", sj);
      }
    }
  }
  vul_printf (vcl_cout, "\n");

  //2) Main loop to output the matrix
  //   The corner table is symmetric at <a to c> and <i to k>.
  //for all corners (a, b, c) in G.

  for (int a=0; a<GnN-1; a++) {
    for (int b=0; b<GnN; b++) {
      for (int c=a+1; c<GnN; c++) {
        if (ga_match_->hypg_G()->corners (a, b, c) == 0)
          continue;

        sa = G_sid (a);
        sb = G_sid (b);
        sc = G_sid (c);
        vul_printf (vcl_cout, "C[%02d-%02d-%02d]", sa, sb, sc);

        //for all corners (i, j, k) in g.      
        for (i=0; i<gnN-1; i++) {
          for (j=0; j<gnN; j++) {
            for (k=i+1; k<gnN; k++) {
              if (ga_match_->hypg_g()->corners (i, j, k) == 0)
                continue;

              double cost = ga_match_->C_aibjck (a, b, c, i, j, k);
              vul_printf (vcl_cout, " %3.0f", cost * 100);
            }
          }
        } //end all corners (i, j, k) in g.

        vul_printf (vcl_cout, "\n");
      }
    }
  }
       
  vul_printf (vcl_cout, "\n");
  for (i=0; i<gnN-1; i++) {
    for (j=0; j<gnN; j++) {
      for (k=i+1; k<gnN; k++) {
        si = g_sid (i);
        sj = g_sid (j);
        sk = g_sid (k);
        if (ga_match_->hypg_g()->corners (i, j, k) == 0)
          continue;

        vul_printf (vcl_cout, "c%02d[%02d-%02d] ", sj, si, sk);
      }
    }
  }
  vul_printf (vcl_cout, "\n");
}


bool dbsk3dr_match::compute_matching (const int verbose)
{
  if (verbose) {
    vcl_cout<< "\n  Running Graduate Assignment Shock Hypergraph Matching.\n\t";
    vcl_cout<< "G: "<< ga_hypg_G()->n_nodes() <<" nodes, "<<
               "g: "<< ga_hypg_g()->n_nodes() <<" nodes.\n\n";
    if (ga_hypg_g()->n_nodes() > ga_hypg_G()->n_nodes()) {
      vul_printf (vcl_cout, "\n\t Graduated Assignment warning (g %d is bigger than G %d)!\n\n",
                  ga_hypg_g()->n_nodes(), ga_hypg_G()->n_nodes());
    }
  }

  ga_match_->get_assignment ();

  //debug: print_C_aibj ();
  //debug: print_N_ai_array (true);
  if (verbose)
    print_match_results ();

  ga_match_->get_similarity (verbose);

  return ga_match_->num_stable();
}

void dbsk3dr_match::print_match_results ()
{
  if (ga_match_->num_stable())
    vul_printf (vcl_cout, "\n  Shock hypergraph matching successful:\n");
  else
    vul_printf (vcl_cout, "\n  Shock hypergraph matching WITH EXPONENTIAL EXPLOSION:\n");

  vul_printf (vcl_cout, "    ms_hypg_G : %d nodes, %d curves, %d sheets.\n",
              ms_hypg_G_->vertexmap().size(),
              ms_hypg_G_->edgemap().size(),            
              ms_hypg_G_->sheetmap().size());
  vul_printf (vcl_cout, "    ga_hypg_G : %d nodes, %d links, %d corners.\n",
              ga_hypg_G()->n_nodes(),
              ga_hypg_G()->n_links(),            
              ga_hypg_G()->n_corners());

  vul_printf (vcl_cout, "    ms_hypg_g : %d nodes, %d curves, %d sheets.\n",
              ms_hypg_g_->vertexmap().size(),
              ms_hypg_g_->edgemap().size(),            
              ms_hypg_g_->sheetmap().size());
  vul_printf (vcl_cout, "    ga_hypg_g : %d nodes, %d links, %d corners.\n",
              ga_hypg_g()->n_nodes(),
              ga_hypg_g()->n_links(),            
              ga_hypg_g()->n_corners());

  ///assert (ms_hypg_G_->vertexmap().size() == ga_hypg_G()->n_nodes());
  assert (ms_hypg_G_->n_selected_ms_nodes() == ga_hypg_G()->n_nodes());
  ///assert (ms_hypg_g_->vertexmap().size() == ga_hypg_g()->n_nodes());
  assert (ms_hypg_g_->n_selected_ms_nodes() == ga_hypg_g()->n_nodes());

  vul_printf (vcl_cout, "  Gsid(nid) <-> gsid(nid):\n");

  for (int a=0; a<ga_match_->M_row()-1; a++) {
    int i = ga_match_->labelGg (a);
    int Gsid = G_sid (a);
    if (i == -1) //slack!
      vul_printf (vcl_cout, "    %d(%d) <-> no match\n", Gsid, a);
    else {
      int gsid = g_sid (i);
      vul_printf (vcl_cout, "    %d(%d) <-> %d(%d)\n", Gsid, a, gsid, i);
    }
  }
}

int dbsk3dr_match::matched_G_sid (int gsid)
{
  for (int a=0; a<ga_match_->M_row()-1; a++) {
    int i = ga_match_->labelGg (a);
    if (i == -1)
      return -1;

    if (g_sid(i) == gsid)
      return G_sid (a);
  } 
  return -1; //for no match, return -1. the a -> i mapping is not onto!
}

//: Given dbsk3d_ms_curve id in graph_1, return the matching dbsk3d_ms_curve id in graph_2
//  if there is no match (any ending vertex not matched), return -1
dbsk3d_ms_curve* dbsk3dr_match::matched_g_curve (dbsk3d_ms_curve* G_curve, bool& flip)
{
  int a = G_nid (G_curve->sV()->id());
  int b = G_nid (G_curve->eV()->id());
  int i = ga_match_->labelGg (a);
  int j = ga_match_->labelGg (b);
  flip = false;

  if (i==-1 || j==-1)
    return NULL; //no match

  //now check if i-j are an actual link in ms_g
  if (ga_hypg_g()->links(i,j) <= 0)
    return NULL;

  //search for the matched dbsk3d_ms_curve
  dbsk3d_ms_node* Ni = (dbsk3d_ms_node*) ms_hypg_g_->vertexmap (g_sid (i));
  dbsk3d_ms_node* Nj = (dbsk3d_ms_node*) ms_hypg_g_->vertexmap (g_sid (j));

  for (dbmsh3d_ptr_node* cur = Ni->E_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) cur->ptr();
    if (MC->selected() == false)
        continue;
    if (MC->sV() == Ni && MC->eV() == Nj) { //found it!
      flip = false;
      return MC;
    }
    if (MC->sV() == Nj && MC->eV() == Ni) { //found it!
      flip = true;
      return MC;
    }
  }

  assert (0);
  return NULL;
}


// ######################################################################

void dbsk3dr_match::test_mc_dp_match (const int option, const int cid1, const int cid2, const bool flip,
                                      vcl_vector< vcl_pair<int,int> >& alignment)
{
  dbsk3d_ms_curve* MC1 = (dbsk3d_ms_curve*) ms_hypg_G_->edgemap (cid1);
  dbsk3d_ms_curve* MC2 = (dbsk3d_ms_curve*) ms_hypg_g_->edgemap (cid2);

  if (flip == false)
    vul_printf (vcl_cout, "\nMatching G_C %d (N%d-N%d, e:%d) to g_C %d (N%d-N%d, e:%d).\n",
                MC1->id(), MC1->s_MN()->id(), MC1->e_MN()->id(), MC1->E_vec().size(), 
                MC2->id(), MC2->s_MN()->id(), MC2->e_MN()->id(), MC2->E_vec().size());
  else
    vul_printf (vcl_cout, "\nMatching G_C %d (N%d-N%d, e:%d) to g_C %d flip (N%d-N%d, e:%d).\n",
                MC1->id(), MC1->s_MN()->id(), MC1->e_MN()->id(), MC1->E_vec().size(), 
                MC2->id(), MC2->e_MN()->id(), MC2->s_MN()->id(), MC2->E_vec().size());
  
  dbsk3dr_ms_curve* RC1 = new dbsk3dr_ms_curve (MC1, false);
  dbsk3dr_ms_curve* RC2 = new dbsk3dr_ms_curve (MC2, flip);

  //option 1 : test 3d curve matching.
  //option 2 : test 3d shock curve matching.
  dbsk3dr_dpmatch* dpm = new dbsk3dr_dpmatch (option);
  dpm->setCurve1 (RC1);
  RC1->computeProperties ();
  dpm->setCurve2 (RC2);
  RC2->computeProperties ();

  dpm->Match ();

  vul_printf (vcl_cout, "\tFinal cost (edit distance): %f %s\n", 
              dpm->finalCost(), flip ? "(flip)" : "");
  //dpm->ListDPTable ();
  dpm->ListAlignCurve ();

  alignment.clear();
  for (unsigned int i=0; i<dpm->finalMap()->size(); i++) {
    vcl_pair<int,int> pair = (*(dpm->finalMap()))[i];
    alignment.push_back (pair);
  }

  delete dpm;
  delete RC1;
  delete RC2;
}

// ######################################################################

//Rigidly rotate and translate ms_G to match ms_g
void dbsk3dr_match::get_rigid_xform_matrices (const bool node_only,  const int verbose)
{
  if (verbose)
    vcl_cout<< "\n  get_rigid_xform_matrices()\n";

  //1)Put all nodes in ms_G into the point set cor_movPS.
  //  and all nodes in ms_g into the point set cor_fixPS.
  vcl_vector<vgl_point_3d<double> > cor_movPS, cor_fixPS;

  for (int a=0; a<ga_match_->M_row()-1; a++) {
    int i = ga_match_->labelGg (a);
    int Gsid = G_sid (a);
    int gsid = g_sid (i);
    if (Gsid==-1 || gsid==-1)
      continue; //ignore incomplete match.

    //Put int P1(x, y, z): the fixed one to be registered to.
    dbsk3d_ms_node* Na = G_ms_node (a);
    assert (Na->selected());
    cor_movPS.push_back (Na->FV()->pt());

    //Put int P2(x, y, z)
    dbsk3d_ms_node* Ni = g_ms_node (i);
    assert (Ni->selected());
    cor_fixPS.push_back (Ni->FV()->pt());
  } 

  if (node_only == false) {
    //2) linearly assign each matched curve and put in each node_elm of curves as correspondence.
    vcl_map<int, dbmsh3d_edge*>::iterator SC_it = ms_hypg_G()->edgemap().begin();
    for (; SC_it != ms_hypg_G()->edgemap().end(); SC_it++) {
      dbsk3d_ms_curve* G_MC = (dbsk3d_ms_curve*) (*SC_it).second;
      if (G_MC->selected() == false)
        continue;
      bool flip;
      dbsk3d_ms_curve* g_MC = matched_g_curve (G_MC, flip);
      if (g_MC == NULL)
        continue;
      assert (g_MC->selected());

      //if there is a match, put corresponding node elements of this ms_curve into sets.
      vcl_vector<dbmsh3d_vertex*> G_MC_V_vec;
      vcl_vector<dbmsh3d_vertex*> g_MC_V_vec;
      G_MC->get_V_vec (G_MC_V_vec);
      g_MC->get_V_vec (g_MC_V_vec);
      int fi, ii;
      dbmsh3d_vertex *Va, *Vi;
      
      for (unsigned int i=0; i<G_MC_V_vec.size(); i++) {
        Va = G_MC_V_vec[i];
        cor_movPS.push_back (Va->pt());
        //round (a) = floor (a + 0.5).
        ii = vcl_floor (double(i)*g_MC_V_vec.size()/G_MC_V_vec.size() + 0.5);
        fi = flip ? g_MC_V_vec.size()-ii : ii;

        if (fi >= int(g_MC_V_vec.size()))
          fi = int(g_MC_V_vec.size())-1;
        if (fi < 0)
          fi = 0;
        Vi = g_MC_V_vec[fi];
        cor_fixPS.push_back (Vi->pt());
      }
    }

  }

  //3)Compute the centroids and rotation matrix R, such that
  //  it matches P1 to P2 (P2 fixed). The vector size are the same.
  dbgl_get_rigid_alignment (cor_fixPS, cor_movPS, R_, Cf_, Cm_);
}

double dbsk3dr_match::get_curve_align_avg_Eu_dist (dbmsh3d_curve* C1, dbmsh3d_curve* C2, 
                                                   vcl_vector< vcl_pair<int,int> >& alignment,
                                                   vcl_vector<vgl_point_3d<double> >& cor_PS1, 
                                                   vcl_vector<vgl_point_3d<double> >& cor_PS2)
{
  unsigned int i;
  //Put all nodes in C1 into the point set cor_movPS.
  //and all nodes in C2 into the point set cor_fixPS.
  //in the order specified in the alignment[].
  vcl_vector<dbmsh3d_vertex*> C1_V_vec, C2_V_vec;
  C1->get_V_vec (C1_V_vec);
  C2->get_V_vec (C2_V_vec);

  vcl_vector<vgl_point_3d<double> > cor_movPS, cor_fixPS;
  for (i=0; i<alignment.size(); i++) {
    int id1 = alignment[i].first;
    int id2 = alignment[i].second;
    cor_PS1.push_back (C1_V_vec[id1]->pt());
    cor_movPS.push_back (C1_V_vec[id1]->pt());
    cor_PS2.push_back (C2_V_vec[id2]->pt());
    cor_fixPS.push_back (C2_V_vec[id2]->pt());
  }

  //Match P1 to P2 (P2 fixed). The vector size are the same.
  dbgl_get_rigid_alignment (cor_fixPS, cor_movPS, R_, Cf_, Cm_);  
  ///print_R_C1_C2 (R_, Cf_, Cm_);

  //The cor_fixPS[] and cor_movPS[] is now shifted to their object centers.
  //Rigid transform cor_movPS to cor_fixPS.
  for (i=0; i<cor_movPS.size(); i++) {
    transform_point_3d (R_, Cf_, Cm_, cor_movPS[i]);
  }
  
  //Transform the cor_PS1[].
  for (i=0; i<cor_PS1.size(); i++)
    transform_point_3d (R_, Cf_, Cm_, cor_PS1[i]);

  //Compute the average Euclidean dist directly from the assignment (align. curve).
  double avg_align_dist = 0;
  for (i=0; i<cor_PS1.size(); i++) {
    double d = vgl_distance (cor_PS1[i], cor_PS2[i]);
    avg_align_dist += d;
  }
  avg_align_dist /= cor_PS1.size();
  //vul_printf (vcl_cout, "avg_align_dist: %f, ", avg_align_dist);

  //Compute the final average Euclidean distance between closest sample points (from PS1 to PS2).
  /*vcl_vector<double> min_dists;
  vcl_vector<int> min_ids;  
  compute_pp_min_dist (cor_PS1, cor_PS2, min_dists, min_ids);
  double avg_min_dist = 0;
  for (i=0; i<min_dists.size(); i++)
    avg_min_dist += min_dists[i];
  avg_min_dist /= min_dists.size();
  vul_printf (vcl_cout, "avg_min_dist: %f, ", avg_min_dist);*/

  //Register C1 to C2 using ICP from the current position.
  /*int nMaxIter = 20;
  float conv_th = 0.0001f;
  vgl_h_matrix_3d<double> H;
  H.set_identity ();
  bool r = dbmsh3dr_pp_icp_regstr (cor_PS2, cor_PS1, nMaxIter, conv_th, FLT_MAX, H);
  
  //Compute the final average Euclidean distance between closest sample points (from PS1 to PS2).
  compute_pp_min_dist (cor_PS1, cor_PS2, min_dists, min_ids);
  double avg_min_dist_ICP = 0;
  for (i=0; i<min_dists.size(); i++)
    avg_min_dist_ICP += min_dists[i];
  avg_min_dist_ICP /= min_dists.size();
  vul_printf (vcl_cout, "avg_min_dist_ICP: %f.\n", avg_min_dist_ICP);*/

  //The analysis shows the avg_align_dist is the best in terms of matching results.
  return avg_align_dist;
}

void dbsk3dr_match::transform_scaffold_graph_1_to_2 ()
{
  //3-1)Go through all FVs of SG and transform (move) it.  
  ///vcl_map<int, dbsk3d_fs_vertex*>::iterator it = ms_hypg_G_->FV_map().begin();
  vcl_map<int, dbmsh3d_vertex*>::iterator it = ms_hypg_G_->fs_mesh()->vertexmap().begin();
  for (; it != ms_hypg_G_->fs_mesh()->vertexmap().end(); it++) {
    dbsk3d_fs_vertex* V = (dbsk3d_fs_vertex*) (*it).second;
    transform_point_3d (R_, Cf_, Cm_, V->get_pt());
  }
}

void dbsk3dr_match::transform_point_G_1_to_2 (dbmsh3d_mesh* M)
{
  vcl_map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (int i=0; it != M->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* v = (*it).second;
    transform_point_3d (R_, Cf_, Cm_, v->get_pt());
  }
}

/*void dbsk3dr_match::SaveMatchFile (const char* matchFile)
{
  FILE* fp;

  vcl_cout<< "\nWriting Shock Matching File... "<< matchFile <<vcl_endl;
  if ((fp = fopen(matchFile, "w")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open output matchFile %s.\n", matchFile);
    return;
  }

  vcl_fprintf (fp, "ms_G: %s (%d nodes, %d links, %d corners)\n", 
        ShockGraph1_->n_nodes(), 
        -1, 
        ShockGraph1_->nCorners());
  vcl_fprintf (fp, "ms_g: %s (%d nodes, %d links, %d corners)\n", 
        ShockGraph2_->n_nodes(), 
        -1, 
        ShockGraph2_->nCorners());

  assert (ShockGraph1_->n_nodes() == M_row_-1);
  vcl_fprintf (fp, "dbsk3dr_ms_gradasn_graph Assignment GSid(Gnid) <-> gSid(gnid):\n");
  for (int a=0; a<M_row_-1; a++) {
    int i = labelGg_[a];
    int GSid = shockIdGg_[a][0];
    int gSid = shockIdGg_[a][1];
    vcl_fprintf (fp, "%d(%d) <-> %d(%d)\n", GSid, a, gSid, i);
  }

  fclose (fp);
}*/

/*bool dbsk3dr_match::LoadMatchFile (const char* matchFile)
{
  FILE* fp;
  char SCFile[1024];
  int nNodes1, nLinks1, nCorners1;
  int nNodes2, nLinks2, nCorners2;

  if ((fp = fopen(matchFile, "r")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open matchFile %s\n",
          matchFile);
    return false; 
  }
  vul_printf (vcl_cout, "\nMESG(LoadMatchFile): in MatchFile: %s\n", matchFile);
  matchFile_ = vcl_string (matchFile);

  vcl_fscanf (fp, "ms_G: %s (%d nodes, %d links, %d corners)\n", SCFile, 
        &nNodes1, &nLinks1, &nCorners1);
  fileName1_ = vcl_string (SCFile);
  vul_printf (vcl_cout, "ms_G: %s (%d nodes)\n", SCFile, 
        nNodes1);
  ShockGraph1_->load_sg (SCFile);

  vcl_fscanf (fp, "ms_g: %s (%d nodes, %d links, %d corners)\n", SCFile, 
        &nNodes2, &nLinks2, &nCorners2);
  fileName2_ = vcl_string (SCFile);
  vul_printf (vcl_cout, "ms_g: %s (%d nodes)\n", SCFile, 
        nNodes2);
  ShockGraph2_->load_sg (SCFile);

  M_row_ = nNodes1+1;
  M_col_ = nNodes2+1;
  labelGg_ = new int[M_row_-1];
  shockIdGg_ = new int*[M_row_-1];
  for (int i=0; i<M_row_-1; i++)
    shockIdGg_[i] = new int[2];
  
  vcl_fscanf (fp, "dbsk3dr_ms_gradasn_graph Assignment GSid(Gnid) <-> gSid(gnid):\n");
  vul_printf (vcl_cout, "dbsk3dr_ms_gradasn_graph Assignment GSid(Gnid) <-> gSid(gnid):\n");
  for (int a=0; a<M_row_-1; a++) {
    int GSid, Gid, gSid, gid;
    //char s[1024];
    //vcl_fscanf (fp, "%s", s);
    vcl_fscanf (fp, "%d(%d) <-> %d(%d)\n", &GSid, &Gid, &gSid, &gid);
    assert (a == Gid);
    labelGg_[a] = gid;
    shockIdGg_[a][0] = GSid;
    shockIdGg_[a][1] = gSid;

    vul_printf (vcl_cout, "%d(%d) <-> %d(%d)\n", GSid, Gid, gSid, gid);
  }

  fclose (fp);
  return true;
}*/


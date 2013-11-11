//: dbsk3d_ms_algos.cxx
//  MingChing Chang

#include <vcl_iostream.h>
#include <vnl/vnl_random.h>
#include <vul/vul_printf.h>

#include <dbgl/algo/dbgl_curve_smoothing.h>
#include <dbmsh3d/algo/dbmsh3d_sheet_algo.h>

#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/dbsk3d_sg_sa.h>
#include <dbsk3d/algo/dbsk3d_fs_algo.h>
#include <dbsk3d/algo/dbsk3d_ms_algos.h>

void count_ms_vertices (dbmsh3d_graph* SG, int& nA1A3, int& nA14, int& nDegeA1A3, int& nDegeA14, int& nLN)
{
  nA1A3 = 0;
  nA14 = 0;
  nDegeA1A3 = 0;
  nDegeA14 = 0;
  nLN = 0;
  vcl_map<int, dbmsh3d_vertex*>::iterator it = SG->vertexmap().begin();
  for (; it != SG->vertexmap().end(); it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*it).second;
    switch (MN->n_type()) {
    case N_TYPE_RIB_END:
      nA1A3++;
    break;
    case N_TYPE_AXIAL_END:
      nA14++;
    break;
    case N_TYPE_DEGE_RIB_END:
      nDegeA1A3++;
    break;
    case N_TYPE_DEGE_AXIAL_END:
      nDegeA14++;
    break;
    case N_TYPE_LOOP_END:
      nLN++;
    break;
    default:
      assert (0);
    break;
    }
  }
  assert (nA1A3 + nA14 + nDegeA1A3 + nDegeA14 + nLN == int(SG->vertexmap().size()));
}

void count_ms_curves (dbmsh3d_graph* SG, int& nA3, int& nA13, int& nDege, int& nLC, int& nVirtual)
{
  nA3 = 0;
  nA13 = 0;
  nDege = 0;
  nLC = 0;
  nVirtual = 0;
  vcl_map<int, dbmsh3d_edge*>::iterator it = SG->edgemap().begin();
  for (; it != SG->edgemap().end(); it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*it).second;
    switch (MC->c_type()) {
    case C_TYPE_RIB:
      nA3++;
    break;
    case C_TYPE_AXIAL:
      nA13++;
    break;
    case C_TYPE_DEGE_AXIAL:
      nDege++;
    break;    
    case C_TYPE_VIRTUAL:
      nVirtual++;
    break;
    default:
      assert (0);
    break;
    }
  }
  assert (static_cast<unsigned>(nA3 + nA13 + nDege + nLC + nVirtual) == SG->edgemap().size());
}


//#####################################################################
//  Check Integrity 
//#####################################################################

bool check_integrity (vcl_set<dbsk3d_ms_sheet*> MS_set,
                      vcl_set<dbsk3d_ms_curve*> MC_set,
                      vcl_set<dbsk3d_ms_node*> MN_set)
{
  vcl_set<dbsk3d_ms_sheet*>::iterator sit = MS_set.begin();
  for (; sit != MS_set.end(); sit++) {
    dbsk3d_ms_sheet* MS = (*sit);
    if (MS->check_integrity() == false) {
      assert (0);
      return false;
    }
  }
  vcl_set<dbsk3d_ms_curve*>::iterator cit = MC_set.begin();
  for (; cit != MC_set.end(); cit++) {
    dbsk3d_ms_curve* MC = (*cit);
    if (MC->check_integrity() == false) {
      assert (0);
      return false;
    }
  }
  vcl_set<dbsk3d_ms_node*>::iterator vit = MN_set.begin();
  for (; vit != MN_set.end(); vit++) {
    dbsk3d_ms_node* MN = (*vit);
    if (MN->check_integrity() == false) {
      assert (0);
      return false;
    }
  }

  return true;
}


//#####################################################################

//: Get fs_faces that are with 3 or more coarse-scale shock curves.
int get_SS_P_n_SCxforms (dbsk3d_ms_hypg* ms_hypg, const int n_xform_th,
                         vcl_vector<vcl_pair<dbsk3d_ms_sheet*, dbsk3d_fs_face*> >& SS_P_n_SCxforms)
{
  vcl_map<int, dbmsh3d_sheet*>::iterator SS_it = ms_hypg->sheetmap().begin();
  for (; SS_it != ms_hypg->sheetmap().end(); SS_it++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*SS_it).second;

    vcl_set<dbmsh3d_edge*> incident_Es;
    MS->get_incident_Es (incident_Es);    
    
    vcl_map<int, dbmsh3d_face*>::iterator it = MS->facemap().begin();
    for (; it != MS->facemap().end(); it++) {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it).second;

      //Check if FF is incident with more than 2 shock curves.      
      vcl_set<dbsk3d_ms_curve*> P_incident_SCs;
      get_FF_incident_MCs (FF, incident_Es, P_incident_SCs);
      if (int(P_incident_SCs.size()) > n_xform_th)
        SS_P_n_SCxforms.push_back (vcl_pair<dbsk3d_ms_sheet*, dbsk3d_fs_face*>(MS, FF));
    }
  }

  vul_printf (vcl_cout, "get_SS_P_n_SCxforms(): %d fs_faces detected.\n", 
              SS_P_n_SCxforms.size());
  return SS_P_n_SCxforms.size();
}

bool get_FF_incident_MCs (const dbsk3d_fs_face* FF, 
                          vcl_set<dbmsh3d_edge*>& incident_Es, 
                          vcl_set<dbsk3d_ms_curve*>& P_incident_SCs)
{
  dbmsh3d_halfedge* HE = FF->halfedge();
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
    dbsk3d_ms_curve* MC = find_MC_containing_E (incident_Es, FE);
    if (MC)
      P_incident_SCs.insert (MC);
    HE = HE->next();
  }
  while (HE != FF->halfedge());
  return P_incident_SCs.empty() == false;
}

dbsk3d_ms_curve* find_MC_containing_E (vcl_set<dbmsh3d_edge*>& incident_Es, 
                                       dbsk3d_fs_edge* inputE)
{
  //Go through each MC in incident_Es and find the inputL.
  vcl_set<dbmsh3d_edge*>::iterator it = incident_Es.begin();
  for (; it != incident_Es.end(); it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*it);
    if (MC->contain_E (inputE))
      return MC;
  }
  return NULL;
}

//#########################################################################

//: Smooth MS hypergraph rib curves by smoothing each bordering MS FF elements.
//
void ms_hypg_rib_smooth_within_face (dbsk3d_ms_hypg* ms_hypg, const float psi, const int iter, 
                                     const float tiny_rib_ratio)
{
  vul_printf (vcl_cout, "ms_hypg_rib_smooth_within_face(): DCS step_size %f, iter %d, tiny_rib_ratio %f.\n", 
              psi, iter, tiny_rib_ratio);

  //Loop through all MS with bordering A3ribs.
  vcl_map<int, dbmsh3d_sheet*>::iterator MSit = ms_hypg->sheetmap().begin();
  for (; MSit != ms_hypg->sheetmap().end(); MSit++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*MSit).second;
    if (MS->has_incident_A3rib() == false)
      continue;

    //Loop through each FF with bordering A3rib elements.
    vcl_map<int, dbmsh3d_face*>::iterator it = MS->facemap().begin();
    for (; it != MS->facemap().end(); it++) {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it).second;
      if (FF->find_1st_bnd_HE() == false)
        continue;
      
      //Detect and remove tiny bnd edges.
      remove_tiny_rib_edges (ms_hypg, MS, FF, tiny_rib_ratio);

      //Now smooth FF's rib curve.
      FF_smooth_rib_curve (FF, psi, iter);
    }
  }
}

bool remove_tiny_rib_edges (dbsk3d_ms_hypg* ms_hypg, dbsk3d_ms_sheet* MS, 
                            dbsk3d_fs_face* FF, const double& rib_len_ratio)
{
  assert (FF->halfedge());
  if (FF->halfedge()->next() == NULL)
    return false;
  
  //epsilon to remove tiny rib edges.
  dbsk3d_fs_mesh* fs_mesh = ms_hypg->fs_mesh();
  double epsilon = fs_mesh->median_A122_dist() * rib_len_ratio;

  bool b_break = false;

  //Go through each rib_edge and remove it is too small.
  dbmsh3d_halfedge* HE = FF->halfedge();
  do {
    dbmsh3d_halfedge* nextHE = HE->next();
    if (nextHE->pair() == NULL) {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) nextHE->edge();
      if (FE->length() < epsilon) {
        
        //Delete FV and keep FV2.
        dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) Es_sharing_V (HE->edge(), nextHE->edge());
        dbsk3d_fs_vertex* FV2 = (dbsk3d_fs_vertex*) FE->other_V (FV);
        if (FV->n_incident_Es() == 2) {
        }
        else if (FV2->n_incident_Es() == 2) { //swap FV and FV2
          dbsk3d_fs_vertex* tmp = FV;
          FV = FV2;
          FV2 = tmp;
        }
        else {
          ///vul_printf (vcl_cout, "\t  failed!!\n");
          return false; //TO-DO!!
        }

        if (FF->n_bnd_Es() <4)
          return false;

        if (nextHE == FF->halfedge())
          b_break = true;

        //Remove nextHE and FE. 
        vul_printf (vcl_cout, "remove tiny rib FE %d from FF %d.\n",
                    FE->id(), FF->id());

        //Find the rib MC containing FE.
        dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) MS->get_incident_C_via_E (FE);

        //Remove nextHE->edge and FV from coarse-scale curve MC.
        MC->del_from_E_vec (FE);
        MC->del_from_V_vec (FV);

        //pass FE's generator to FV2.
        for (dbmsh3d_ptr_node* cur = FE->asgn_G_list(); cur != NULL; cur = cur->next()) {
          dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
          if (FV2->is_G_asgn (G) == false)
            FV2->add_asgn_G (G);
        }
        FE->clear_asgn_G_list();

        //pass FV's generator to FV2.
        for (dbmsh3d_ptr_node* cur = FV->asgn_G_list(); cur != NULL; cur = cur->next()) {
          dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
          if (FV2->is_G_asgn (G) == false)
            FV2->add_asgn_G (G);
        }
        FV->clear_asgn_G_list();

        //disconnect nextHE from FE.
        FE->_disconnect_HE (nextHE); 
        //delete FE
        fs_mesh->remove_edge (FE);

        //connect HE->edge to V2.
        if (HE->edge()->is_V_incident (FV)) {
          HE->edge()->_replace_vertex (FV, FV2);
          FV2->add_incident_E (HE->edge());
          FV->del_incident_E (HE->edge());
        }
        else {
          nextHE->next()->edge()->_replace_vertex (FV, FV2);
          FV2->add_incident_E (nextHE->next()->edge());
          FV->del_incident_E (nextHE->next()->edge());
        }

        //delete FV
        fs_mesh->remove_vertex (FV);

        //disconnect nextHE from FF.
        HE->set_next (nextHE->next()); 

        delete nextHE;        
      }      

      if (b_break) {
        FF->set_halfedge (HE);
        break;
      }
    }
    HE = HE->next();
  }
  while (HE != FF->halfedge());

  return true;
}

//: Smooth MS hypergraph rib curves using DCS.
//
void ms_hypg_rib_smooth_DCS (dbsk3d_ms_hypg* ms_hypg, const float psi, const int iter, 
                             const float tiny_rib_ratio)
{
  vul_printf (vcl_cout, "ms_hypg_rib_smooth_DCS(): DCS step_size %f, iter %d, tiny_rib_ratio %f.\n", 
              psi, iter, tiny_rib_ratio);

  //Loop through all A3ribs.
  vcl_map<int, dbmsh3d_edge*>::iterator eit = ms_hypg->edgemap().begin();
  for (; eit != ms_hypg->edgemap().end(); eit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*eit).second;
    if (MC->n_incident_Fs() != 1)
      continue; //Skip non-rib MC.

    //DCS smooth this curve.
    MC_DCS_smooth (MC, psi, iter);
  }
}

bool MC_DCS_smooth (dbsk3d_ms_curve* MC, const float psi, const int nsteps)
{
  if (MC->E_vec().size() < 2)
    return false;

  //Identify all sample points to apply DCS.
  vcl_vector<vgl_point_3d<double> > curve;
  get_digi_curve_E_chain (MC->E_vec(), curve);

  //DCS smooth the curve.
  dbgl_curve_shorten_3d (curve, psi, nsteps);

  //Update the dbmsh3d_vertices in bndE_chains[i].
  update_digi_curve_E_chain (MC->E_vec(), curve);
  return true;
}

//: Gaussian smooth MS hypergraph rib curves.
//
void ms_hypg_rib_smooth_gaussian (dbsk3d_ms_hypg* ms_hypg, const int iter, const float sigma)
{
  vul_printf (vcl_cout, "ms_hypg_rib_smooth_gaussian(): Gaussian sigma %f, iter %d.\n", 
              sigma, iter);

  //Loop through all A3ribs.
  vcl_map<int, dbmsh3d_edge*>::iterator eit = ms_hypg->edgemap().begin();
  for (; eit != ms_hypg->edgemap().end(); eit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*eit).second;
    if (MC->n_incident_Fs() != 1)
      continue; //Skip non-rib MC.

    //DCS smooth this curve.
    MC_gaussian_smooth (MC, iter, sigma);
  }
}

#define CURVE_GAUSSIAN_SEG_TH   30
#define CURVE_GAUSSIAN_SEG      15

bool MC_gaussian_smooth (dbsk3d_ms_curve* MC, const int iter, const float sigma)
{
  if (MC->E_vec().size() < 2)
    return false;

  //Identify all sample points to apply DCS.
  vcl_vector<vgl_point_3d<double> > curve;
  get_digi_curve_E_chain (MC->E_vec(), curve);

  //Gaussian smooth the curve.
  //Break long curve into short ones to prevent shrinkage in Gaussian smoothing.
  if (curve.size() < CURVE_GAUSSIAN_SEG_TH) {
    dbgl_gaussian_smooth_curve_3d_fixedendpt (curve, sigma, iter);
  }
  else {
    vnl_random rand;
    for (int c=0; c<iter; c++) {
      int step = CURVE_GAUSSIAN_SEG + rand.lrand32 (-5, 5);
      for (int i=0; i<int(curve.size()); i+=step) {
        //smooth curve[i] to curve[i+step-1] (or last)        
        vcl_vector<vgl_point_3d<double> > sc;
        int last = vcl_min (i+step, int(curve.size()));
        sc.insert (sc.begin(), curve.begin()+i, curve.begin()+last);

        dbgl_gaussian_smooth_curve_3d_fixedendpt (sc, sigma, 1);

        //write sc back to curve.
        for (int j=0; j<sc.size(); j++)
          curve[i+j] = sc[j];
      }
    }
  }

  //Update the dbmsh3d_vertices in bndE_chains[i].
  update_digi_curve_E_chain (MC->E_vec(), curve);
  return true;
}

//##################################################################
//###### Sub-sampled with noise ######
//##################################################################

dbsk3d_ms_hypg* sub_sampled_w_noise (dbsk3d_ms_hypg* MSH, const float noise)
{
  //deep-copy MSH
  dbsk3d_ms_hypg* MSH2 = (dbsk3d_ms_hypg*) MSH->clone ();

  //apply transforms on MSH2.
  
  //add noise to fs_vertices of MSH2.  
  vnl_random mzr;
  float d = MSH2->fs_mesh()->compute_median_A122_dist ();

  vcl_map<int, dbmsh3d_vertex*>::iterator vit = MSH2->fs_mesh()->vertexmap().begin();
  for (; vit != MSH2->fs_mesh()->vertexmap().end(); vit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*vit).second;
    vgl_point_3d<double> P = FV->pt();
    FV->set_pt (P.x() + mzr.drand32(0.0, 1.0) * d,
                P.y() + mzr.drand32(0.0, 1.0) * d,
                P.z() + mzr.drand32(0.0, 1.0) * d);
  }

  return MSH2;
}


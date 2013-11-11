//: This is dbsk3d_ms_algos.h
//  MingChing Chang 070718

#ifndef dbsk3d_ms_algos_h_
#define dbsk3d_ms_algos_h_

#include <vcl_vector.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>

void count_ms_vertices (dbmsh3d_graph* SG, int& nA1A3, int& nA14, int& nDegeA1A3, int& nDegeA14, int& nLV);

void count_ms_curves (dbmsh3d_graph* SG, int& nA3, int& nA13, int& nDege, int& nLC, int& nVirtual);


//###### Check Integrity ######

bool check_integrity (vcl_set<dbsk3d_ms_sheet*> SS_set,
                      vcl_set<dbsk3d_ms_curve*> SC_set,
                      vcl_set<dbsk3d_ms_node*> SV_set);

int get_SS_P_n_SCxforms (dbsk3d_ms_hypg* ms_hypg, const int n_xform_th,
                         vcl_vector<vcl_pair<dbsk3d_ms_sheet*, dbsk3d_fs_face*> >& SS_P_n_SCxforms);

bool get_FF_incident_MCs (const dbsk3d_fs_face* FF, 
                          vcl_set<dbmsh3d_edge*>& incident_Es, 
                          vcl_set<dbsk3d_ms_curve*>& P_incident_SCs);

dbsk3d_ms_curve* find_MC_containing_E (vcl_set<dbmsh3d_edge*>& incident_Es, 
                                       dbsk3d_fs_edge* inputE);

//###### Smooth Rib Curves ######

void ms_hypg_rib_smooth_within_face (dbsk3d_ms_hypg* ms_hypg, const float psi, const int iter, 
                                     const float tiny_rib_ratio);

bool remove_tiny_rib_edges (dbsk3d_ms_hypg* ms_hypg, dbsk3d_ms_sheet* MS, 
                            dbsk3d_fs_face* FF, const double& tiny_rib_len_r);

void ms_hypg_rib_smooth_DCS (dbsk3d_ms_hypg* ms_hypg, const float psi, const int iter, 
                             const float tiny_rib_ratio);

bool MC_DCS_smooth (dbsk3d_ms_curve* MC, const float psi, const int nsteps);

void ms_hypg_rib_smooth_gaussian (dbsk3d_ms_hypg* ms_hypg, const int iter, const float sigma);

bool MC_gaussian_smooth (dbsk3d_ms_curve* MC, const int iter, const float sigma);

//###### Sub-sampled with noise ######

dbsk3d_ms_hypg* sub_sampled_w_noise (dbsk3d_ms_hypg* ms_hypg, const float noise);

#endif

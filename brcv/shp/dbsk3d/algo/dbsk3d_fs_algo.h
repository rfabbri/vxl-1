//: This is dbsk3d_fs_algo.h
//  MingChing Chang 061026

#ifndef dbsk3d_fs_algo_h_
#define dbsk3d_fs_algo_h_

#include <vcl_vector.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/dbsk3d_fs_sheet_set.h>

void add_bndsphere_genes (dbmsh3d_mesh* bndset, const float radius_ratio, const int nsample_ratio);

//###### Reconstruct from QHull Voronoi File (*.VOR) ######
// Code in dbsk3d_fs_recovery_qhull.cxx
bool load_from_vor_file (dbsk3d_fs_mesh* fs_mesh, const char* pcVORFile);

bool load_from_vor_file_pass2 (dbsk3d_fs_mesh* fs_mesh, 
                               const char* pcVORFile, const int nOrigGene,
                               vcl_vector<vcl_vector<dbsk3d_fs_face*> >& G_S_asso);

void rm_bndsphere_shock_recovery (dbsk3d_fs_mesh* fs_mesh, const int n_orig_gene,
                                  vcl_vector<vcl_vector<dbsk3d_fs_face*> >& G_S_asso);

void remove_bndsphere_genes (dbmsh3d_mesh* bndset, const int n_orig_gene);

//###### Shock Flow Type Detection ######

void get_FF_min_max_time (dbsk3d_fs_face* FF, float& min_time, float& max_time);

void compute_fs_patches_flow_type (dbsk3d_fs_mesh* fs_mesh);

bool brute_force_verify_A122_P (dbsk3d_fs_face* FF, dbmsh3d_mesh* M);

void compute_fs_links_flow_type (dbsk3d_fs_mesh* fs_mesh);

void compute_fs_nodes_flow_type (dbsk3d_fs_mesh* fs_mesh);

//###### Boundary mesh labeling ######

//Label the A12-2 shock sheets with mesh edge as visited.
void label_edge_A12_2_sheets (dbsk3d_fs_mesh* fs_mesh);
//Label the A13-2 shock links with mesh face as visited.
void label_face_A13_links (dbsk3d_fs_mesh* fs_mesh);

//###### Output shock elements to files ######

void output_A12_2_file (dbsk3d_fs_mesh* fs_mesh, const int option);
void output_A13_file (dbsk3d_fs_mesh* fs_mesh);

//###### Surface Pruning Functions ######
void shock_pruning_bnd_FF_compactness (dbsk3d_fs_sheet_set* fs_ss, const int iter, const float c_th);
void shock_pruning_box (dbsk3d_fs_sheet_set* fs_ss, const float box_ratio);

//###### The Temporarily Rmin Pruning ######

//Shock trimming on minimum radius. 
void rmin_trim_xforms (dbsk3d_fs_sheet_set* fs_ss, const float rmin_th);

void perform_rmin_trim_xforms (vcl_multimap<float, dbsk3d_fs_face*>& FF_mmap,
                               const float rmin_th);

//Shock trimming on maximum radius. 
void rmax_trim_xforms (dbsk3d_fs_sheet_set* fs_ss, const float rmax_th);


void perform_rmax_trim_xforms (vcl_multimap<float, dbsk3d_fs_face*>& FF_mmap,
                               const float rmax_th);

void prune_shocks_of_bnd_holes (dbmsh3d_mesh* bnd_mesh, dbsk3d_fs_mesh* fs_mesh);

//###### Recover Gene-Shock Assignment ######

//: Re-assign 'lost' generators via finding the closest valid fs_vertex.
bool reasgn_lost_Gs_closest_FV (dbsk3d_fs_mesh* fs_mesh, vcl_vector<dbmsh3d_vertex*>& unasgn_genes);

//: Re-assign 'lost' generators via searching through fs_faces.
bool reasgn_lost_Gs_via_FF (dbsk3d_fs_mesh* fs_mesh, vcl_vector<dbmsh3d_vertex*>& unasgn_genes);
bool asgn_lost_gene (dbsk3d_fs_mesh* fs_mesh, const dbmsh3d_vertex* G,
                     vcl_vector<dbsk3d_fs_face*>& init_fs_faces);
void prop_BFS_on_FF (dbsk3d_fs_mesh* fs_mesh, dbsk3d_fs_face* frontP, 
                     vcl_queue<dbsk3d_fs_face*>& BFS_patch_queue, 
                     vcl_set<dbsk3d_fs_edge*>& cand_A3_links);

//###### Smooth Rib Curves ######

dbmsh3d_edge* trim_FF (dbsk3d_fs_face* FF, dbmsh3d_vertex* keepV, const dbmsh3d_edge* keepE, 
                       const dbmsh3d_vertex* trimV, const bool pass_gene,
                       vcl_vector<dbmsh3d_edge*>& E_to_del, vcl_vector<dbmsh3d_vertex*>& V_to_del);

bool FF_smooth_rib_curve (dbsk3d_fs_face* FF, const float psi, const int nsteps);

#endif

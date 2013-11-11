//: dbmsh3dr/dbmsh3dr_match.h
//  Mesh Recognition, Matching and Registration Library
//  Feb 27, 2007  MingChing Chang

#ifndef dbmsh3dr_match_h_
#define dbmsh3dr_match_h_

#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

#include <rgrl/rgrl_feature_sptr.h>
#include <rgrl/rgrl_trans_rigid.h>
///class rgrl_feature_sptr;
///class rgrl_trans_rigid;

#include <dbmsh3d/dbmsh3d_mesh.h>

//#####################################################################

bool dbmsh3dr_pp_icp_regstr (const vcl_vector<vgl_point_3d<double> >& fixPS, 
                             vcl_vector<vgl_point_3d<double> >& movPS, 
                             const int nMaxIter, const float conv_th, const double dist_th,
                             vgl_h_matrix_3d<double>& H);

bool dbmsh3dr_pf_icp_regstr (dbmsh3d_mesh* fixPS, dbmsh3d_mesh* movPS, 
                             const int max_iter, const float conv_th, const double dist_th,
                             vgl_h_matrix_3d<double>& H);

vgl_h_matrix_3d<double> set_h_matrix (const vnl_matrix_fixed<double,3,3>& R, 
                                      const vnl_vector_fixed<double,3>& fixC,
                                      const vnl_vector_fixed<double,3>& movC);

double estimate_ICP_conv (const vnl_matrix_fixed<double,3,3>& R);

///typedef vnl_vector_fixed<double,3>     vector_3d;
///typedef vcl_vector<rgrl_feature_sptr>  feature_vector;

bool rgrl_rigid_icp_register (const vcl_vector<rgrl_feature_sptr>& fixedFV, 
                              const vcl_vector<rgrl_feature_sptr>& movingFV,    
                              const vnl_matrix<double>& init_R, 
                              const vnl_vector_fixed<double,3>&  init_t,
                              rgrl_trans_rigid& r_xform, double& error);

//#####################################################################

void compute_pp_min_dist (const vcl_vector<vgl_point_3d<double> >& pts1, 
                          const vcl_vector<vgl_point_3d<double> >& pts2, 
                          vcl_vector<double>& min_dists,
                          vcl_vector<int>& min_ids);

void compute_pp_min_dist (const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts1, 
                          const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts2, 
                          vcl_vector<double>& min_dists,
                          vcl_vector<int>& min_ids);

void compute_pp_min_dist (dbmsh3d_pt_set* PS1, dbmsh3d_pt_set* PS2,
                          vcl_vector<double>& min_dists,
                          vcl_vector<int>& min_ids);

void compute_pf_min_dist (dbmsh3d_mesh* M, dbmsh3d_mesh* M2, const int top_n, 
                          vcl_vector<double>& min_dists,
                          vcl_vector<vgl_point_3d<double> >& closest_pts);

bool get_error_estim (const vcl_vector<double>& min_dists, const double& dist_th, 
                      double& min, double& mean, double& median, double& RMS, double& max);

void normalize_min_dist (const double dist_th, vcl_vector<double>& min_dists); 

void compute_imprv_of_error (const vcl_vector<double>& min_dists_1,
                             const vcl_vector<double>& min_dists_2,
                             const double& dist_th, 
                             double& min, double& mean, double& median, double& RMS, double& max);

//: return false and the problematic quert pt if test fail.
bool test_perturb_compute_dist (dbmsh3d_mesh* M, const int iteration,
                                const int top_n, const float perturb,
                                vgl_point_3d<double>& pt);

void find_P_closest_face (const vgl_point_3d<double>& queryP, 
                          dbmsh3d_vertex* V, double& dist_f, int& fid);

//#####################################################################

#endif

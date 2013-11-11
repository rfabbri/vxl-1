#include "dbkpr_projective_reconstruct_process.h"

#include <bpro1/bpro1_parameters.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <dbdet/dbdet_keypoint_corr3d.h>
#include <dbdet/dbdet_keypoint.h>
#include <dbkpr/pro/dbkpr_corr3d_storage_sptr.h>
#include <dbkpr/pro/dbkpr_corr3d_storage.h>
#include <dbkpr/dbkpr_view_span_tree.h>
#include <vpgl/algo/vpgl_bundle_adjust.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_double_4.h>
#include <vgl/vgl_distance.h>

#include <mw/mw_util.h>

/* OVERALL STEPS
 *
 * + Frame selection: 
 *    + in automatic reconstruction from all frames (it will be a mother process of this one),
 *    select it based on the greatest number of matches and furtherst apart frames as possible.
 *
 * =======================================
 * + From fund. matrix F, get pair of cameras P1 and P2 
 *    + P1 and P2 either canonical form or in some form that embeds some rough knowledge of the
 *    intrinsics. Canonical can be chosen for now (Pollefeys).
 * + Access all correspondences between v1 and v2
 * + Set up a reconstruction framework
 *    - Closed-form solution as in Pollefeys
 *    OR
 *    + Least-squares + Levenberg-marquadt
 *      + I chose least-squares (Ma's book)
 *
 * + visualize reconstruction
 *
 * + Use classical calibration to provide the intrinsics and manually check that the rectified
 *   reconstruction makes sense
 *
 * =======================================
 *
 * ADD A VIEW TO EXISTING RECONSTRUCTION:
 *
 * - pick v3 connected to v1 or v2 in the spanning tree of views
 *    - Access all correspondences in common with v3 that have some reconstruction
 *      - Epipolar constraint could be better enfoced to get only the best matches
 *    - Make sure the number of reliable correspondences in common is large enough to code resectioning (I
 *    think its 6 points).
 *    - problem: there might be many connected components of views with no match between them.
 *
 *    - Use vpgl's resectioning code to find the pose of the 2nd camera
 *
 *      - Alternatively, use fmatrix::get_left_camera method that takes in 3D-2D corresps
 *        ./vxl/contrib/gel/mrc/vpgl/algo/tests/test_fm_compute.cxx:    fm_aff1.extract_left_camera( vnl_vector_fixed<double,3>(1,2,3), 1 );
 *        ./vxl/contrib/gel/mrc/vpgl/tests/test_fundamental_matrix.cxx:    F3.extract_left_camera( vnl_vector_fixed<double,3>(0,0,0), 1 );
 *      - First, use least squares solution. Find a way to visualize it for the calibrated case.
 *      - Second, use RANSAC. Visualize it for the calibrated case to see how it performed
 *          
 *    - Find additional correspondences?
 *
 *    - Refine reconstruction? Refine cameras?
 *        - Using least squares for a point and many projections  (brct_algos_bundle_reconstruct)
 *
 * - apply a known intrinsic calibration to recover the points
 * 
 * - idea: by using a synthetic dataset we can check the accuracy of the matches.
 *
 * =================================================
 *
 * Further todo
 * - Predict degenerate motions
 *   - Trifocal tensor
 *
 */


//: Constructor
dbkpr_projective_reconstruct_process::dbkpr_projective_reconstruct_process()
{
  if( !parameters()->add( "VRML File",          "-vrmlfile",  bpro1_filepath("","wrl")) ||
      !parameters()->add( "Verbose",            "-verbose",   (bool)true)) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbkpr_projective_reconstruct_process::~dbkpr_projective_reconstruct_process()
{
}


//: Clone the process
bpro1_process*
dbkpr_projective_reconstruct_process::clone() const
{
  return new dbkpr_projective_reconstruct_process(*this);
}


//: Return the name of this process
vcl_string
dbkpr_projective_reconstruct_process::name()
{
  return "Projective-Reconstruct Keypoints";
}


//: Return the number of input frame for this process
int
dbkpr_projective_reconstruct_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbkpr_projective_reconstruct_process::output_frames()
{
  return 0;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbkpr_projective_reconstruct_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "keypoints_corr3d" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbkpr_projective_reconstruct_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "keypoints_corr3d" );
  return to_return;
}


//: Execute the process
bool
dbkpr_projective_reconstruct_process::execute()
{
  return true;
}


//: Stitches multiple view into a consistent set of cameras and 3D points. This is useful as input
// to auto-calibration and to initialize bundle adjustment.
//
bool
dbkpr_projective_reconstruct_process::finish()
{
  /*
  // -------------------------------------------------------------------------------------
  // cast the storage classes
  for(unsigned int i=0; i<input_data_.size(); ++i) {
    assert(input_data_[i].size() == 1);
    if (i == 0) {
      dbkpr_corr3d_storage_sptr frame_corr3d;
      frame_corr3d.vertical_cast(input_data_[i][0]);
      if (!frame_corr3d) {
        vcl_cerr << "Error: unable to get corr3d storage\n";
        return false;
      }
      if (!frame_corr3d->has_tree()) {
        vcl_cerr << "Error: corr3d storage has no spanning tree\n";
        return false;
      }
      tree_ptr_ = frame_corr3d->tree();
    }
  }
  //  - optional argument to dbkpr_span_match_process to output a span tree storage

  const dbkpr_view_span_tree &tree = *tree_ptr_;
  //dbkpr_view_span_tree tree(all_keypoints, max_bins, std, num_samples);

  
  bpro1_filepath vrml_file;
  bool verbose = true;
  parameters()->get_value( "-vrmlfile" , vrml_file );
  parameters()->get_value( "-verbose" , verbose );

  // sometimes links might not be formed between all views
  nviews_ = tree.links().size();
  assert (nviews_ == input_data_.size());

  // -------------------------------------------------------------------------------------

  unsigned v1, v2; unsigned nmatches;

  select_best_views(tree,v1,v2, nmatches);

  vpgl_proj_camera<double> Pr1, Pr2;
  get_initial_reconstruction(v1,v2,corr3d);
  // Returns Pr1,Pr2, corr3d representing correspondences and instantiated 3D points

  vcl_vector<unsigned> view_set;
  views.push_back(v1);
  views.push_back(v2);

  vcl_vector<bool> in_view_set(nviews_,false);
  in_view_set[v1] = true;
  in_view_set[v2] = true;

  vcl_vector<dvpgl_proj_camera<double> > cam_set;
  cam_set.push_back(Pr1);
  cam_set.push_back(Pr2);


  // -------------------------------------------------------------------------------------
  // Attempt to add a view which is connected to some image in the current set of views in the
  // spanning tree. I am only dealing with one connected component for now
  for (count=2; count<=nviews_; ++count) { //:< count = number of reconstructed views
    
    unsigned v3 = pick_new_view(view_set);
    add_new_view_to_reconstruction(v3,view_set,cam_set,corr_set);
  }

  // projective-bundle_adjust all reconstructions _AND_ cameras
  projective_bundle_adjust(cam_set,corr_set);

  // Upgrade to metric
  //
  if (supplied_calibration) {
    rectify_reconstruction(K,corr3d,cam_set);
    metric_bundle_adjustment(K,corr3d,cam_set);
  } else {
    vgl_h_matrix_3d<double> H;

    auto_calibration::dual_quadric_solver autocal;
    autocal.solve(corr3d,cam_set,H,K);

    rectify_reconstruction(H,corr3d,cam_set);
    // K is not being optimized by Matt's code. In the future, should his modify bundle adjustment
    // for this.
    metric_bundle_adjustment(K,corr3d,cam_set);
  }


  // OUTPUT --------------------------------------------------------------------------------------- 

  // for now, output all corr3d intact, but we might use additional criteria to prune the matches
  vcl_vector<dbdet_keypoint_corr3d_sptr> corr3d = tree.global_corr3d_points();
  
  // Also output camera storages separately so we can visualize epipolar geometry
  vcl_vector<vpgl_proj_camera<double> *> cameras(input_data_.size());
  cameras[v1] = new vpgl_proj_camera<double> (Pr1);
  cameras[v2] = new vpgl_proj_camera<double> (Pr2);


  // pack up the results into storage classes
  dbkpr_corr3d_storage_sptr in_storage;
  for(unsigned i=0; i<input_data_.size(); ++i) {
    in_storage.vertical_cast(input_data_[i][0]);
    // cameras[v1] = Pr1; cameras[v2] = Pr2; cameras[i] = Pr1 for all other i
    if (i != v2 && i != v1) {
      cameras[i] = new vpgl_proj_camera<double> (*(cameras[v1]));
    }

    dbkpr_corr3d_storage_sptr out_storage = 
      dbkpr_corr3d_storage_new(corr3d, in_storage->ni(),in_storage->nj());

    out_storage->set_camera(cameras[i]);
    out_storage->set_tree(tree_ptr_);
    output_data_[i].push_back(out_storage);
  }

  if(vrml_file.path != "")  {
    vcl_cout << "warning: not writing VRML right now...\n";
  }
//    vpgl_bundle_adjust::write_vrml(vrml_file.path, cameras, pts3d);

  */

    vcl_cerr << "WARNING: THIS PROCESS IS STILL BEING IMPLEMENTED\n";
  return true;
}


vgl_homg_point_3d<double> dbkpr_projective_reconstruct_process::
triangulate_3d_point(
    const vgl_homg_point_2d<double>& x1, const vnl_double_3x4& P1,
    const vgl_homg_point_2d<double>& x2, const vnl_double_3x4& P2)
{
  vnl_double_4x4 A;

  for (int i=0; i<4; i++){
    A[0][i] = x1.x()*P1[2][i] - P1[0][i];
    A[1][i] = x1.y()*P1[2][i] - P1[1][i];
    A[2][i] = x2.x()*P2[2][i] - P2[0][i];
    A[3][i] = x2.y()*P2[2][i] - P2[1][i];
  }

  vnl_svd<double> svd_solver(A);
  vnl_double_4 p = svd_solver.nullvector();
  return vgl_homg_point_3d<double>(p[0],p[1],p[2],p[3]);
}

#if 0

void dbkpr_projective_reconstruct_process::
select_best_views(const dbkpr_view_span_tree &tree, unsigned &v1, unsigned &v2, unsigned &nmatches) const
{
  v1 = v2 = nmatches = 0;
  for (unsigned i=0; i < tree.links().size(); ++i) {
    assert(tree.links()[i].size());
    assert(tree.links()[i].back().from == (int)i);
    if (tree.links()[i].size() > 1)
      assert(tree.links()[i][tree.links()[i].size()-2].matches.size() <= tree.links()[i].back().matches.size());

    if (tree.links()[i].back().matches.size() > nmatches) {
      v1 = i;
      nmatches = tree.links()[i].back().matches.size();
    }
  }

  v2 = tree.links()[v1].back().to;

  vcl_cout << "Picked key frames with index " << v1 << " and " << v2  << " nmatches = " << nmatches << vcl_endl;
}



// \todo consider only the subset of correspondences spanning more than 2 views.
// For now, I'll work with all correspondences btw v1 and v2 
void dbkpr_projective_reconstruct_process::
get_initial_reconstruction(
    unsigned v1, unsigned v2, 
    vpgl_proj_camera<double> &Pr1, vpgl_proj_camera<double> &Pr2,
    vcl_vector<dbdet_keypoint_corr3d_sptr> &corr3d)
{
  const dbkpr_view_span_tree &tree = *tree_ptr_;
  const dbkpr_view_span_link &link = tree.links()[v1].back();
  vpgl_fundamental_matrix<double> fm(link.F.get_matrix());


  vnl_vector_fixed<double,3> zero3(0.0,0.0,0.0);
  Pr2 = fm.extract_left_camera(zero3,1.0);

#ifndef NDEBUG
  vpgl_fundamental_matrix<double> fm_sane(Pr1,Pr2);
  double ratio;
  for (unsigned i=0; i < 3; ++i)
    for (unsigned k=0; k < 3; ++k) {
      if (fm_sane.get_matrix()(i,k)) {
        ratio = fm_sane.get_matrix()(i,k)/fm.get_matrix()(i,k);
          break;
      }
    }
  for (unsigned i=0; i < 3; ++i)
    for (unsigned k=0; k < 3; ++k) {
      if (fm_sane.get_matrix()(i,k)) {
        double thisratio = fm_sane.get_matrix()(i,k)/fm.get_matrix()(i,k);
        assert(mw_util::near_zero(thisratio-ratio));
      }
    }
#endif

  // reconstruct points in 3D

  // for each match between the two views, reconstruct.

  vcl_vector<vgl_point_3d<double> > pts3d;
  pts3d.reserve(link.matches.size());

  vcl_map<int,int>::const_iterator it = link.matches.begin();
  vcl_map<vcl_pair<int,int>,int> id_pts3d;
  for (; it != link.matches.end(); ++it) {
    const vgl_homg_point_2d<double> p1(*tree.keypoints()[v1][it->first]);
    const vgl_homg_point_2d<double> p2(*tree.keypoints()[v2][it->second]);

    vgl_homg_point_3d<double> P = triangulate_3d_point( p1, Pr1.get_matrix(), p2, Pr2.get_matrix());

    pts3d.push_back(P);
    id_pts3d[vcl_pair<int,int>(it->first,it->second)] = pts3d.size()-1;
  }
  

  
  /*
  unsigned i=0;
  it = link.matches.begin();
  for (; it != link.matches.end(); ++it) {
    const vgl_point_2d<double> p1(*tree.keypoints()[v1][it->first]);
    const vgl_point_2d<double> p2(*tree.keypoints()[v2][it->second]);

    vgl_point_2d<double> p1_reproj(cameras[v1]->project(vgl_homg_point_3d<double>(pts3d[i])));
    vgl_point_2d<double> p2_reproj(cameras[v2]->project(vgl_homg_point_3d<double>(pts3d[i])));
    vgl_point_2d<double> p2_reproj_orig(Pr2.project(vgl_homg_point_3d<double>(pts3d[i])));

    vcl_cout << "Reproj error in p1: " << vgl_distance(p1,p1_reproj) << vcl_endl;;
    vcl_cout << "Reproj error in p2: " << vgl_distance(p2,p2_reproj) << vcl_endl;;
    vcl_cout << "Reproj error in p2_reproj_orig: " << vgl_distance(p2,p2_reproj_orig) << vcl_endl;;

    ++i;
  }
  */
}

unsigned dbkpr_projective_reconstruct_process::
pick_new_view(const vcl_vector<unsigned> &view_set)
{
  // select the view. For now, its anything connected to v1 or v2, whichever has more matches
  //tree.links().[v1] will have at least two, unless its a leaf.

  unsigned max_nmatches = 0; unsigned v_chosen = views[0]; unsigned l_idx = 0;
  bool found_some=false;
  for (unsigned i=0; i < views.size(); ++i) {
    vcl_vector<>::const_iterator l;
    unsigned i=tree.links()[i].size()-1;
    for (l = tree.links()[i].back(); l != tree.links().begin(); --l,--i) {
      if (!in_view_set[l->to] && !in_view_set[l->from])
        continue;

      if (l->matches.size() > max_nmatches) {
        max_nmatches = l->matches.size();
        v_chosen = i
        l_idx = i;
        found_some = true;
      }
    }
  }

  vcl_cout << "Picked new view as: " << v_chosen << vcl_endl;
  // TODO: test this.
  return v_chosen;
}

void dbkpr_projective_reconstruct_process::
add_new_view_to_reconstruction(unsigned v3, 
    vcl_vector<unsigned> &view_set, 
    vcl_vector<vpgl_proj_camera<double> > &cam_set,
    t_corr_set corr_set)
{

  // Get hold of matches in common btw all three views.
  //  - build what I think it is
  //    - assert if matches to tree.global_corr3d 
  //  - make sure it is more than 6
  //  - TODO see if we can get 6 satisfying triple-wise epipolar constraints by computing fm13

  vcl_vector<vcl_vector<int> > triplets; 

  for (it = link12.matches.begin(); it != link12.matches.end(); ++it) {
    vcl_map<int,int>::const_iterator it23 = link23.matches.begin();
    for (; it23 != link23.matches.end(); ++it23) {
      if (it23.first == it->second) {
        // it->first it->second it23.second is a possible triplet

        triplets.push_back(vcl_vector<int>());
        triplets.back().resize(3);
        triplets.back()[0] = it->first;
        triplets.back()[1] = it->second;
        triplets.back()[2] = it->third;

        pts3d_triplets.push_back(pts3d(id_pts3d[vcl_pair<int,int>(it->first,it->second)]));
      }
    }
  }

  if (triplets.size() < 6)
    vcl_cout << "Error: number of trinocular matches is too small\n";

  // Now generate simple least squares solution just to test

  vpgl_proj_camera<double> Pr3;

  // Calls my RANSAC code
  dvpgl_camera_compute_ransac Pr3(image_pts,world_pts, Pr3);


  // TODO: find more correspondences by searching point-wise

  // Refine reconstruction simultaneously using new camera sequence and least-squares with inliers
  
  for (unsigned i=0; i < pts3d.size(); ++i) {
    pts3d[i] = brct_algos::bundle_reconstruct_3d_point(pts, cams);
    //  vgl_point_3d<double> bundle_reconstruct_3d_point(vcl_vector<vnl_double_2> &pts,
    //                                                   vcl_vector<vnl_double_3x4> &cams);
  }

}

void dbkpr_projective_reconstruct_process::
projective_bundle_adjust(vcl_vector<vpgl_proj_camera<double> > &cameras, t_corr_set &corr_set)
{
  //: Gradient tolerance
  unsigned const gtol = 1e-6;
  
  //: Consider only points with 3 or more projections
  unsigned const minproj = 3; 

  bool const verbose=true;

  //: Use adaptive weights for outliers (Gordon and Lowe's idea)
  bool const usewgt = true;
  

  vnl_vector<double> init_a = vpgl_bundle_adj_lsqr::create_param_vector(cam_set);
  vnl_vector<double> init_b = vpgl_bundle_adj_lsqr::create_param_vector(world_points(corr_set));
  vnl_vector<double> a(init_a), b(init_b);

  // do the bundle adjustment
  dvpgl_proj_bundle_adj_lsqr ba_func(image_points,mask,usewgt);
  bnl_sparse_lm lm(ba_func);
  lm.set_max_function_evals(maxitr);
  lm.set_g_tolerance(gtol);
  lm.set_trace(true);
  lm.set_verbose(verbose);
  if(!lm.minimize(a,b))
    return false;
  lm.diagnose_outcome();

  double error1 = lm.get_end_error();

  // Update the camera parameters
  for(unsigned int i=0; i<cameras.size(); ++i)
    cameras[i] = ba_func.param_to_cam(i,a);
    
  // Update the world points
  for(unsigned int j=0; j<world_points.size(); ++j){
    world_points[j] = ba_func.param_to_point(j,b);
    corr3d[j]->set(world_points[j].x(),world_points[j].y(),world_points[j].z());
  }
  
  if(usewgt)
  {
    vcl_vector<double> weights = ba_func.weights();
    const bnl_crs_index& crs = ba_func.residual_indices();
    typedef bnl_crs_index::sparse_vector::iterator sv_itr;
    for(unsigned int i=0; i<cameras.size(); ++i)
    {
      bnl_crs_index::sparse_vector row = crs.sparse_row(i);
      for(sv_itr r_itr=row.begin(); r_itr!=row.end(); ++r_itr)
      {
        unsigned int j = r_itr->second;
        unsigned int k = r_itr->first;
        if(weights[k] < 1.0){
          corr3d[j]->remove_correspondence(i);
        }
      }
    }
  }
  
  
  double avg_dist = 0.0;
  for(unsigned int i=0; i<cameras.size(); ++i){
    vgl_point_3d<double> c1 = cameras[i].camera_center();
    for(unsigned int j=i+1; j<cameras.size(); ++j){
      vgl_point_3d<double> c2 = cameras[j].camera_center();
      avg_dist += vgl_distance(c1,c2);
    }
  }
  avg_dist /= (cameras.size()*(cameras.size()-1))/2.0;
  
  vcl_vector<int> num_meaningful(corr3d.size());
  for(unsigned int k=0; k<corr3d.size(); ++k){
    num_meaningful[k] = corr3d[k]->size();
  }
  for(unsigned int i=0; i<cameras.size(); ++i){
    vgl_point_3d<double> c1 = cameras[i].camera_center();
    vcl_vector<bool> meaningful(corr3d.size(),true);
    for(unsigned int j=i+1; j<cameras.size(); ++j){
      vgl_point_3d<double> c2 = cameras[j].camera_center();
      if(vgl_distance(c1,c2) < avg_dist*.1){
        for(unsigned int k=0; k<corr3d.size(); ++k){
          if(corr3d[k]->in_view(i) && corr3d[k]->in_view(j))
            meaningful[k] = false;
        }
      }
    }
    for(unsigned int k=0; k<corr3d.size(); ++k)
      if(!meaningful[k])
        --num_meaningful[k];
  }
  
  // remove all points without at least minproj meaningful projections
  vcl_vector<dbdet_keypoint_corr3d_sptr> old_corr3d(corr3d); corr3d.clear(); 
  vcl_vector<vgl_point_3d<double> > old_world_points(world_points); world_points.clear();
  for(unsigned int j=0; j<old_corr3d.size(); ++j)
    if(num_meaningful[j] >= minproj){
      corr3d.push_back(old_corr3d[j]);
      world_points.push_back(old_world_points[j]);
    } 

/*
  vnl_vector<double> b2(b);
  // reset cameras
  vnl_vector<double> a2(init_a);

  ba_func.reset();
  // do the bundle adjustment again
  if(!lm.minimize(a2,b2))
    return false;
  lm.diagnose_outcome();

  double error2 = lm.get_end_error();
  if(error2 < error1){
    a = a2;
    b = b2;
  }
*/
  



//  if(vrml_file.path != "") 
//    vpgl_bundle_adjust::write_vrml(vrml_file.path, cameras, world_points);


  // pack up the results into storage classes
  dbkpr_corr3d_storage_sptr in_storage;
  for(unsigned int i=0; i<input_data_.size(); ++i){
    in_storage.vertical_cast(input_data_[i][0]);
    dbkpr_corr3d_storage_sptr out_storage = dbkpr_corr3d_storage_new(some_corr3d, in_storage->ni(),in_storage->nj());
    out_storage->set_camera(new vpgl_perspective_camera<double> (cameras[i]));
    output_data_[i].push_back(out_storage);
  }
}
#endif

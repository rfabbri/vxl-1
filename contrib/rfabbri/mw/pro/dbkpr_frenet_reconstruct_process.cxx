// This is brcv/seg/dbkpr/pro/dbkpr_frenet_reconstruct_process.cxx

//:
// \file

#include "dbkpr_frenet_reconstruct_process.h"
#include <dbdet/dbdet_frenet_keypoint.h>
#include <dbdet/dbdet_keypoint_corr3d.h>
#include <dbkpr/dbkpr_view_span_tree.h>
#include <vpgl/algo/vpgl_bundle_adjust.h>
#include <vnl/algo/vnl_sparse_lm.h>
#include <vnl/vnl_quaternion.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_distance.h>

#include <dbdet/pro/dbdet_keypoint_storage.h>
#include <dbkpr/pro/dbkpr_corr3d_storage_sptr.h>
#include <dbkpr/pro/dbkpr_corr3d_storage.h>

#include <bpro1/bpro1_parameters.h>
#include <dbdif/dbdif_frenet.h>
#include <mw/algo/mw_algo_util.h>
#include <mw/algo/dvpgl_DG_bundle_adjust.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <dvpgl/io/dvpgl_io_cameras.h>


//: Constructor
dbkpr_frenet_reconstruct_process::dbkpr_frenet_reconstruct_process()
{
  if( !parameters()->add( "VRML File",          "-vrmlfile",  bpro1_filepath("","wrl")) ||
      !parameters()->add( "Minimum Projections","-minproj",   (int)3) ||
      !parameters()->add( "Maximum Iterations", "-maxitr",    (int)200) ||
      !parameters()->add( "Gradient Tolerence", "-gtol",      (float)1e-6) ||
      !parameters()->add( "Use Adaptive Weights", "-usewgt",  (bool)false) ||
      !parameters()->add( "Verbose",            "-verbose",   (bool)true) ||
      !parameters()->add( "Output vsol edges" ,             "-bvsol" ,    false )  ||
      !parameters()->add( "Output edgemap" ,            "-blines" ,   true )
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbkpr_frenet_reconstruct_process::~dbkpr_frenet_reconstruct_process()
{
}


//: Clone the process
bpro1_process*
dbkpr_frenet_reconstruct_process::clone() const
{
  return new dbkpr_frenet_reconstruct_process(*this);
}


//: Return the name of this process
vcl_string
dbkpr_frenet_reconstruct_process::name()
{
  return "Reconstruct Frenet Keypoints";
}


//: Return the number of input frame for this process
int
dbkpr_frenet_reconstruct_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbkpr_frenet_reconstruct_process::output_frames()
{
  return 0;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbkpr_frenet_reconstruct_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "keypoints_corr3d" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbkpr_frenet_reconstruct_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "keypoints_corr3d" );

  bool bvsol=false;
  parameters()->get_value( "-bvsol" , bvsol );

  if (bvsol)
    to_return.push_back( "vsol2D" );
  else
    to_return.push_back( "edge_map");


  return to_return;
}


//: Execute the process
bool
dbkpr_frenet_reconstruct_process::execute()
{
  return true;
}

bool
dbkpr_frenet_reconstruct_process::finish()
{

  // cast the storage classes
  vcl_vector<dbdet_keypoint_corr3d_sptr> all_corr3d;
  vcl_vector<vpgl_perspective_camera<double> > cameras;
  cameras.reserve(input_data_.size());
  for(unsigned int i=0; i<input_data_.size(); ++i){
    dbkpr_corr3d_storage_sptr frame_corr3d;
    frame_corr3d.vertical_cast(input_data_[i][0]);
    if(i==0)
      all_corr3d = frame_corr3d->correspondences();

    const vpgl_perspective_camera<double> *pcam;

    pcam = dvpgl_cast_to_perspective_camera(frame_corr3d->get_camera());
    if(!pcam) {
      vcl_cerr << "Error: requires a perspective  camera" << vcl_endl;
      return false;
    }
    cameras.push_back(*pcam);
  }
  unsigned nviews = cameras.size();

  // Make sure all_corr3d's keypoints are of Frenet type

  for (unsigned iv=0; iv < input_data_.size(); ++iv) {
    dbdet_keypoint_sptr kpt = all_corr3d[0]->in_view(iv);
    if (kpt) {
      dbdet_frenet_keypoint *fkpt = dynamic_cast<dbdet_frenet_keypoint *>( kpt.ptr() );
      if (!fkpt) {
        vcl_cerr << "ERROR: Frenet keypoint required\n";
        return false;
      }
      break;
    }
  }

  
  bpro1_filepath vrml_file;
  int maxitr = 0, minproj = 0;
  bool verbose = true, usewgt = false;
  float gtol = 0.0;
  parameters()->get_value( "-vrmlfile" , vrml_file );
  parameters()->get_value( "-minproj" , minproj );
  parameters()->get_value( "-maxitr" , maxitr );
  parameters()->get_value( "-usewgt" , usewgt );
  parameters()->get_value( "-verbose" , verbose );
  parameters()->get_value( "-gtol" , gtol );

  // collect the subset of points with more than 3 projections
  vcl_vector<dbdet_keypoint_corr3d_sptr> some_corr3d;
  for(unsigned int i=0; i<all_corr3d.size(); ++i){
    if(all_corr3d[i]->size() >= (unsigned int)minproj)
      some_corr3d.push_back(new dbdet_keypoint_corr3d(*all_corr3d[i]));
  }

  // Get image_points, world_points in the proper format
  vcl_vector<dbdif_3rd_order_point_2d> image_points;
  vcl_vector<vgl_point_3d<double> > world_points;
  vcl_vector<vcl_vector<bool> > mask(cameras.size(),
                                     vcl_vector<bool>(some_corr3d.size(),false));

  {
    for(unsigned int j=0; j<some_corr3d.size(); ++j)
      world_points.push_back(*some_corr3d[j]);

    for(unsigned int i=0; i<cameras.size(); ++i){
      for(unsigned int j=0; j<some_corr3d.size(); ++j){
        dbdet_frenet_keypoint *kp = dynamic_cast<dbdet_frenet_keypoint *>(some_corr3d[j]->in_view(i).ptr());
        if(kp){
          mask[i][j] = true;
          image_points.push_back(mw_algo_util::mw_get_3rd_order_point_2d(*kp));
        } else
          abort();
      }
    }
  }
  
  //---------- BUNDLE ADJUSTMENT CORE ------------------------------------------------------------

  vcl_cout << "=============================================================\n";
  vcl_cout << "Cameras BEFORE Differential-Geometric cost bundle adjustment:\n";



  vcl_vector<vpgl_calibration_matrix<double> > Ks;
  vnl_vector<double> init_a = vpgl_bundle_adj_lsqr::create_param_vector(cameras);
  vnl_vector<double> init_b = vpgl_bundle_adj_lsqr::create_param_vector(world_points);

  for(unsigned int i=0; i<cameras.size(); ++i){
    Ks.push_back(cameras[i].get_calibration());
    vcl_cout << "Camera:\n";
    vcl_cout << cameras[i] << vcl_endl;
  }

  vcl_cout << "\nReprojection errors BEFORE bundle adjustment: \n";


  {

    vcl_vector<dbdif_camera> ecam(nviews);
    for (unsigned iv=0; iv < nviews; ++iv)
      ecam[iv].set_p(cameras[iv]);

    vcl_vector<double> dthetas;
    double dpos_total=0;
    double dtheta_total=0;
  //  double dk_total=0;
  //  double dkdot_total=0;
    unsigned n_total=0;
    double dnormal_plus_total=0;
    vcl_vector<dbdif_3rd_order_point_2d> pts;
    pts.resize(nviews);

    for (unsigned ip=0; ip < some_corr3d.size(); ++ip) {

      for (unsigned iv=0; iv < nviews; ++iv) {
        dbdet_frenet_keypoint *kp = dynamic_cast<dbdet_frenet_keypoint *>(some_corr3d[ip]->in_view(iv).ptr());
        if(kp){
          pts[iv] = mw_algo_util::mw_get_3rd_order_point_2d(*kp);
        } else
          abort();
      }
      
      double dpos, dtheta, dnormal_plus, dnormal_minus, dtangential_plus, dtangential_minus, dk, dkdot;
      unsigned n;

      bool valid;
      for (unsigned l=0; l < ecam.size(); ++l) {

        valid = mw_algo_util::dg_reprojection_error(
            pts,ecam,l,dpos,
            dtheta,dnormal_plus,dnormal_minus, 
            dtangential_plus, dtangential_minus, dk,dkdot,n);

        if (valid) {
          dpos_total   += dpos;
          dtheta_total += dtheta;
          dthetas.push_back(dtheta/n);
          dnormal_plus_total += dnormal_plus;

          n_total+=n;
        }
      }
    }
    dpos_total = vcl_sqrt(dpos_total);
    dtheta_total = dtheta_total/(double)n_total;

    vcl_cout << "       Pos reproj error using DG: " << dpos_total << vcl_endl;
    vcl_cout << "Theta reproj error using DG(rad): " << dtheta_total << vcl_endl;
    vcl_cout << "           # valid reprojections: " << n_total << vcl_endl;

    unsigned dummy;
    vcl_cout<< "       dtheta max: " << mw_util::max(dthetas,dummy);
    vcl_cout<< " (idx = " << dummy << "of " << dthetas.size() << ")" << vcl_endl;
    vcl_cout<< "       dtheta min: " << mw_util::min(dthetas,dummy) << vcl_endl;
    vcl_cout<< "       dtheta avg: " << mw_util::mean(dthetas) << vcl_endl;
  }
  vcl_cout << "\n\n";



  // ------- CORE
  
  vnl_vector<double> a(init_a), b(init_b);
  dvpgl_DG_bundle_adj_lsqr ba_func(Ks,image_points,mask,usewgt);
  vnl_sparse_lm lm(ba_func);
  lm.set_max_function_evals(maxitr);
  lm.set_g_tolerance(gtol);
  lm.set_trace(true);
  lm.set_verbose(verbose);
  if(!lm.minimize(a,b)) {
    vcl_cout << "No convergence...\n";
//    return false;
  }
  lm.diagnose_outcome();

  // OBS: function vpgl_bundle_adjust::optimze is not being called..
  // -------  END CORE
  
  // Flip points (removed code)
  
  // Update the camera parameters
  for(unsigned int i=0; i<cameras.size(); ++i)
    cameras[i] = ba_func.param_to_cam(i,a);
    
  // Update the world points
  for(unsigned int j=0; j<world_points.size(); ++j){
    world_points[j] = ba_func.param_to_point(j,b);
    some_corr3d[j]->set(world_points[j].x(),world_points[j].y(),world_points[j].z());
  }
  
  if(usewgt)
  {
    vcl_cout << "Not yet coded\n";
    abort();
    vcl_vector<double> weights = ba_func.weights();
    const vnl_crs_index& crs = ba_func.residual_indices();
    typedef vnl_crs_index::sparse_vector::iterator sv_itr;
    for(unsigned int i=0; i<cameras.size(); ++i)
    {
      vnl_crs_index::sparse_vector row = crs.sparse_row(i);
      for(sv_itr r_itr=row.begin(); r_itr!=row.end(); ++r_itr)
      {
        unsigned int j = r_itr->second;
        unsigned int k = r_itr->first;
        if(weights[k] < 1.0){
          some_corr3d[j]->remove_correspondence(i);
        }
      }
    }
  }

  //---------- ! Bundle Adjustment Core ------------------------------------------------------------
  
  
  // Compute number of meaningful projections for each corr3d. A projected point is deemed
  // meaningful if for some other view containing a corresponding projected point, the distance of
  // the two view's camera centers is greater than 10% the average distance between all pairs of
  // cameras.
  
  
  double avg_dist = 0.0;
  for(unsigned int i=0; i<cameras.size(); ++i){
    vgl_point_3d<double> c1 = cameras[i].camera_center();
    for(unsigned int j=i+1; j<cameras.size(); ++j){
      vgl_point_3d<double> c2 = cameras[j].camera_center();
      avg_dist += vgl_distance(c1,c2);
    }
  }
  avg_dist /= (cameras.size()*(cameras.size()-1))/2.0;
  
  vcl_vector<int> num_meaningful(some_corr3d.size());
  for(unsigned int k=0; k<some_corr3d.size(); ++k){
    num_meaningful[k] = some_corr3d[k]->size();
  }
  for(unsigned int i=0; i<cameras.size(); ++i){
    vgl_point_3d<double> c1 = cameras[i].camera_center();
    vcl_vector<bool> meaningful(some_corr3d.size(),true);
    for(unsigned int j=i+1; j<cameras.size(); ++j){
      vgl_point_3d<double> c2 = cameras[j].camera_center();
      if(vgl_distance(c1,c2) < avg_dist*.1){
        for(unsigned int k=0; k<some_corr3d.size(); ++k){
          if(some_corr3d[k]->in_view(i) && some_corr3d[k]->in_view(j))
            meaningful[k] = false;
        }
      }
    }
    for(unsigned int k=0; k<some_corr3d.size(); ++k)
      if(!meaningful[k])
        --num_meaningful[k];
  }
  
  // remove all points without at least minproj meaningful projections
  vcl_vector<dbdet_keypoint_corr3d_sptr> old_corr3d(some_corr3d); some_corr3d.clear(); 
  vcl_vector<vgl_point_3d<double> > old_world_points(world_points); world_points.clear();
  for(unsigned int j=0; j<old_corr3d.size(); ++j)
    if(num_meaningful[j] >= minproj){
      some_corr3d.push_back(old_corr3d[j]);
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
  
  //----------------------------------------------------------------------


  if(vrml_file.path != "") 
    vpgl_bundle_adjust::write_vrml(vrml_file.path, cameras, world_points);



  bool blines=false, bvsol=false;

  parameters()->get_value( "-bvsol" , bvsol );
  parameters()->get_value( "-blines" , blines );
  // edge_map 
  dbdet_edgemap_sptr edge_map;

  // edgels (vsol)
  vcl_vector< vsol_spatial_object_2d_sptr > edgels;



  // pack up the results into storage classes
  dbkpr_corr3d_storage_sptr in_storage;
  for(unsigned int i=0; i<input_data_.size(); ++i){
    in_storage.vertical_cast(input_data_[i][0]);

    dbkpr_corr3d_storage_sptr out_storage = dbkpr_corr3d_storage_new(some_corr3d, in_storage->ni(),in_storage->nj());

    out_storage->set_camera(new vpgl_perspective_camera<double> (cameras[i]));
    output_data_[i].push_back(out_storage);

    // Populate Edge structures

    
    edge_map = new dbdet_edgemap(in_storage->ni(),in_storage->nj());

    for (unsigned p=0; p < some_corr3d.size(); ++p) {
      dbdet_keypoint_sptr kpt = some_corr3d[p]->in_view(i);
      dbdet_frenet_keypoint *fp = dynamic_cast<dbdet_frenet_keypoint *>( kpt.ptr() );

      assert(fp);

      dbdif_3rd_order_point_2d frenet_pt = mw_algo_util::mw_get_3rd_order_point_2d(*fp);

      vgl_point_2d<double> pt(cameras[i](vgl_homg_point_3d<double>(world_points[p])));

      frenet_pt.gama[0]=pt.x();
      frenet_pt.gama[1]=pt.y();


      double x; double y; double dir;

      x = frenet_pt.gama[0];
      y = frenet_pt.gama[1];
      dir = vcl_atan2(frenet_pt.t[1],frenet_pt.t[0]);

      if (bvsol){
        if ( blines){
          vsol_line_2d_sptr newLine = new vsol_line_2d(vgl_vector_2d<double>(vcl_cos(dir)/2.0, vcl_sin(dir)/2.0), vgl_point_2d<double>(x,y));
          edgels.push_back(newLine->cast_to_spatial_object());
        }
        else {
          vsol_point_2d_sptr newPt = new vsol_point_2d(x,y);
          edgels.push_back(newPt->cast_to_spatial_object());
        }
      }
      else {
        //create an edgel token
        edge_map->insert(new dbdet_edgel(vgl_point_2d<double>(x, y), dir));
      }
    }

    // Output to repository
    if (bvsol){
      vcl_cout << "N edgels: " << edgels.size() << vcl_endl;
      // create the output storage class
      vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
      output_vsol->add_objects(edgels,"bundle_adjusted_edgels");
      output_data_[i].push_back(output_vsol); //: TODO check this.
    }  
    else {
      vcl_cout << "N edgels: " << edge_map->num_edgels() << vcl_endl;
      // create the output storage class
      dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
      output_edgemap->set_edgemap(edge_map);
      output_data_[i].push_back(output_edgemap);
    }
  }







  vcl_cout << "==========================================\n";
  vcl_cout << "Cameras after bundle adjustment:\n";
  for (unsigned i=0; i < cameras.size(); ++i) {
    vcl_cout << cameras[i] << vcl_endl;
  }



  return true;
}

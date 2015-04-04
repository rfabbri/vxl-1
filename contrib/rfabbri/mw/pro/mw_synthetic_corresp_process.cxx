#include "mw_synthetic_corresp_process.h"
#include <vnl/vnl_double_3x3.h>
#include <dbdet/dbdet_keypoint_corr3d.h>
#include <dbdet/dbdet_keypoint.h>
#include <dbkpr/pro/dbkpr_corr3d_storage.h>
#include <vgl/vgl_distance.h>
#include <dbdif/algo/dbdif_data.h>
#include <mw/algo/mw_algo_util.h>
#include <bpgl/algo/bpgl_construct_cameras.h>

#include <bpro1/bpro1_parameters.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/dbdet_frenet_keypoint.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>



//: Constructor
mw_synthetic_corresp_process::mw_synthetic_corresp_process()
{
  if( 
      !parameters()->add( "Output vsol edges" ,             "-bvsol" ,    false )  ||
      !parameters()->add( "Output edgemap" ,            "-blines" ,   true )
    ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
mw_synthetic_corresp_process::~mw_synthetic_corresp_process()
{
}


//: Clone the process
bpro1_process*
mw_synthetic_corresp_process::clone() const
{
  return new mw_synthetic_corresp_process(*this);
}


//: Return the name of this process
vcl_string
mw_synthetic_corresp_process::name()
{
  return "Synthetic Correspondences";
}


//: Return the number of input frame for this process
int
mw_synthetic_corresp_process::input_frames()
{
  return 0;
}


//: Return the number of output frames for this process
int
mw_synthetic_corresp_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > mw_synthetic_corresp_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
//  to_return.push_back( "keypoints" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > mw_synthetic_corresp_process::get_output_type()
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
mw_synthetic_corresp_process::execute()
{
  return true;
}


//-----------------------------------------------------------------------------

bool mw_synthetic_corresp_process::
finish()
{

  vpgl_calibration_matrix<double> *K;
  //: global outputs: vgl_pts_, pt_id_, pcam_
  unsigned ncols,nrows;
    define_dataset(&K,ncols,nrows);

//  initialize_poses(*K, pcam_ini_, vgl_pts_ini_);


  // Compute the global correspondence points
  vcl_vector<dbdet_keypoint_corr3d_sptr> corr_pts;
    generate_corr_pts(corr_pts);

//  vnl_matrix_fixed<double,3,3> rot;
//  rot.set_identity();
//  vgl_h_matrix_3d<double> R(rot, vnl_vector_fixed< double, 3 >(0.0));
//  vgl_point_3d<double> center(0,0,-10);

  vcl_cout << "# Output frames : " << output_data_.size() << vcl_endl;


  bool blines=false, bvsol=false;

  parameters()->get_value( "-bvsol" , bvsol );
  parameters()->get_value( "-blines" , blines );
  // edge_map 
  dbdet_edgemap_sptr edge_map;

  // edgels (vsol)
  vcl_vector< vsol_spatial_object_2d_sptr > edgels;


  // pack up the results into storage classes
  for(unsigned int i=0; i<output_data_.size(); ++i) {
//    vpgl_perspective_camera<double> *default_cam = new vpgl_perspective_camera<double>(*K,center,R);
//    vpgl_perspective_camera<double> *default_cam = pcam_ini_[i];
    vpgl_perspective_camera<double> *default_cam = pcam_[i];

    dbkpr_corr3d_storage_sptr out_storage = dbkpr_corr3d_storage_new(corr_pts, ncols, nrows);

    out_storage->set_camera(default_cam);
    output_data_[i].push_back(out_storage);


    // Populate Edge structures

    
    edge_map = new dbdet_edgemap(ncols,nrows);

    for (unsigned p=0; p < pt_id_.size(); ++p) {
      dbdet_keypoint_sptr kpt = corr_pts[p]->in_view(i);
      dbdet_frenet_keypoint *fp = dynamic_cast<dbdet_frenet_keypoint *>( kpt.ptr() );

      assert(fp);

      dbdif_3rd_order_point_2d frenet_pt = mw_algo_util::mw_get_3rd_order_point_2d(*fp);

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
      output_vsol->add_objects(edgels,"myedgels");
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

  delete K;

  return true;
}

//-----------------------------------------------------------------------------

void 
mw_synthetic_corresp_process::
define_dataset(vpgl_calibration_matrix<double> **K, unsigned &ncols, unsigned &nrows)
{
  // For now, using regular digital camera turntable dataset

  vnl_double_3x3 Kmatrix;

  nrows = 400;
  ncols = 500;
  unsigned  crop_origin_x = 450;
  unsigned  crop_origin_y = 1750;
  dbdif_turntable::internal_calib_olympus(Kmatrix, ncols, crop_origin_x, crop_origin_y);

  *K = new vpgl_calibration_matrix<double> (Kmatrix);

  vpgl_perspective_camera<double> *P;


  nviews_ = 3; //: for now.

  vcl_vector<unsigned> angles;
  angles.push_back(30);
  angles.push_back(50);
  angles.push_back(70);

  vcl_vector<dbdif_camera> cam;

  cam.resize(nviews_);

  pcam_.resize(nviews_);
  for (unsigned i=0; i < nviews_; ++i) {
    P = dbdif_turntable::camera_olympus(angles[i], **K);
    cam[i].set_p(*P);
    pcam_[i] = P;
  }

  vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > crv3d;
  dbdif_data::space_curves_digicam_turntable_medium_sized( crv3d );

  dbdif_data::project_into_cams(crv3d, cam, crv2d_);
  vcl_cout << "Number of samples INCLUDING epitangencies: " << crv2d_[0].size() << vcl_endl;

  for (unsigned n=0; n < crv2d_[0].size(); n+=1)
    pt_id_.push_back(n);

  for (unsigned i=0; i < pt_id_.size(); ++i) {
    vgl_pts_.push_back(dbdif_data::get_point_crv3d(crv3d,pt_id_[i]));
  }

  mw_algo_util::move_world_to_1st_cam(pcam_,vgl_pts_);
  for (unsigned i=0; i < pcam_.size(); ++i) {
    vcl_cout << "Camera view index " << i << ":" << vcl_endl;
    vcl_cout << *(pcam_[i]) << vcl_endl;
  }

  { // sanity test

    // projection should be good

    double d_total=0;
    for (unsigned ip=0; ip < vgl_pts_.size(); ++ip) {

      for (unsigned iv=0; iv < nviews_; ++iv) {
        vgl_point_2d<double>p_proj = pcam_[iv]->project(vgl_pts_[ip]);

        vgl_point_2d<double> p_orig(crv2d_[iv][pt_id_[ip]].gama[0],crv2d_[iv][pt_id_[ip]].gama[1]);
        double d=vgl_distance(p_proj,p_orig);
        assert (d < 1e-5);
        d_total += d*d;
      }
    }
    d_total = vcl_sqrt(d_total);
    vcl_cout << "Total reprojection error after coordinate transf to 1st cam: " << d_total << vcl_endl;
  }
}

//-----------------------------------------------------------------------------

void  mw_synthetic_corresp_process::
generate_corr_pts(vcl_vector<dbdet_keypoint_corr3d_sptr> &corr_pts)
{
  corr_pts.resize(pt_id_.size());

  for (unsigned i=0; i < vgl_pts_.size(); ++i) {
//    dbdet_keypoint_corr3d_sptr corr = new dbdet_keypoint_corr3d(vgl_pts_ini_[i].x(),vgl_pts_ini_[i].y(),vgl_pts_ini_[i].z());
//    dbdet_keypoint_corr3d_sptr corr = new dbdet_keypoint_corr3d(vgl_pts_[i].x()+100,vgl_pts_[i].y()-20,vgl_pts_[i].z()+10);
    dbdet_keypoint_corr3d_sptr corr = new dbdet_keypoint_corr3d(vgl_pts_[i].x()+0.1,vgl_pts_[i].y()-0.1,vgl_pts_[i].z()+0.1);
//    dbdet_keypoint_corr3d_sptr corr = new dbdet_keypoint_corr3d(vgl_pts_[i].x(),vgl_pts_[i].y(),vgl_pts_[i].z());

    for (unsigned iv=0; iv < nviews_; ++iv) {
      dbdet_keypoint_sptr p;
//      p = new dbdet_keypoint(crv2d_[iv][pt_id_[i]].gama[0],crv2d_[iv][pt_id_[i]].gama[1]);

      p = new dbdet_frenet_keypoint(mw_algo_util::mw_get_frenet_keypoint(crv2d_[iv][pt_id_[i]]));
      corr->add_correspondence(p,iv);
    }

    corr_pts[i] = corr;
  }
}


//-----------------------------------------------------------------------------
//: Initializes pose solution using 8-point correspondences
//
// TODO: move to dbkpr_reconstruct_process
void mw_synthetic_corresp_process::
initialize_poses(
    const vpgl_calibration_matrix<double> &K, 
    vcl_vector<vpgl_perspective_camera<double> *> &pcam_ini,
    vcl_vector<vgl_point_3d<double> > &vgl_pts_ini
    ) const
{
  pcam_ini.resize(nviews_);
  vcl_vector<vgl_point_2d<double> > p0;
  p0.reserve(pt_id_.size());
  for (unsigned ip=0; ip < pt_id_.size(); ++ip) {
    p0.push_back(vgl_point_2d<double>(crv2d_[0][pt_id_[ip]].gama[0],crv2d_[0][pt_id_[ip]].gama[1]));
  }


  vcl_vector<vcl_vector< vgl_point_3d<double> > > world_points_ini;
  world_points_ini.resize(nviews_);

  for (unsigned iv=1; iv < pcam_ini.size(); ++iv) {
    vcl_vector<vgl_point_2d<double> > p1;
    p1.reserve(pt_id_.size());
    for (unsigned ip=0; ip < pt_id_.size(); ++ip) {
      p1.push_back(vgl_point_2d<double>(crv2d_[iv][pt_id_[ip]].gama[0],crv2d_[iv][pt_id_[ip]].gama[1]));
    }

    bpgl_construct_cameras cstr(p0,p1,&K);
    cstr.construct();
    cstr.get_world_points(world_points_ini[iv]);
    pcam_ini[iv]  = new vpgl_perspective_camera<double> (cstr.get_camera2());
    if (iv == 1)
      pcam_ini[0] = new vpgl_perspective_camera<double> (cstr.get_camera1());
  }

  for (unsigned i=0; i < pcam_ini.size(); ++i) {
    vcl_cout << "Essential-matrix-initialized camera view index " << i << ":" << vcl_endl;
    vcl_cout << *(pcam_ini[i]) << vcl_endl;
  }

  // Renormalize the translations to have only 1 global ambiguous scale
  // TODO: guarantee that world_points_ini[1][0] is not near any baseline.
  for (unsigned iv=2; iv < nviews_; ++iv) {
    assert(!mw_util::near_zero(mw_util::vgl_to_vnl(world_points_ini[iv][0]).two_norm()));
    double scale = mw_util::vgl_to_vnl(world_points_ini[1][0]).two_norm() / mw_util::vgl_to_vnl(world_points_ini[iv][0]).two_norm();

    vgl_point_3d<double> c = pcam_ini[iv]->get_camera_center();
    c.set(c.x()*scale, c.y()*scale, c.z()*scale);
    pcam_ini[iv]->set_camera_center(c);
  }

  vgl_pts_ini = world_points_ini[1]; 


  { // sanity test

    // projection should be good

    double d_total=0;
    unsigned cnt=0;
    for (unsigned ip=0; ip < vgl_pts_.size(); ++ip) {

      assert(vgl_pts_.size() == world_points_ini[1].size());
      for (unsigned iv=0; iv < nviews_; ++iv) {
        vgl_point_2d<double>p_proj = pcam_ini[iv]->project(world_points_ini[1][ip]);

        vgl_point_2d<double> p_orig(crv2d_[iv][pt_id_[ip]].gama[0],crv2d_[iv][pt_id_[ip]].gama[1]);
        double d=vgl_distance(p_proj,p_orig);
        if (d > 1e-5 && cnt < 10) {
          vcl_cout << "Warning: reproj error " << d << " point id: " << pt_id_[ip] 
            << "\np_proj: " << p_proj << " p_orig: " << p_orig << vcl_endl;
          ++cnt;
        }

        d_total += d*d;
      }
    }
    d_total = vcl_sqrt(d_total);
    vcl_cout << "Total reprojection error after closed-form initialization: " << d_total << vcl_endl;
  }
}
/*
 * Camera view index 0:
perspective:
K
2900.4 0 249.804
0 2900.25 -205.363
0 0 1

R
1 0 0 0
0 1 0 0
0 0 1 0
0 0 0 1

C <vgl_point_3d 0,0,0>

P
2900.4 0 249.804 -0
0 2900.25 -205.363 0
0 0 1 -0


Camera view index 1:
perspective:
K
2900.4 0 249.804
0 2900.25 -205.363
0 0 1

R
0.939693 -0.0405256 0.339611 0
0.0405256 0.999153 0.00709542 0
-0.339611 0.00709542 0.940539 0
0 0 0 1

C <vgl_point_3d 374.164,-7.81734,65.5105>

P
2640.65 -115.768 1219.96 -1.06886e+06
187.278 2896.33 -172.574 -36125.7
-0.339611 0.00709542 0.940539 65.5105


Camera view index 2:
perspective:
K
2900.4 0 249.804
0 2900.25 -205.363
0 0 1

R
0.766044 -0.0761632 0.638259 0
0.0761632 0.996715 0.0275259 0
-0.638259 0.0275259 0.769329 0
0 0 0 1

C <vgl_point_3d 703.199,-30.3265,254.141>
normalized : 0.939692  -0.040526   0.339611

P
2062.4 -214.028 2043.39 -1.97608e+06
351.967 2885.07 -78.1601 -140145
-0.638259 0.0275259 0.769329 254.141
*/

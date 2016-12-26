// This is brcv/trk/dbmrf/pro/dbmrf_curve_3d_builder_process.cxx

//:
// \file

#include "dbmrf_curve_3d_builder_process.h"

#include "dbmrf_bmrf_storage.h"
#include "dbmrf_curvel_3d_storage.h"

#include <bmrf/bmrf_network.h>
#include <bmrf/bmrf_curve_3d_builder.h>
#include <bmrf/bmrf_curvel_3d.h>
#include <bmrf/bmrf_curve_3d.h>

#include <vcl_limits.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_det.h>


//: Constructor
dbmrf_curve_3d_builder_process::dbmrf_curve_3d_builder_process() : bpro1_process()
{
  if( !parameters()->add( "Minimum number of projections" , "-min_prj", (int)5 ) ||
      !parameters()->add( "Minimum number of points" ,      "-min_len", (int)10 ) ||
      !parameters()->add( "Smoothing Kernel Sigma" ,        "-sigma",   0.5f ) ||
      !parameters()->add( "Gamma St Dev Threshold" ,        "-trim_thresh", 0.001f ) ||
      !parameters()->add( "Bounding Box Inlier Fraction X" ,  "-inlierX",  0.95f ) ||
      !parameters()->add( "Bounding Box Inlier Fraction Y" ,  "-inlierY",  0.95f ) ||
      !parameters()->add( "Bounding Box Inlier Fraction Z" ,  "-inlierZ",  1.0f ) ||
      !parameters()->add( "Reverse motion direction" ,        "-reverse",  false ) ||
      !parameters()->add( "Camera File",                    "-camera",  bpro1_filepath("","*")) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  } 
}


//: Destructor
dbmrf_curve_3d_builder_process::~dbmrf_curve_3d_builder_process()
{
}


//: Clone the process
bpro1_process* 
dbmrf_curve_3d_builder_process::clone() const
{
  return new dbmrf_curve_3d_builder_process(*this);
}


//: Return the name of the process
vcl_string
dbmrf_curve_3d_builder_process::name()
{
  return "Build 3D Curves";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbmrf_curve_3d_builder_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "bmrf" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbmrf_curve_3d_builder_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "curvel_3d" );
  return to_return;
}


//: Returns the number of input frames to this process
int
dbmrf_curve_3d_builder_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbmrf_curve_3d_builder_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbmrf_curve_3d_builder_process::execute()
{
  return true;
}


//: Finish
bool
dbmrf_curve_3d_builder_process::finish()
{
  // find the first frame with input
  unsigned int frame = 0;
  bpro1_storage_sptr input_storage = NULL;
  for(; frame < input_data_.size(); ++frame)
    if(!input_data_[frame].empty() && input_data_[frame][0])
      break;

  if(frame >= input_data_.size())
    return false;

  // extract the network
  dbmrf_bmrf_storage_sptr frame_network;
  frame_network.vertical_cast(input_data_[frame][0]);
  bmrf_network_sptr network = frame_network->network();

  bool reverse=false;
  parameters()->get_value( "-reverse" , reverse );
  
  // prune the network
  network->prune_by_probability(0.001, true);
  if(reverse)
    network->prune_by_gamma(-1, -0.01);
  else
    network->prune_by_gamma(0.01, 1);
  network->prune_directed();

  // read the parameters
  int min_prj=0, min_len=0;
  float sigma=0;
  float inlier[3];
  float trim_thresh=0;
  bpro1_filepath camera_path;
  parameters()->get_value( "-min_prj", min_prj );
  parameters()->get_value( "-min_len", min_len );
  parameters()->get_value( "-sigma",   sigma );
  parameters()->get_value( "-trim_thresh",   trim_thresh );
  parameters()->get_value( "-inlierX",  inlier[0] );
  parameters()->get_value( "-inlierY",  inlier[1] );
  parameters()->get_value( "-inlierZ",  inlier[2] );  
  parameters()->get_value( "-camera" , camera_path );

  // load or create a camera for frame 0
  vnl_double_3x4 C;
  bool world_camera = false;
  vcl_ifstream fp(camera_path.path.c_str());
  if(fp.is_open()){
    fp >> C;
    fp.close();
    // Make the camera right-handed
    if(vnl_det(vnl_double_3x3(C.extract(3,3))) < 0)
      C *= -1;
    world_camera = true;
  }
  else{
    // Internal Calibration Matrix
    vnl_double_3x3 K;
    K[0][0] = 2000;  K[0][1] = 0;      K[0][2] = 512;
    K[1][0] = 0;     K[1][1] = 2000;   K[1][2] = 384;
    K[2][0] = 0;     K[2][1] = 0;      K[2][2] = 1;

    // Identity camera
    C[0][0] = 1;  C[0][1] = 0;  C[0][2] = 0;  C[0][3] = 0;
    C[1][0] = 0;  C[1][1] = 1;  C[1][2] = 0;  C[1][3] = 0;
    C[2][0] = 0;  C[2][1] = 0;  C[2][2] = 1;  C[2][3] = 0;

    C = K*C;
  }
  
  
  bmrf_curve_3d_builder builder;
  builder.set_network(network);

  // build correspondences from the network
  if( !builder.build(min_prj, min_len, trim_thresh) )
    return false;

  // initialize the cameras
  if(reverse)
    builder.init_cameras(C,-1.0);
  else
    builder.init_cameras(C);
  // reconstruct the points
  builder.reconstruct(sigma);


  // compute the bounding box using the camera for orientation
  // also rescale so that the bounding box is on the ground plane
  if(world_camera){
    // compute the camera height
    vnl_double_4 center = vnl_svd<double>(C).nullvector();
    double cam_height = center[2]/center[3];

    // find the bottom of the bounding box
    builder.compute_bounding_box(inlier);
    vnl_double_4x4 bb_xform = builder.bb_xform();
    double min_z = vcl_numeric_limits<double>::infinity();
    for(int i=0; i<8; ++i){ // for each corner
      vnl_double_4 corner = bb_xform*vnl_double_4(double(i/4),double((i/2)%2),double(i%2),1.0);
      double z = corner[2]/corner[3];
      if(z < min_z)
        min_z = z;
    }
    // find a scale that puts the bottom of bounding box at z=0.0 
    double scale = (cam_height-0.0)/(cam_height - min_z );
    vcl_cerr << "scale = " << scale << "  min z = " << min_z << vcl_endl;
    if(reverse)
      scale *= -1;
  
    // recompute everything with the new scale
    builder.init_cameras(C,scale);
    builder.reconstruct(sigma);
    builder.compute_bounding_box(inlier);
  }
  // compute the bounding box using the epipole for partial orientation
  else
    builder.compute_bounding_box(inlier, true);


  vcl_set<bmrf_curve_3d_sptr> curves = builder.curves();
  vcl_map<int,vnl_double_3x4> cameras = builder.cameras();
  vgl_vector_3d<double> direction = builder.direction();
  vnl_double_4x4 bb_xform = builder.bb_xform(); 

  if(output_data_.size() < input_data_.size())
    output_data_.resize(input_data_.size());

  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
    dbmrf_curvel_3d_storage_sptr output_curves = 
      dbmrf_curvel_3d_storage_new(curves, cameras[frame], direction, bb_xform);
    output_data_[frame].push_back(output_curves);
  }

  vcl_cout << "Bounding Box Transform\n" << bb_xform << vcl_endl;
  for ( vcl_map<int,vnl_double_3x4>::const_iterator C_itr = cameras.begin();
        C_itr != cameras.end();  ++C_itr )
    vcl_cout << "Camera " << C_itr->first << "\n" << C_itr->second << vcl_endl;

  return true;
}





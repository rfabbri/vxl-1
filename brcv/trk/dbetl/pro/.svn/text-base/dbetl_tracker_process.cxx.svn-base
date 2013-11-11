// This is brl/dvidpro1/process/dbetl_tracker_process.cxx

//:
// \file

#include "dbetl_tracker_process.h"
#include <bpro1/bpro1_parameters.h>

#include "dbetl_track_storage.h"
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbecl/pro/dbecl_episeg_storage.h>

#include <dbecl/dbecl_episeg.h>
#include <dbetl/dbetl_camera.h>
#include <dbetl/dbetl_point_track.h>


//: Constructor
dbetl_tracker_process::dbetl_tracker_process()
 : bpro1_process(), tracker_(NULL)
{
  if( !parameters()->add( "Min Angle",         "-min_a", 0.0f) ||
      !parameters()->add( "Max Angle",         "-max_a", 0.0f) ||
      !parameters()->add( "Number of Samples", "-num",   10) ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  } 
}


//: Copy Constructor
dbetl_tracker_process::
dbetl_tracker_process(const dbetl_tracker_process& other)
: bpro1_process(other), tracker_(NULL)
{
}


//: Destructor
dbetl_tracker_process::~dbetl_tracker_process()
{
  delete tracker_;
}


//: Clone the process
bpro1_process* 
dbetl_tracker_process::clone() const
{
  return new dbetl_tracker_process(*this);
}


//: Return the name of the process
vcl_string
dbetl_tracker_process::name()
{
  return "Track Epi-Points";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbetl_tracker_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "episeg" );
  to_return.push_back( "image" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbetl_tracker_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbetl" );
  return to_return;
}


//: Returns the number of input frames to this process
int
dbetl_tracker_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbetl_tracker_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbetl_tracker_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << __FILE__ << " - not exactly one input frame" << vcl_endl;
    return false;
  }

  if(!tracker_){
    float min_a=0, max_a=0;
    int num=0;
    parameters()->get_value( "-min_a", min_a );
    parameters()->get_value( "-max_a", max_a );
    parameters()->get_value( "-num",   num );
    tracker_ = new dbetl_tracker(min_a, max_a, num);
  }


  dbecl_episeg_storage_sptr frame_episegs;
  frame_episegs.vertical_cast(input_data_[0][0]);
  vcl_vector<dbecl_episeg_sptr> episegs = frame_episegs->episegs();

  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][1]);

  vgl_point_2d<double> ep = episegs.front()->epipole()->location();
  double mag = 1.0;//vcl_sqrt(ep.x()*ep.x() + ep.y()*ep.y() + 1);
  double t = -frame_episegs->frame()/mag;

  vnl_double_3x4 C;
  C[0][0] = 2000;  C[0][1] = 0;     C[0][2] = 512;  C[0][3] = t*ep.x();
  C[1][0] = 0;     C[1][1] = 2000;  C[1][2] = 384;  C[1][3] = t*ep.y();
  C[2][0] = 0;     C[2][1] = 0;     C[2][2] = 1;    C[2][3] = t;
  dbetl_camera_sptr bcam = new dbetl_camera(C);

  vcl_cout << "frame: " << t << vcl_endl;
  vcl_cout << "Camera = \n" << C << vcl_endl; 

  tracker_->set_camera(bcam);
  tracker_->set_image(frame_image->get_image());
  tracker_->set_episegs(episegs);
  tracker_->track();

  vcl_cout << "num tracks = " << tracker_->tracks()[0].size() << vcl_endl;
  vcl_cout << "best prob = " << tracker_->tracks()[0].front()->error() << vcl_endl;

  // create the output storage class
  dbetl_track_storage_sptr output_dbetl = dbetl_track_storage_new();
  output_dbetl->set_tracks(tracker_->tracks());
  output_dbetl->set_camera(bcam);
  output_data_[0].push_back(output_dbetl);

  return true;
}


//: Finish
bool
dbetl_tracker_process::finish()
{
  return true;
}


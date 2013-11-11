// This is brl/blem/brcv/mvg/dbirl/pro/dbirl_h_computation_process.cxx
//:
// \file
#include <vsol/vsol_point_2d.h>
#include <vsl/vsl_binary_io.h>
#include <dbinfo/dbinfo_track.h>
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <dbinfo/pro/dbinfo_track_storage_sptr.h>
#include <dbirl/dbirl_h_computation.h>
#include <dbirl/pro/dbirl_h_computation_process.h>

//: Constructor
dbirl_h_computation_process::dbirl_h_computation_process()
{
  if( !parameters()->add( "Input file < filename ..>" , "-fin" ,  bpro1_filepath("","*.*")) ||
      !parameters()->add( "Target Frame", "-tf", (int)0 ) ||
      !parameters()->add( "Output file < filename ..>" , "-fout" ,  bpro1_filepath("","*.*")))
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
} 

//: Destructor
dbirl_h_computation_process::~dbirl_h_computation_process()
{
}


//: Clone the process
bpro1_process* 
dbirl_h_computation_process::clone() const
{
  return new dbirl_h_computation_process(*this);
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbirl_h_computation_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  //to_return.push_back( "dbinfo_track_storage" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbirl_h_computation_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}


//: Returns the number of input frames to this process
int
dbirl_h_computation_process::input_frames()
{
  return 2;
}


//: Returns the number of output frames from this process
int
dbirl_h_computation_process::output_frames()
{
  return 0;
}


//: Run the process on the current frame
bool
dbirl_h_computation_process::execute()
{
  return true;
}


//: Finish
bool
dbirl_h_computation_process::finish()
{
  if ( input_data_.size() < 2 ){
    vcl_cerr << "In dbirl_h_computation_process::finish() - "
             << "need at least two input frames \n";
    return false;
  }
  // Set up the data
  vcl_cerr << "\ndbirl_h_computation_process: retrieving data\n";
  dbinfo_track_storage_sptr track_storage = new dbinfo_track_storage();
  //track_storage.vertical_cast( input_data_[0][0] );
  bpro1_filepath input_filename;
  parameters()->get_value( "-fin", input_filename );
  vcl_string ifname=input_filename.path;
  vsl_b_ifstream is( ifname );
  vcl_cerr << ifname;
  track_storage->b_read( is );
  vcl_vector< dbinfo_track_sptr > track_list = track_storage->tracks();
  vcl_vector< dbinfo_track_geometry_sptr > tracks;
  for( int i = 0; i < static_cast<int>(track_list.size()); i++ )
    tracks.push_back( track_list[i]->track_geometry() ); // THIS COULD BE WRONG.
  int target_frame=0;
  parameters()->get_value( "-tf" , target_frame );
  vcl_vector< vgl_h_matrix_2d<double> > h;

  // Compute the homographies
  vcl_cerr << "dbirl_h_computation_process: computing homographies\n";
  dbirl_h_computation hc;
  if( !hc.compute_affine( tracks, h, target_frame ) ){
    vcl_cerr << " error has occured, aborting\n";
    return false;
  }

  // Store the data into a file.
  vcl_cerr << "dbirl_h_computation_process: storing data to file\n";
  bpro1_filepath output_filename;
  parameters()->get_value( "-fout", output_filename );
  vcl_string ofname = output_filename.path;
  vcl_ofstream ofp(ofname.c_str(),vcl_ios::out|vcl_ios::app);
  if(!ofp)
    {
      vcl_cout << "\n Could not open file " << ofname;
      return false;
    }
  for( int i = 0; i < static_cast<int>(h.size()); i++ ){
    vcl_cerr << h[i] << '\n';
    ofp << "Frame No " << i << "\n" << h[i].get_matrix();
  }
  ofp.close();

  return true;
}

// This is brcv/trk/dbmrf/pro/dbmrf_backproject_process.cxx

//:
// \file

#include "dbmrf_backproject_process.h"

#include "dbmrf_bmrf_storage.h"


#include <vcl_limits.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_inverse.h>


#include <bmrf/bmrf_network.h>
#include <bmrf/bmrf_epi_transform.h>
#include <bmrf/bmrf_epi_seg.h>
#include <bmrf/bmrf_epi_point.h>


//: Constructor
dbmrf_backproject_process::dbmrf_backproject_process()
{
  if( !parameters()->add( "scale" ,      "-scale" ,       2.1464f ) ||
      !parameters()->add( "min gamma" ,  "-min_gamma" ,   0.02f   ) ||
      !parameters()->add( "min length" , "-min_length" ,  5       ) ||
      !parameters()->add( "Camera" ,     "-camera" ,      bpro1_filepath("","*")  ) ||
      !parameters()->add( "MAT file" ,   "-mat_file" ,    bpro1_filepath("","*")  ) ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  } 
}


dbmrf_backproject_process::
dbmrf_backproject_process(const dbmrf_backproject_process& other)
{
}


//: Destructor
dbmrf_backproject_process::~dbmrf_backproject_process()
{
}


//: Clone the process
bpro1_process* 
dbmrf_backproject_process::clone() const
{
  return new dbmrf_backproject_process(*this);
}


//: Return the name of the process
vcl_string
dbmrf_backproject_process::name()
{
  return "BMRF Back Project";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbmrf_backproject_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "bmrf" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbmrf_backproject_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}


//: Returns the number of input frames to this process
int
dbmrf_backproject_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbmrf_backproject_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbmrf_backproject_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << __FILE__ << " - not exactly one input frame" << vcl_endl;
    return false;
  }


  float scale, min_gamma;
  int min_length;
  bpro1_filepath camera_file, mat_file;
  parameters()->get_value( "-scale" ,        scale );
  parameters()->get_value( "-min_gamma" ,    min_gamma );
  parameters()->get_value( "-min_length" ,   min_length );
  parameters()->get_value( "-camera",        camera_file);
  parameters()->get_value( "-mat_file",      mat_file);

  // extract the network
  dbmrf_bmrf_storage_sptr frame_network;
  frame_network.vertical_cast(input_data_[0][0]);
  bmrf_network_sptr network = frame_network->network();

  int frame = frame_network->frame();

  vnl_double_3x4 camera;
  vcl_fstream fh(camera_file.path.c_str());
  fh >> camera;
  fh.close();

  vnl_double_3x3 M(camera.extract(3,3));
  vnl_double_3x3 Minv = vnl_inverse(M);
  vnl_double_3 t(camera.get_column(3));

  vcl_ofstream mat_out(mat_file.path.c_str(), vcl_fstream::app);

  unsigned long count = 0;
  for(bmrf_network::seg_node_map::const_iterator n = network->begin(frame);
      n != network->end(frame); ++n){
    bmrf_node_sptr node = n->second;
    bmrf_gamma_func_sptr gamma_func = node->gamma();
    if(!gamma_func){
      continue;
    }

    if(n->first->n_pts() <= min_length)
      continue;
    if( gamma_func->mean() < min_gamma )
      continue;

    ++count;
    mat_out << "curve{"<<frame<<","<<count<<"} = [";
    for(vcl_vector<bmrf_epi_point_sptr>::const_iterator pi = n->first->begin();
        pi != n->first->end(); ++pi)
    {
      vgl_point_2d<double> gpt((*pi)->p());
      double dc = scale / (*gamma_func)((*pi)->alpha());

      if(vnl_math_isfinite(dc)){
        vnl_double_3 pt(gpt.x()*dc, gpt.y()*dc, dc);
        pt -= t;
        pt = Minv*pt;
        mat_out << pt[0]<<" "<<pt[1]<<" "<<pt[2]<<"; ";
      }

    }
    mat_out << "];"<< vcl_endl;
  }

  mat_out.close();

  return true;
}


//: Finish
bool
dbmrf_backproject_process::finish()
{
  return true;
}





// This is brcv/trk/dbmrf/pro/dbmrf_predict_process.cxx

//:
// \file

#include "dbmrf_predict_process.h"

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include "dbmrf_bmrf_storage.h"

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vnl/vnl_math.h>


#include <bmrf/bmrf_network.h>
#include <bmrf/bmrf_epi_transform.h>
#include <bmrf/bmrf_epi_seg.h>
#include <bmrf/bmrf_epi_point.h>


//: Constructor
dbmrf_predict_process::dbmrf_predict_process()
{
  if( !parameters()->add( "Time" ,      "-time" ,       1.0f ) ||
      !parameters()->add( "Min gamma" , "-min_gamma" ,  0.02f ) ||
      !parameters()->add( "Max gamma" , "-max_gamma" ,  0.05f ) ||
      !parameters()->add( "Min size" ,  "-min_size"  ,  5 )  ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  } 
}


dbmrf_predict_process::
dbmrf_predict_process(const dbmrf_predict_process& other)
{
}


//: Destructor
dbmrf_predict_process::~dbmrf_predict_process()
{
}


//: Clone the process
bpro1_process* 
dbmrf_predict_process::clone() const
{
  return new dbmrf_predict_process(*this);
}


//: Return the name of the process
vcl_string
dbmrf_predict_process::name()
{
  return "BMRF Predict";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbmrf_predict_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "bmrf" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbmrf_predict_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol" );
  return to_return;
}


//: Returns the number of input frames to this process
int
dbmrf_predict_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbmrf_predict_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbmrf_predict_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << __FILE__ << " - not exactly one input frame" << vcl_endl;
    return false;
  }


  float time, min_gamma, max_gamma;
  int min_size;
  parameters()->get_value( "-time" ,      time );
  parameters()->get_value( "-min_gamma" , min_gamma );
  parameters()->get_value( "-max_gamma" , max_gamma );
  parameters()->get_value( "-min_size" ,  min_size );

  // extract the network
  dbmrf_bmrf_storage_sptr frame_network;
  frame_network.vertical_cast(input_data_[0][0]);
  bmrf_network_sptr network = frame_network->network();

  int frame = frame_network->frame();

  vcl_vector< vsol_spatial_object_2d_sptr > curves;
  for(bmrf_network::seg_node_map::const_iterator n = network->begin(frame);
      n != network->end(frame); ++n){
    bmrf_node_sptr node = n->second;
    bmrf_gamma_func_sptr gamma_func = node->gamma();
    if(!gamma_func){
      vcl_cout << "skipped\n" <<vcl_endl;
      continue;
    }
    double g = gamma_func->mean();
    if(vnl_math_isnan(g) || g < min_gamma || g > max_gamma)
      continue;
    if(n->first->n_pts() <= min_size)
      continue;

    bmrf_epi_seg_sptr t_seg = bmrf_epi_transform(n->first,gamma_func,time,true);

    vcl_vector< vsol_point_2d_sptr > points;
    for(vcl_vector<bmrf_epi_point_sptr>::const_iterator pi = t_seg->begin();
        pi != t_seg->end(); ++pi)
    {
      points.push_back(new vsol_point_2d((*pi)->p()));
    }
    curves.push_back(new vsol_digital_curve_2d(points));
  }

  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(curves, "curves");
  output_data_[0].push_back(output_vsol);

  return true;
}


//: Finish
bool
dbmrf_predict_process::finish()
{
  return true;
}





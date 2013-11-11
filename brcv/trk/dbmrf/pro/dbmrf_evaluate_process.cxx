// This is brcv/trk/dbmrf/pro/dbmrf_evaluate_process.cxx

//:
// \file

#include "dbmrf_evaluate_process.h"

#include <vidpro1/storage/vidpro1_image_storage.h>
#include "dbmrf_bmrf_storage.h"


#include <vcl_limits.h>
#include <vnl/vnl_math.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_bilin_interp.h>


#include <bmrf/bmrf_network.h>
#include <bmrf/bmrf_epi_transform.h>
#include <bmrf/bmrf_epi_seg.h>
#include <bmrf/bmrf_epi_point.h>


//: Constructor
dbmrf_evaluate_process::dbmrf_evaluate_process()
{
  if( !parameters()->add( "scale" ,     "-scale" ,    1.0f ) ||
      !parameters()->add( "compare depths" ,    "-depth" ,    true ) ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  } 
}


dbmrf_evaluate_process::
dbmrf_evaluate_process(const dbmrf_evaluate_process& other)
{
}


//: Destructor
dbmrf_evaluate_process::~dbmrf_evaluate_process()
{
}


//: Clone the process
bpro1_process* 
dbmrf_evaluate_process::clone() const
{
  return new dbmrf_evaluate_process(*this);
}


//: Return the name of the process
vcl_string
dbmrf_evaluate_process::name()
{
  return "BMRF Evaluate";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbmrf_evaluate_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "bmrf" );
  to_return.push_back( "image" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbmrf_evaluate_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  //to_return.push_back( "vsol" );
  return to_return;
}


//: Returns the number of input frames to this process
int
dbmrf_evaluate_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbmrf_evaluate_process::output_frames()
{
  return 1;
}

namespace {

inline double safe_interp(vil_image_view< double >& image, double x, double y)
{
  int x1 = static_cast<int>(vcl_floor(x)), x2 = x1+1;
  int y1 = static_cast<int>(vcl_floor(y)), y2 = y1+1;
  if(x1<0 || x2 >= image.ni() || y1<0 || y2 >= image.nj())
    return  vcl_numeric_limits<double>::infinity();

  double v11 = image(x1,y1), v21 = image(x2,y1),
         v12 = image(x1,y2), v22 = image(x2,y2);
  if(vnl_math_isfinite(v11) && vnl_math_isfinite(v12) &&
     vnl_math_isfinite(v21) && vnl_math_isfinite(v22) ){
    double nx = x - x1;
    double ny = y - y1;
    return nx*(ny*v22 + (1-ny)*v21)
        + (1-nx)*(ny*v12 + (1-ny)*v11);
  }

  if(vnl_math_isfinite(v11)) return v11;
  if(vnl_math_isfinite(v12)) return v12;
  if(vnl_math_isfinite(v21)) return v21;
  if(vnl_math_isfinite(v22)) return v22;

  return vcl_numeric_limits<double>::infinity();
}

};


//: Run the process on the current frame
bool
dbmrf_evaluate_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << __FILE__ << " - not exactly one input frame" << vcl_endl;
    return false;
  }


  float scale;
  bool depth;
  parameters()->get_value( "-scale" ,    scale );
  parameters()->get_value( "-depth" ,    depth );

  // extract the network
  dbmrf_bmrf_storage_sptr frame_network;
  frame_network.vertical_cast(input_data_[0][0]);
  bmrf_network_sptr network = frame_network->network();

  int frame = frame_network->frame();

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][1]);

  vil_image_resource_sptr image_sptr = frame_image->get_image();
  if(!image_sptr) return false;
  vil_image_view< double > image = image_sptr->get_view();
  if(!image) return false;

  if(depth){
    double error = 0.0, error2 = 0.0;
    unsigned long count = 0;
    for(bmrf_network::seg_node_map::const_iterator n = network->begin(frame);
        n != network->end(frame); ++n){
      bmrf_node_sptr node = n->second;
      bmrf_gamma_func_sptr gamma_func = node->gamma();
      if(!gamma_func){
        //vcl_cout << "skipped\n" <<vcl_endl;
        continue;
      }

      if(n->first->n_pts() <= 5)
        continue;
      if( gamma_func->mean() < .02 )
        continue;

      for(vcl_vector<bmrf_epi_point_sptr>::const_iterator pi = n->first->begin();
          pi != n->first->end(); ++pi)
      {
        vgl_point_2d<double> pt((*pi)->p());
        double di = vil_bilin_interp(image,pt.x(),pt.y());
        double dc = scale / (*gamma_func)((*pi)->alpha());
        if(vnl_math_isfinite(di) && vnl_math_isfinite(dc)){
          //vcl_cout << di << " \t"<< dc<< " \t" << vcl_endl;
          error += vcl_abs(di-dc);
          error2 += (di-dc)*(di-dc);
          ++count;
        }
        
        if(vnl_math_isfinite(dc)){
          vcl_cout << pt.x()<<" "<<pt.y()<<" "<<dc<<vcl_endl;
        }
        
      }
    }
    //vcl_cout << "count: "<< count <<'\n'
    //         << "mean: " << error/count << '\n'
    //         << "mean2: "<< error2/count <<vcl_endl;
  }
  else{
    
  double last_error = 1.0;
  for(double s=2.13; s<2.16; s+=0.00001){
  
  double error = 0.0;
  unsigned long count = 0;
  for(bmrf_network::seg_node_map::const_iterator n = network->begin(frame);
      n != network->end(frame); ++n){
    bmrf_node_sptr node = n->second;
    bmrf_gamma_func_sptr gamma_func = node->gamma();
    if(!gamma_func){
      //vcl_cout << "skipped\n" <<vcl_endl;
      continue;
    } 

    for(vcl_vector<bmrf_epi_point_sptr>::const_iterator pi = n->first->begin();
        pi != n->first->end(); ++pi)
    {
      vgl_point_2d<double> pt((*pi)->p());
      double gi = vil_bilin_interp(image,pt.x(),pt.y())*s;
      double gc = (*gamma_func)((*pi)->alpha());
      error += (gi-gc)*(gi-gc);
      ++count;
    }
  }
  double rms = vcl_sqrt(error/count);

  if(rms < last_error){
    last_error = rms;
  }else{
    vcl_cout <<"min: "<< s<<" \t" << rms << vcl_endl;
    break;
  }
  //vcl_cout << s<<" \t" << rms << vcl_endl;
  }
  }
  
  return true;
}


//: Finish
bool
dbmrf_evaluate_process::finish()
{
  return true;
}





// This is brcv/trk/dbmrf/pro/dbmrf_evaluate_process.cxx

//:
// \file

#include "dbmrf_evaluate_process.h"

#include <vidpro1/storage/vidpro1_image_storage.h>
#include "dbmrf_bmrf_storage.h"


#include <limits>
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
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
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
std::string
dbmrf_evaluate_process::name()
{
  return "BMRF Evaluate";
}


//: Returns a vector of strings describing the input types to this process
std::vector< std::string > dbmrf_evaluate_process::get_input_type()
{
  std::vector< std::string > to_return;
  to_return.push_back( "bmrf" );
  to_return.push_back( "image" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
std::vector< std::string > dbmrf_evaluate_process::get_output_type()
{
  std::vector< std::string > to_return;
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
  int x1 = static_cast<int>(std::floor(x)), x2 = x1+1;
  int y1 = static_cast<int>(std::floor(y)), y2 = y1+1;
  if(x1<0 || x2 >= image.ni() || y1<0 || y2 >= image.nj())
    return  std::numeric_limits<double>::infinity();

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

  return std::numeric_limits<double>::infinity();
}

};


//: Run the process on the current frame
bool
dbmrf_evaluate_process::execute()
{
  if ( input_data_.size() != 1 ){
    std::cerr << __FILE__ << " - not exactly one input frame" << std::endl;
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
        //std::cout << "skipped\n" <<std::endl;
        continue;
      }

      if(n->first->n_pts() <= 5)
        continue;
      if( gamma_func->mean() < .02 )
        continue;

      for(std::vector<bmrf_epi_point_sptr>::const_iterator pi = n->first->begin();
          pi != n->first->end(); ++pi)
      {
        vgl_point_2d<double> pt((*pi)->p());
        double di = vil_bilin_interp(image,pt.x(),pt.y());
        double dc = scale / (*gamma_func)((*pi)->alpha());
        if(vnl_math_isfinite(di) && vnl_math_isfinite(dc)){
          //std::cout << di << " \t"<< dc<< " \t" << std::endl;
          error += std::abs(di-dc);
          error2 += (di-dc)*(di-dc);
          ++count;
        }
        
        if(vnl_math_isfinite(dc)){
          std::cout << pt.x()<<" "<<pt.y()<<" "<<dc<<std::endl;
        }
        
      }
    }
    //std::cout << "count: "<< count <<'\n'
    //         << "mean: " << error/count << '\n'
    //         << "mean2: "<< error2/count <<std::endl;
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
      //std::cout << "skipped\n" <<std::endl;
      continue;
    } 

    for(std::vector<bmrf_epi_point_sptr>::const_iterator pi = n->first->begin();
        pi != n->first->end(); ++pi)
    {
      vgl_point_2d<double> pt((*pi)->p());
      double gi = vil_bilin_interp(image,pt.x(),pt.y())*s;
      double gc = (*gamma_func)((*pi)->alpha());
      error += (gi-gc)*(gi-gc);
      ++count;
    }
  }
  double rms = std::sqrt(error/count);

  if(rms < last_error){
    last_error = rms;
  }else{
    std::cout <<"min: "<< s<<" \t" << rms << std::endl;
    break;
  }
  //std::cout << s<<" \t" << rms << std::endl;
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





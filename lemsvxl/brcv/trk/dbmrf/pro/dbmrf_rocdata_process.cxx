// This is brcv/trk/dbmrf/pro/dbmrf_rocdata_process.cxx

//:
// \file

#include "dbmrf_rocdata_process.h"

#include "dbmrf_bmrf_storage.h"
#include <vidpro1/storage/vidpro1_image_storage.h>


#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_greyscale_dilate.h>

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
dbmrf_rocdata_process::dbmrf_rocdata_process()
{
  if( !parameters()->add( "min gamma" ,  "-min_gamma" ,   0.02f   ) ||
      !parameters()->add( "min length" , "-min_length" ,  5       ) ||
      !parameters()->add( "output file" ,   "-mat_file" ,    bpro1_filepath("","*")  ) ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  } 
}


dbmrf_rocdata_process::
dbmrf_rocdata_process(const dbmrf_rocdata_process& other)
{
}


//: Destructor
dbmrf_rocdata_process::~dbmrf_rocdata_process()
{
}


//: Clone the process
bpro1_process* 
dbmrf_rocdata_process::clone() const
{
  return new dbmrf_rocdata_process(*this);
}


//: Return the name of the process
vcl_string
dbmrf_rocdata_process::name()
{
  return "Export ROC Data";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbmrf_rocdata_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "bmrf" );
  to_return.push_back( "image" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbmrf_rocdata_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}


//: Returns the number of input frames to this process
int
dbmrf_rocdata_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbmrf_rocdata_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbmrf_rocdata_process::execute()
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

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][1]);

  vil_image_resource_sptr image_sptr = frame_image->get_image();
  if(!image_sptr) return false;
  vil_image_view< double > image = image_sptr->get_view();
  if(!image) return false;

  vil_structuring_element disk; disk.set_to_disk(2);
  vil_image_view< double > dimage;
  vil_greyscale_dilate(image,dimage,disk);


  vcl_ofstream mat_out(mat_file.path.c_str(), vcl_fstream::app);

  unsigned long count = 0;
  for(bmrf_network::seg_node_map::const_iterator n = network->begin(frame);
      n != network->end(frame); ++n){
    bmrf_node_sptr node = n->second;
    bmrf_gamma_func_sptr gamma_func = node->gamma();
    if(!gamma_func){
      gamma_func = new bmrf_const_gamma_func(0.0);
    }

    ++count;
    unsigned int ni = image.ni(), nj = image.nj();
    mat_out << "curve{"<<frame<<","<<count<<"} = [";
    for(vcl_vector<bmrf_epi_point_sptr>::const_iterator pi = n->first->begin();
        pi != n->first->end(); ++pi)
    {
      vgl_point_2d<double> gpt((*pi)->p());
      int x1 = int(vcl_floor(gpt.x())),  x2 = int(vcl_ceil(gpt.x()));
      int y1 = int(vcl_floor(gpt.y())),  y2 = int(vcl_ceil(gpt.y()));
      bool fg = false;
      if(x2 < ni && x1 >= 0 && y2 < nj && y1 >= 0){
        fg = (dimage(x1,y1)!=0) || (dimage(x2,y1)!=0) || (dimage(x1,y2)!=0)|| (dimage(x2,y2)!=0);
      }
      mat_out << gpt.x()<<" "<<gpt.y()<<" "<<(*gamma_func)((*pi)->alpha())<<" "<<(fg?1:0)<<"; ";
    }
    mat_out << "];"<< vcl_endl;
  }

  mat_out.close();

  return true;
}


//: Finish
bool
dbmrf_rocdata_process::finish()
{
  return true;
}





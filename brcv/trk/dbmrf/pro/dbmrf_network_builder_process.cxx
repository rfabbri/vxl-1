// This is brcv/trk/dbmrf/pro/dbmrf_network_builder_process.cxx

//:
// \file

#include "dbmrf_network_builder_process.h"

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vtol_storage.h>
#include "dbmrf_bmrf_storage.h"

#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>

#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d.h>

#include <bmrf/bmrf_network_builder.h>
#include <bmrf/bmrf_network.h>

#include <vul/vul_timer.h>

//: Constructor
dbmrf_network_builder_process::dbmrf_network_builder_process()
 : bpro1_process(), net_params_(), builder_(NULL)
{
  if( !parameters()->add( "Col position of epipole" ,
                          "-nbeu" ,          0.0f ) ||
      !parameters()->add( "Row position of epipole" ,
                          "-nbev" ,          0.0f ) ||
      !parameters()->add( "Col position of epipolar parameter space" ,
                          "-nbelu" ,         (int)net_params_.elu_ ) ||
      !parameters()->add( "Min row position of epipolar space" ,
                          "-nbelv_min" ,     (int)net_params_.elv_min_ ) ||
      !parameters()->add( "Max row position of epipolar space" ,
                          "-nbelv_max" ,     (int)net_params_.elv_max_ ) ||
      !parameters()->add( "Number of intensity samples in s" ,
                          "-nbns" ,          (int)net_params_.Ns_ ) ||
      !parameters()->add( "Maximum difference of reciprocals of s for time neighbors" ,
                          "-nbdrs_min" ,     (float)net_params_.max_delta_recip_s_ ) ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  } 
}


dbmrf_network_builder_process::
dbmrf_network_builder_process(const dbmrf_network_builder_process& other)
: bpro1_process(other), net_params_(other.net_params_), builder_(NULL)
{
}


//: Destructor
dbmrf_network_builder_process::~dbmrf_network_builder_process()
{
  if ( builder_ )
    delete builder_;
}


//: Clone the process
bpro1_process* 
dbmrf_network_builder_process::clone() const
{
  return new dbmrf_network_builder_process(*this);
}


//: Return the name of the process
vcl_string
dbmrf_network_builder_process::name()
{
  return "Build MRF Network";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbmrf_network_builder_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "vtol" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbmrf_network_builder_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "bmrf" );
  return to_return;
}


//: Returns the number of input frames to this process
int
dbmrf_network_builder_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbmrf_network_builder_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbmrf_network_builder_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << __FILE__ << " - not exactly one input frame" << vcl_endl;
    return false;
  }

  if (!builder_){

    parameters()->get_value( "-nbeu" ,          net_params_.eu_ );
    parameters()->get_value( "-nbev" ,          net_params_.ev_ );
    parameters()->get_value( "-nbelu" ,         net_params_.elu_ );
    parameters()->get_value( "-nbelv_min" ,     net_params_.elv_min_ );
    parameters()->get_value( "-nbelv_max" ,     net_params_.elv_max_ );
    parameters()->get_value( "-nbns" ,          net_params_.Ns_ );
    parameters()->get_value( "-nbdrs_min" ,     net_params_.max_delta_recip_s_ );

    builder_ = new bmrf_network_builder( net_params_ );
    builder_->init();   
  }

  // get image from the storage class

  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  vil_image_resource_sptr image_sptr = frame_image->get_image();
  vil_image_view< vxl_byte > image_view = vil_convert_to_grey_using_rgb_weighting(image_sptr->get_view());
  vil_image_view< float > greyscale_view;

  vil_convert_cast( image_view, greyscale_view );
  vil_math_scale_values( greyscale_view, (1.0/255.0) );

  int frame = frame_image->frame();

  // Give the image to the network builder
  builder_->set_image(greyscale_view);

  vidpro1_vtol_storage_sptr frame_vtol;
  frame_vtol.vertical_cast(input_data_[0][1]);

  vcl_vector<vtol_edge_2d_sptr> edges;
  for ( vcl_set<vtol_topology_object_sptr>::const_iterator itr = frame_vtol->begin();
        itr != frame_vtol->end();  ++itr ) {
    vtol_edge *edge = (*itr)->cast_to_edge();
    if (edge){
      vtol_edge_2d *edge_2d = edge->cast_to_edge_2d();
      if (edge_2d)
        edges.push_back(vtol_edge_2d_sptr(edge_2d));
    }
  }
  vcl_cout << "num edges = "<< edges.size() << vcl_endl;
  builder_->set_edges(frame, edges);

  vul_timer time;
  // Build the network
  builder_->build();
  vcl_cout << "Done Building network in "<< time.all()<< "msec" << vcl_endl;

  vcl_cout << "network size = "<< builder_->network()->size() << vcl_endl;

  // create the output storage class
  dbmrf_bmrf_storage_sptr output_bmrf = dbmrf_bmrf_storage_new(builder_->network());
  output_data_[0].push_back(output_bmrf);

  return true;
}


//: Finish
bool
dbmrf_network_builder_process::finish()
{
  return true;
}





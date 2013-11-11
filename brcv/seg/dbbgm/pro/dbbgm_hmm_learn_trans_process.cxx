// This is brcv/seg/dbbgm/pro/dbbgm_hmm_learn_trans_process.cxx

//:
// \file

#include "dbbgm_hmm_learn_trans_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_plane.h>
#include <vil/vil_new.h>



//: Constructor
dbbgm_hmm_learn_trans_process::dbbgm_hmm_learn_trans_process()
  : num_obs_(NULL), label_image_last_(NULL)
{
  if( !parameters()->add( "image width" ,     "-width" ,   (unsigned int)352 )  ||
      !parameters()->add( "image height" ,    "-height" ,  (unsigned int)240 )){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
dbbgm_hmm_learn_trans_process::~dbbgm_hmm_learn_trans_process()
{
}


//: Clone the process
bpro1_process* 
dbbgm_hmm_learn_trans_process::clone() const
{
  return new dbbgm_hmm_learn_trans_process(*this);
}


//: Return the name of this process
vcl_string
dbbgm_hmm_learn_trans_process::name()
{
  return "HMM Learn Transitions";
}


//: Return the number of input frame for this process
int
dbbgm_hmm_learn_trans_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbbgm_hmm_learn_trans_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbbgm_hmm_learn_trans_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbbgm_hmm_learn_trans_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}


//: Execute the process
bool
dbbgm_hmm_learn_trans_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbbgm_hmm_learn_trans_process::execute() - "
             << "not exactly one input frame \n";
    return false;
  }
  clear_output();

  unsigned int width=0,height=0;
  parameters()->get_value( "-width" ,  width );
  parameters()->get_value( "-height" , height );

  // get label image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);
  vil_image_view<vxl_byte> label_image;
  if(!frame_image){
    label_image = vil_image_view<vxl_byte>(width,height);
    label_image.fill(0);
  }
  else
    label_image = frame_image->get_image()->get_view();


  // get the transition probabilities image
  vil_image_view<float> img_transitions;
  vidpro1_image_storage_sptr frame_transimg;
  frame_transimg.vertical_cast(input_data_[0][1]);
  if(!frame_transimg){
    img_transitions = vil_image_view<float>(label_image.ni(),label_image.nj(),9);
    img_transitions.fill(1.0/3.0);
    frame_transimg = vidpro1_image_storage_new();
    frame_transimg->mark_global();
    frame_transimg->set_image(vil_new_image_resource_of_view(img_transitions));
    input_data_[0][1] = frame_transimg;
  }
  else{
    img_transitions = vil_convert_cast(float(), frame_transimg->get_image()->get_view());
    // This marks the data as modified so the GUI will redraw
    output_data_[0].push_back(frame_transimg);
  }

  if(label_image_last_ && num_obs_)
  {
    double alpha;
    for(unsigned int j=0; j<label_image.nj(); ++j){
      for(unsigned int i=0; i<label_image.ni(); ++i){
        const unsigned int pl = label_image_last_(i,j);
        alpha = 1.0/(++num_obs_(i,j,pl));
        for(unsigned int idx=pl*3; idx < (pl+1)*3; ++idx){
          img_transitions(i,j,idx) *= (1-alpha);
        }
        unsigned int idx = (unsigned int)(pl*3 + label_image(i,j));
        img_transitions(i,j,idx) += alpha;
      }
    }
  }
  else{
    num_obs_ = vil_image_view<unsigned int>(label_image.ni(),label_image.nj(),3);
    num_obs_.fill(0);
  }

  label_image_last_ = label_image;

  return true;
}


bool
dbbgm_hmm_learn_trans_process::finish()
{
  num_obs_ = NULL;
  label_image_last_ = NULL;
  return true;
}

// This is brcv/seg/dbbgm/pro/dbbgm_hmm_detect_process.cxx

//:
// \file

#include "dbbgm_hmm_detect_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include "dbbgm_image_storage.h"
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_image_of.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/algo/bsta_truth_updater.h>
#include <bsta/algo/bsta_gaussian_updater.h>
#include <bsta/algo/bsta_gaussian_stats.h>
#include <bsta/algo/bsta_bayes_functor.h>
#include <bsta/algo/bsta_mixture_functors.h>
#include <bsta/bsta_basic_functors.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <bbgm/bbgm_apply.h>
#include <dbbgm/algo/dbbgm_hmm_algo.h>
#include <vul/vul_timer.h>



//: Constructor
dbbgm_hmm_detect_process::dbbgm_hmm_detect_process()
 : prob_state_(NULL)
{
  if( !parameters()->add( "Weight Theshold" ,      "-wt" ,        0.7f        ) ||
      !parameters()->add( "Foreground Prob" ,      "-fgprob" ,    1.0f        ) ||
      !parameters()->add( "Field Weight" ,         "-fieldw" ,    9.0f        ) ||
      !parameters()->add( "Single Mixture" ,       "-singmix" ,   false       ) ||
      !parameters()->add( "Debug"          ,       "-debug" ,     false       ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
dbbgm_hmm_detect_process::~dbbgm_hmm_detect_process()
{
}


//: Clone the process
bpro1_process* 
dbbgm_hmm_detect_process::clone() const
{
  return new dbbgm_hmm_detect_process(*this);
}


//: Return the name of this process
vcl_string
dbbgm_hmm_detect_process::name()
{
  return "HMM Detect";
}


//: Return the number of input frame for this process
int
dbbgm_hmm_detect_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbbgm_hmm_detect_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbbgm_hmm_detect_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  to_return.push_back( "dbbgm_image" );
  bool singmix = false;
  parameters()->get_value( "-singmix" ,  singmix);
  if(!singmix)
    to_return.push_back( "dbbgm_image" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbbgm_hmm_detect_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  bool debug = false;
  parameters()->get_value( "-debug" ,  debug);
  if(debug){
    to_return.push_back( "image" );
    to_return.push_back( "image" );
    to_return.push_back( "image" );
  }

  return to_return;
}


//: Execute the process
bool
dbbgm_hmm_detect_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbbgm_hmm_detect_process::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  clear_output();

  // get the process parameters 
  float wt=0, fgprob=0, fieldw=0;
  bool singmix=false, debug=false;
  parameters()->get_value( "-wt" ,       wt);
  parameters()->get_value( "-fgprob" ,   fgprob);
  parameters()->get_value( "-fieldw" ,   fieldw);
  parameters()->get_value( "-singmix" ,  singmix);
  parameters()->get_value( "-debug" ,    debug);

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  // convert the image to float
  vil_image_resource_sptr image_rsc = frame_image->get_image();
  vil_image_view<float> img = *vil_convert_cast(float(), image_rsc->get_view());
  if(image_rsc->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);

  unsigned int ni = img.ni();
  unsigned int nj = img.nj();
  unsigned int np = img.nplanes();

  vil_image_view<float> transitions;
  if(input_data_[0][1]){
    frame_image.vertical_cast(input_data_[0][1]);
    image_rsc = frame_image->get_image();
    transitions = image_rsc->get_view();
  }


  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_num_obs<bsta_mixture<gauss_type> > mix_gauss_type;

  bbgm_image_of<mix_gauss_type> *model, *model_shadow=NULL;

  // get the background model
  dbbgm_image_storage_sptr frame_model = NULL;
  if(input_data_[0][2]){ 
    frame_model.vertical_cast(input_data_[0][2]);
  }
  model = dynamic_cast<bbgm_image_of<mix_gauss_type>*>(frame_model->dist_image().ptr());
  if(!model)
    return false;

  if(!singmix){
    frame_model = NULL;
    if(input_data_[0][3]){
      frame_model.vertical_cast(input_data_[0][3]);
    }
    model_shadow =
        dynamic_cast<bbgm_image_of<mix_gauss_type>*>(frame_model->dist_image().ptr());
    if(!model_shadow)
      return false; 
  }


  vul_timer timer;

  // Detect the probabilities
  vil_image_view<float> probs(ni,nj,3);
  //float uniform_val = 1.0/256;//1.0/(256*256*256);
  vil_image_view<float> p0(vil_plane(probs,0));
  vil_image_view<float> p1(vil_plane(probs,1));
  vil_plane(probs,2).fill(fgprob);

  typedef bsta_prob_density_functor<gauss_type> prob_functor;
  prob_functor pf;
  if(!singmix){
    //typedef bsta_mixture_data_functor<mix_gauss_type,prob_functor> mix_prob_functor;
    typedef bsta_top_weight_functor<mix_gauss_type,prob_functor> top_prob_functor;
    top_prob_functor probs_func(pf, wt);
    float fail_val = 0.0f;
    bbgm_apply(*model,probs_func,img,p0,&fail_val);
    bbgm_apply(*model_shadow,probs_func,img,p1,&fail_val);

    //typedef bsta_weighted_sum_data_functor<mix_gauss_type,prob_functor> wprob_functor;
    //dbbgm_apply(*model,wprob_functor(),img,p0);
    //dbbgm_apply(*model_shadow,wprob_functor(),img,p1,&uniform_val);
  }
  else{
    typedef bsta_mixture_data_functor<mix_gauss_type,prob_functor> mix_prob_functor;
    mix_prob_functor probs_func0(pf,0);
    bbgm_apply(*model,probs_func0,img,p0);
    mix_prob_functor probs_func1(pf,1);
    bbgm_apply(*model,probs_func1,img,p1);
  }

  //dbbgm_normalize_probs(probs);

  //bsta_mixture_prob_functor<bsta_gauss_if3> mix_prob(true,np);
  //vil_image_view<float> probs;
  //dbbgm_apply(*model,mix_prob,img,probs);

  vil_image_view<float> new_probs(ni,nj,3);
  double time1 = timer.user()/1000.0;
  if(!prob_state_ || !transitions){
    prob_state_ = probs;
  }
  else{
    dbbgm_update_probs(prob_state_,transitions,new_probs);
    vil_math_image_product(new_probs,probs,prob_state_);
  }
  dbbgm_normalize_probs(prob_state_);

  if(fieldw > 0)
    dbbgm_mean_field(prob_state_,4,fieldw);



  vil_image_view<vxl_byte> result(ni, nj);
  result.fill(0);

  double time2 = timer.user()/1000.0;

  for(unsigned int j=0; j<nj; ++j){
    for(unsigned int i=0; i<ni; ++i){
      int best_ind = -1;
      double best_prob = 0.0;
      double sum = 0.0;
      for(unsigned int m=0; m<np; ++m){
        sum += prob_state_(i,j,m);
        if(prob_state_(i,j,m) > best_prob){
          best_prob = prob_state_(i,j,m);
          best_ind = m;
        }
      }
      if(best_ind >=0 && best_ind<(int)np)
        result(i,j) = best_ind;
    }
  }

  double time3 = timer.user()/1000.0;
  if(debug)
    vcl_cout << "detect in "<<time1<<" ms  and " << time2<<" ms  and " << time3<<" ms"<<vcl_endl;


  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view( result ));
  output_data_[0].push_back(output_storage);

  if(debug){
    output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view( prob_state_ ));
    output_data_[0].push_back(output_storage);

    dbbgm_normalize_probs(new_probs);
    output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view( new_probs ));
    output_data_[0].push_back(output_storage);

    dbbgm_normalize_probs(probs);
    output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view( probs ));
    output_data_[0].push_back(output_storage);
  }

  return true;
}



bool
dbbgm_hmm_detect_process::finish()
{
  prob_state_=NULL;
  return true;
}





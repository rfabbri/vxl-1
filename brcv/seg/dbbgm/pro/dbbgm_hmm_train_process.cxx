// This is brcv/seg/dbbgm/pro/dbbgm_hmm_train_process.cxx

//:
// \file

#include "dbbgm_hmm_train_process.h"
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

#include <bbgm/bbgm_update.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_plane.h>
#include <vil/vil_new.h>



//: Constructor
dbbgm_hmm_train_process::dbbgm_hmm_train_process()
  : label_image_last_(NULL)
{
  if( !parameters()->add( "Use Epipolar Map" ,     "-epimap" ,  false     )  ||
      !parameters()->add( "Num Epipolar Bins" ,    "-num_epi_bins" ,  (unsigned int)256 )){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
dbbgm_hmm_train_process::~dbbgm_hmm_train_process()
{
}


//: Clone the process
bpro1_process* 
dbbgm_hmm_train_process::clone() const
{
  return new dbbgm_hmm_train_process(*this);
}


//: Return the name of this process
vcl_string
dbbgm_hmm_train_process::name()
{
  return "HMM Train Model";
}


//: Return the number of input frame for this process
int
dbbgm_hmm_train_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbbgm_hmm_train_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbbgm_hmm_train_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  to_return.push_back( "dbbgm_image" );
  to_return.push_back( "image" ); 
  to_return.push_back( "image" ); 
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbbgm_hmm_train_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}


//: Execute the process
bool
dbbgm_hmm_train_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbbgm_hmm_train_process::execute() - "
             << "not exactly one input image \n";
    return false;
  }
  clear_output();

  // get the process parameters
  bool epimap = false;
  parameters()->get_value( "-epimap" ,  epimap );

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  // convert the image to float
  vil_image_resource_sptr image_rsc = frame_image->get_image();
  vil_image_view<float> img = *vil_convert_cast(float(), image_rsc->get_view());
  if(image_rsc->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);
  vil_image_view<float> img_truth(img.ni(),img.nj(),4);
  vil_image_view<float> img_out = vil_planes(img_truth,0,1,3);
  img_out.deep_copy(img);

  // get label image from the storage class
  vidpro1_image_storage_sptr frame_image2;
  frame_image2.vertical_cast(input_data_[0][1]);

  vil_image_view<vxl_byte> label_image = frame_image2->get_image()->get_view();
  assert(label_image.ni() == img.ni());
  assert(label_image.nj() == img.nj());

  // copy the labels into the last plane of the "truth" image
  vil_image_view<float> idx_image = vil_plane(img_truth,3);
  vil_convert_cast(label_image, idx_image);

  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_num_obs<bsta_mixture<gauss_type> > mix_gauss_type;

  bsta_gauss_if3 init_gauss;
  //bsta_gauss_if3 init_gauss(vnl_vector_fixed<float,3>(0.5f),vnl_vector_fixed<float,3>(0.1f));
  // give initial distributions a small fraction of an observation to prevent
  // the variance from going to zero when only one observation is made.
  //gauss_type init_gauss(bsta_gauss_if3(),0.01);
  bsta_truth_updater<gauss_type, bsta_gaussian_updater<bsta_gauss_if3> >
      updater(init_gauss, bsta_gaussian_updater<bsta_gauss_if3>());

  // get the templated mixture model
  bbgm_image_sptr model_sptr;
  dbbgm_image_storage_sptr frame_distimg;
  frame_distimg.vertical_cast(input_data_[0][2]);
  if(!frame_distimg){
    mix_gauss_type mixture;
    model_sptr = new bbgm_image_of<mix_gauss_type>(img.ni(),img.nj(),mixture);
    frame_distimg = dbbgm_image_storage_new(model_sptr);
    frame_distimg->mark_global();
    input_data_[0][2] = frame_distimg;
  }
  else{
    model_sptr = frame_distimg->dist_image();
  }

  if(epimap)
    update_epimap_transitions(label_image);
  else
    update_transitions(label_image);

  bbgm_image_of<mix_gauss_type> *model = static_cast<bbgm_image_of<mix_gauss_type>*>(model_sptr.ptr());
  update(*model,img_truth,updater);

  label_image_last_ = label_image;

  return true;
}


void
dbbgm_hmm_train_process::update_transitions(const vil_image_view<vxl_byte>& label_image)
{
  // get the transition probabilities image
  vil_image_view<float> img_transitions;
  vidpro1_image_storage_sptr frame_transimg;
  frame_transimg.vertical_cast(input_data_[0][3]);
  if(!frame_transimg){
    img_transitions = vil_image_view<float>(label_image.ni(),label_image.nj(),9);
    img_transitions.fill(0.0);
    frame_transimg = vidpro1_image_storage_new();
    frame_transimg->mark_global();
    frame_transimg->set_image(vil_new_image_resource_of_view(img_transitions));
    input_data_[0][3] = frame_transimg;
  }
  else{
    img_transitions = vil_convert_cast(float(), frame_transimg->get_image()->get_view());
    // This marks the data as modified so the GUI will redraw
    output_data_[0].push_back(frame_transimg);
  }

  if(label_image_last_)
  {
    for(unsigned int i=0; i<label_image.ni(); ++i){
      for(unsigned int j=0; j<label_image.nj(); ++j){
        unsigned int idx = (unsigned int)(label_image_last_(i,j)*3 + label_image(i,j));
        img_transitions(i,j,idx) += 1.0f;
      }
    }
  }

}


void 
dbbgm_hmm_train_process::update_epimap_transitions(const vil_image_view<vxl_byte>& label_image)
{
  unsigned int num_epi_bins = 0;
  parameters()->get_value( "-num_epi_bins" ,  num_epi_bins );

  // get the transition probabilities image
  vil_image_view<unsigned int> epimap_img;
  vidpro1_image_storage_sptr frame_img;
  frame_img.vertical_cast(input_data_[0][3]);
  assert(frame_img);
  vil_image_view<float> temp;
  vil_image_view<double> tempd;
  temp = vil_convert_cast(float(), frame_img->get_image()->get_view());
  vil_convert_stretch_range(temp, tempd, 0.0,num_epi_bins-0.001);
  vil_convert_cast(tempd, epimap_img);

  // get the transition probabilities image
  vil_image_view<float> img_trans;
  frame_img.vertical_cast(input_data_[0][4]);
  if(!frame_img){
    img_trans = vil_image_view<float>(9,num_epi_bins);
    img_trans.fill(0.0);
    frame_img = vidpro1_image_storage_new();
    frame_img->mark_global();
    frame_img->set_image(vil_new_image_resource_of_view(img_trans));
    input_data_[0][4] = frame_img;
  }
  else{
    img_trans = vil_convert_cast(float(), frame_img->get_image()->get_view());
    // This marks the data as modified so the GUI will redraw
    output_data_[0].push_back(frame_img);
  }


  if(label_image_last_)
  {
    for(unsigned int i=0; i<label_image.ni(); ++i){
      for(unsigned int j=0; j<label_image.nj(); ++j){
        unsigned int idx = (unsigned int)(label_image_last_(i,j)*3 + label_image(i,j));
        img_trans(idx,epimap_img(i,j)) += 1.0;
      }
    }
  }

}


bool
dbbgm_hmm_train_process::finish()
{
  // find the first valid index
  unsigned int valid_ind = 0;
  for(; valid_ind<input_data_.size(); ++valid_ind){
    if(!input_data_[valid_ind].empty())
      break;
  }
  if(valid_ind >= input_data_.size()){
    vcl_cerr << "Error: couldn't find data in dbbgm_hmm_train_process::finish()"<<vcl_endl;
    return false;
  }

  // get the templated mixture model
  bbgm_image_sptr model_sptr;
  dbbgm_image_storage_sptr frame_distimg;
  frame_distimg.vertical_cast(input_data_[valid_ind][2]);
  if(frame_distimg){
    model_sptr = frame_distimg->dist_image();
    typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
    typedef bsta_num_obs<bsta_mixture<gauss_type> > mix_gauss_type;
    bbgm_image_of<mix_gauss_type> *model = static_cast<bbgm_image_of<mix_gauss_type>*>(model_sptr.ptr());

    bsta_mixture_weight_by_obs_updater<bsta_gauss_if3> reweight;
    update(*model,reweight);
  }

  bool epimap = false;
  parameters()->get_value( "-epimap" ,  epimap );
  if(epimap)
    normalize_epimap_transitions(valid_ind);
  else
    normalize_transitions(valid_ind);

  label_image_last_ = NULL;
  return true;
}


void 
dbbgm_hmm_train_process::normalize_transitions(unsigned int valid_ind)
{
  // get the transition probabilities image
  vidpro1_image_storage_sptr frame_transimg;
  frame_transimg.vertical_cast(input_data_[valid_ind][3]);
  if(frame_transimg){
    vil_image_view<float> img_transitions
      = vil_convert_cast(float(), frame_transimg->get_image()->get_view());
    // This marks the data as modified so the GUI will redraw
    output_data_[valid_ind].push_back(frame_transimg);

    for(unsigned int i=0; i<img_transitions.ni(); ++i){
      for(unsigned int j=0; j<img_transitions.nj(); ++j){
        for(unsigned int m=0; m<3; ++m){
          float sum = 0.0;
          for(unsigned int p=m*3; p<(m+1)*3; ++p)
            sum += img_transitions(i,j,p);
          for(unsigned int p=m*3; p<(m+1)*3; ++p)
            img_transitions(i,j,p) /= sum;
        }
      }
    }
  }

}

void
dbbgm_hmm_train_process::normalize_epimap_transitions(unsigned int valid_ind)
{
  // get the transition probabilities image
  vidpro1_image_storage_sptr frame_transimg;
  frame_transimg.vertical_cast(input_data_[valid_ind][4]);
  if(frame_transimg){
    vil_image_view<float> img_trans
      = vil_convert_cast(float(), frame_transimg->get_image()->get_view());
    // This marks the data as modified so the GUI will redraw
    output_data_[valid_ind].push_back(frame_transimg);

    for(unsigned int j=0; j<img_trans.nj(); ++j){
      for(unsigned int m=0; m<3; ++m){
        float sum = 0.0;
        for(unsigned int i=m*3; i<(m+1)*3; ++i)
          sum += img_trans(i,j);
        for(unsigned int i=m*3; i<(m+1)*3; ++i)
          img_trans(i,j) /= sum;
      }
    }
  }
}




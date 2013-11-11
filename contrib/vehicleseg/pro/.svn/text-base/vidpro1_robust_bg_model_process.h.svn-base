// This is pro/vidpro1_robust_bg_model_process.h
#ifndef vidpro1_robust_bg_model_process_h_
#define vidpro1_robust_bg_model_process_h_

//:
// \file
// \brief 
// \author Vishal Jain(vj@lems.brown.edu)
// \date 11/11/03
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vgel/vgel_kl_params.h>
#include <vgel/vgel_kl.h>
#include <vgel/vgel_multi_view_data_vertex_sptr.h>
#include <vgel/vgel_multi_view_data.h>
#include <vimt/vimt_transform_2d.h>
#include <dbbgm/dbbgm_bgmodel.h>
#include <dbbgm/dbbgm_bgmodel_sptr.h>
//: Derived video process class for computing frame difference
class vidpro1_robust_bg_model_process : public bpro1_process {

public:

  vidpro1_robust_bg_model_process();
  ~vidpro1_robust_bg_model_process();

  vcl_string name();

  //: Clone the process
  virtual bpro1_process* clone() const;

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

  dbbgm_bgmodel_sptr getmodel(){return bgM;}
  dbbgm_bgmodel_sptr bgM;
   
private:
    
    
    bool first_frame_;
    
    int frame_no;
    vil_image_view<float> float_curr_view;
    vil_image_view<float> float_last_view;


};

#endif

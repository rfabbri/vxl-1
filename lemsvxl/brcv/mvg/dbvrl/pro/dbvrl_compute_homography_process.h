// This is lemsvxlsrc/brcv/mvg/dbvrl/pro/dbvrl_compute_homography_process.h
#ifndef dbvrl_compute_homography_process_h_
#define dbvrl_compute_homography_process_h_

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
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vimt/vimt_transform_2d.h>
#include <vbl/vbl_bounding_box.h>
//: Derived video process class for computing frame difference
class dbvrl_compute_homography_process : public bpro1_process {

public:

  dbvrl_compute_homography_process();
  ~dbvrl_compute_homography_process();

  vcl_string name();

  //: Clone the process
  virtual bpro1_process* clone() const;

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

private:

  vimt_transform_2d 
    register_image_with_mask(vil_image_view<float> & curr_view,
                             vil_image_view<float> & last_view,
                             vil_image_view<float> & mask);
                         
 vimt_transform_2d 
   register_image_with_optical_flow(vil_image_view<float> & curr_view,
                                    vil_image_view<float> & last_view,
                                    vil_image_view<bool>   & fixed_mask);

    vimt_transform_2d total_xform;
    bool first_frame_;
    float lrange_;
    float hrange_;
    int no_of_bins;
    int first_frame_no;
    int last_frame_no;
    float eroderadius;
    float dilateradius;
    vbl_bounding_box<double,2> box_;
    unsigned ni_;
    unsigned nj_;


    float sigma_first_image_;
    vcl_string regimgname;

    //: stores the first image to enable registration w.r.t to the first image.
    vil_image_resource_sptr first_img_;
    int intregfirst;
    int count;

};

#endif

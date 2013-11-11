// This is brl/vidpro1/process/vidpro1_compute_homography_process.h
#ifndef vidpro1_compute_homography_process_h_
#define vidpro1_compute_homography_process_h_

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
#include <vbl/vbl_bounding_box.h>
//: Derived video process class for computing frame difference
class vidpro1_compute_homography_process : public bpro1_process {

public:

  vidpro1_compute_homography_process();
  ~vidpro1_compute_homography_process();

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
    vimt_transform_2d register_image_with_optical_flow(vil_image_view<float> float_curr_view,
                                                       vil_image_view<float> float_last_view,
                                                       vimt_transform_2d & init_xform);

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
    int ni_;
    int nj_;


    float sigma_first_image_;
    vcl_string regimgname;

    //: stores the first image to enable registration w.r.t to the first image.
    vil_image_resource_sptr first_img_;
    int intregfirst;
    int count;

};

#endif

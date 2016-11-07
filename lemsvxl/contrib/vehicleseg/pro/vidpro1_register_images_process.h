// This is brl/vidpro1/process/vidpro1_register_images_process.h
#ifndef vidpro1_register_images_process_h_
#define vidpro1_register_images_process_h_

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
class vidpro1_register_images_process : public bpro1_process {

public:

  vidpro1_register_images_process();
  ~vidpro1_register_images_process();

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
    vimt_transform_2d total_xform;
    bool first_frame_;
    
    int last_frame_no;
    int ni_;
    int nj_;

    vcl_vector<vnl_matrix<double> > homographies_;
    vcl_vector<vimt_transform_2d > xforms_;

    bool read_homographies(vcl_string filename);

    //: dimensions for mosaic
    vbl_bounding_box<double,2> box_;

    int bimg_ni;
    int bimg_nj;

    int offset_i;
    int offset_j;


    vcl_string outfilename;




};

#endif

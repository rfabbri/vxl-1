// This is brcv/mvg/dbvrl/pro/dbvrl_register_images_process.h
#ifndef dbvrl_register_images_process_h_
#define dbvrl_register_images_process_h_

//:
// \file
// \brief 
// \author Vishal Jain(vj@lems.brown.edu)
// \date 05/11/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>
#include <vimt/vimt_transform_2d.h>
#include <vbl/vbl_bounding_box.h>
//: Derived video process class for computing frame difference
class dbvrl_register_images_process : public bpro1_process {

public:

  dbvrl_register_images_process();
  ~dbvrl_register_images_process();

  std::string name();

  //: Clone the process
  virtual bpro1_process* clone() const;

  int input_frames();
  int output_frames();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

private:
    vimt_transform_2d total_xform;
    bool first_frame_;
    
    int last_frame_no;
    int ni_;
    int nj_;

    std::vector<vnl_matrix<double> > homographies_;
    std::vector<vimt_transform_2d > xforms_;

    bool read_homographies(std::string filename);

    //: dimensions for mosaic
    vbl_bounding_box<double,2> box_;

    int bimg_ni;
    int bimg_nj;

    int offset_i;
    int offset_j;


    std::string outfilename;




};

#endif

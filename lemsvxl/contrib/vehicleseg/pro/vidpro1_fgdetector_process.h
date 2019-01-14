// This is pro/vidpro1_fgdetector_process.h
#ifndef vidpro1_fgdetector_process_h_
#define vidpro1_fgdetector_process_h_

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
#include <vector>
#include <string>
#include <dbbgm/dbbgm_bgmodel.h>
#include <dbbgm/dbbgm_bgmodel_sptr.h>
//: Derived video process class for computing frame difference
class vidpro1_fgdetector_process : public bpro1_process {

public:

  vidpro1_fgdetector_process();
  ~vidpro1_fgdetector_process();

  std::string name();

  //: Clone the process
  virtual bpro1_process* clone() const;

  int input_frames();
  int output_frames();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

  void setmodel(dbbgm_bgmodel_sptr bgm);
  dbbgm_bgmodel_sptr bgM;
protected:
   
private:
    
  

    bool ismodelset_;
  
    int frame_no;
    int ifnum;
   
    vil_image_view<float> float_curr_view;
    vil_image_view<float> float_last_view;


};

#endif

// This is /lemsvxl/brcv/rec/dborl/algo/vox_train_xgraph_geometry_and_ccm_models/vox_train_xgraph_ccm_model/vox_train_xshock_ccm_model_process.h

#ifndef vox_train_xshock_ccm_model_process_h_
#define vox_train_xshock_ccm_model_process_h_

//:
// \file
// \brief 
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Mar 15, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: 
class vox_train_xshock_ccm_model_process : public bpro1_process
{
public:
  //: Constructor
  vox_train_xshock_ccm_model_process();
  
  //: Destructor
  virtual ~vox_train_xshock_ccm_model_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
};

#endif

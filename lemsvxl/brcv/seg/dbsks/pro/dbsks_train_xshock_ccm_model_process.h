// This is dbsks/pro/dbsks_train_xshock_ccm_model_process.h

#ifndef dbsks_train_xshock_ccm_model_process_h_
#define dbsks_train_xshock_ccm_model_process_h_

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Jan 22, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

//: 
class dbsks_train_xshock_ccm_model_process : public bpro1_process 
{
public:
  //: Constructor
  dbsks_train_xshock_ccm_model_process();
  
  //: Destructor
  virtual ~dbsks_train_xshock_ccm_model_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
};

#endif

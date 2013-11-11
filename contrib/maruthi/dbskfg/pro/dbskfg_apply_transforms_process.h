// This is brcv/shp/dbskfg/pro/dbskfg_apply_transforms_process.h
#ifndef dbskfg_apply_transforms_process_h_
#define dbskfg_apply_transforms_process_h_

//:
// \file
// \brief This process forms a composite graph_process data structure from
//        an intrinsinc shock graph_process
//
// \author Maruthi Narayanan
// \date 07/07/10
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

class dbskfg_apply_transforms_process : public bpro1_process 
{

public:
  //: Constructor
  dbskfg_apply_transforms_process();
  
  //: Destructor
  virtual ~dbskfg_apply_transforms_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
  
};

#endif  //dbskfg_apply_transforms_process_h_

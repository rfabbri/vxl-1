// This is brcv/shp/dbskfg/pro/dbskfg_detect_fragments_process.h
#ifndef dbskfg_detect_fragments_process_h_
#define dbskfg_detect_fragments_process_h_

//:
// \file
// \brief This process will detect a bounding box from a set of fragments
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

class dbskfg_detect_fragments_process : public bpro1_process 
{

public:
  //: Constructor
  dbskfg_detect_fragments_process();
  
  //: Destructor
  virtual ~dbskfg_detect_fragments_process();

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

#endif  //dbskfg_detect_fragments_process_h_

// This is brcv/shp/dbskfg/pro/dbskfg_match_bag_of_fragments_process.h
#ifndef dbskfg_match_bag_of_fragments_process_h_
#define dbskfg_match_bag_of_fragments_process_h_

//:
// \file
// \brief This process will match a bag of fragments to another bag of 
//        of fragments
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

class dbskfg_match_bag_of_fragments_process : public bpro1_process 
{

public:
  //: Constructor
  dbskfg_match_bag_of_fragments_process();
  
  //: Destructor
  virtual ~dbskfg_match_bag_of_fragments_process();

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

#endif  //dbskfg_match_bag_of_fragments_process_h_

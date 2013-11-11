// This is brcv/shp/dbskfg/pro/dbskfg_match_bag_of_curves_process.h
#ifndef dbskfg_match_bag_of_curves_process_h_
#define dbskfg_match_bag_of_curves_process_h_

//:
// \file
// \brief This process will match a bag of curves to another bag of 
//        of curves
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

class dbskfg_match_bag_of_curves_process : public bpro1_process 
{

public:
  //: Constructor
  dbskfg_match_bag_of_curves_process();
  
  //: Destructor
  virtual ~dbskfg_match_bag_of_curves_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

private:

  void load_contours(vcl_vector<bpro1_storage_sptr>& vsol_contour,
                     bpro1_filepath& input);

};

#endif  //dbskfg_match_bag_of_curves_process_h_

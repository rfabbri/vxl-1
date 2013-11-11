// This is brl/blem/brcv/trk/dbinfo/dbinfo_mutual_info_process.h
#ifndef dbinfo_mutual_info_process_h_
#define dbinfo_mutual_info_process_h_

//:
// \file
// \brief A bpro1 process that computes mutual information between images
// \author J. L. Mundy
// \date November 5, 2005
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

class dbinfo_mutual_info_process : public bpro1_process
{
public:

  dbinfo_mutual_info_process();
  dbinfo_mutual_info_process(const dbinfo_mutual_info_process& other);
  ~dbinfo_mutual_info_process();

  //: Clone the process
  virtual bpro1_process* clone() const; 
  
  //: The name of the video process
  virtual vcl_string name() {return "Mutual Information";}

  int input_frames();
  int output_frames();
  
  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();
  
private:
  //: set the parameters from menu assigned values

};

#endif // dbinfo_mutual_info_process_h_

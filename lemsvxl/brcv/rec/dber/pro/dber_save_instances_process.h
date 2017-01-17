// This is brcv/rec/dber/pro/dber_save_instances_process.h

#ifndef dber_save_instances_process_h_
#define dber_save_instances_process_h_

//:
// \file
// \brief A process for saving Video instances storage
// \author O. C. Ozcanli
// \date Nov 8, 2006
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>

class dber_save_instances_process : public bpro1_process
{
 public:

  dber_save_instances_process();
 ~dber_save_instances_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  void clear_output();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
};

#endif // dber_save_instances_process

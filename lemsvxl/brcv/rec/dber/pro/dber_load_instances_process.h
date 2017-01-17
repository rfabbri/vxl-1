//---------------------------------------------------------------------
// This is brcv/rec/dber/pro/dber_load_instances_process.h
//:
// \file
// \brief process to load instances from binary files, creates an dber_instance storage
//
// \author
//  O.C. Ozcanli - Nov 8, 06
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dber_load_instances_process_h_
#define dber_load_instances_process_h_

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <dber/pro/dber_instance_storage.h>
#include <dber/pro/dber_instance_storage_sptr.h>

class dber_load_instances_process : public bpro1_process 
{
public:
  dber_load_instances_process();
  virtual ~dber_load_instances_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Load Video Instances";
  }

  vcl_vector< vcl_string > get_input_type() {
    return vcl_vector< vcl_string >();
  }
  vcl_vector< vcl_string > get_output_type() {
    return vcl_vector< vcl_string >();
  }

  int input_frames() {
    return 0;
  }
  int output_frames() {
    return 0;
  }

  bool execute();
  bool finish();

protected:
  dber_instance_storage_sptr instance_storage_;
  
};

#endif  // dber_load_instances_process_h_

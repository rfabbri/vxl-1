//---------------------------------------------------------------------
// This is brcv/rec/dbru/pro/dbru_load_osl_process.h
//:
// \file
// \brief process to load OSL from binary files, creates an OSL storage
//
// \author
//  O.C. Ozcanli - Aug 01, 06
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dbru_load_osl_process_h_
#define dbru_load_osl_process_h_

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <dbru/pro/dbru_osl_storage.h>
#include <dbru/pro/dbru_osl_storage_sptr.h>

class dbru_load_osl_process : public bpro1_process 
{
public:
  dbru_load_osl_process();
  virtual ~dbru_load_osl_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Load OSL";
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
  dbru_osl_storage_sptr osl_storage_;
  
};

#endif  // dbru_load_osl_process_h_

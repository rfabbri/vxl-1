// This is brl/brcv/trk/dbinfo/pro/dbinfo_load_osl_process.h

#ifndef dbinfo_load_osl_process_h_
#define dbinfo_load_osl_process_h_

//:
// \file
// \brief A process for loading a osl storage file
// \author J. L. Mundy
// \date May 10, 2006
//
// \verbatim
//  Modifications <none>
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <dbinfo/pro/dbinfo_osl_storage_sptr.h>

class dbinfo_load_osl_process : public bpro1_process
{
 public:
  
  dbinfo_load_osl_process();
 ~dbinfo_load_osl_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  virtual void clear_output(int resize = -1);

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();
 

 protected:

 dbinfo_osl_storage_sptr osl_storage_;
};

#endif // dbinfo_load_osl_process

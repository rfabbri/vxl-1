// This is brcv/trk/dbinfo/pro/dbinfo_create_empty_osl_process.h

#ifndef dbru_create_empty_osl_process_h_
#define dbru_create_empty_osl_process_h_

//:
// \file
// \brief A process for creating an empty OSL
// \author O. C. Ozcanli
// \date July 31, 2006
//
// \verbatim
//  Modifications
// \endverbatim


#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>
#include <dbru/pro/dbru_osl_storage_sptr.h>

class dbru_create_empty_osl_process : public bpro1_process
{
 public:

  dbru_create_empty_osl_process();
 ~dbru_create_empty_osl_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  void clear_output();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
protected:
  dbru_osl_storage_sptr osl_storage_;
};

#endif // dbru_create_empty_osl_process

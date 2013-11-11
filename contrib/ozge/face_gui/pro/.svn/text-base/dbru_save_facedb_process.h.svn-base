// This is ozge/face_gui/pro/dbru_save_facedb_process.h

#ifndef dbru_save_facedb_process_h_
#define dbru_save_facedb_process_h_

//:
// \file
// \brief A process for saving facedb storage
// \author O. C. Ozcanli
// \date Aug 19, 06
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>

class dbru_save_facedb_process : public bpro1_process
{
 public:

  dbru_save_facedb_process();
 ~dbru_save_facedb_process();

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

#endif // dbru_save_facedb_process

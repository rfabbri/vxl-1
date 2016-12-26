//---------------------------------------------------------------------
// This is ozge/face_gui/pro/dbru_load_facedb_process.h
//:
// \file
// \brief process to load facedb from binary files, creates a facedb storage
//
// \author
//  O.C. Ozcanli - Aug 19, 06
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dbru_load_facedb_process_h_
#define dbru_load_facedb_process_h_

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <ozge/face_gui/pro/dbru_facedb_storage.h>

class dbru_load_facedb_process : public bpro1_process 
{
public:
  dbru_load_facedb_process();
  virtual ~dbru_load_facedb_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Load facedb";
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
  dbru_facedb_storage_sptr facedb_storage_;
  
};

#endif  // dbru_load_facedb_process_h_

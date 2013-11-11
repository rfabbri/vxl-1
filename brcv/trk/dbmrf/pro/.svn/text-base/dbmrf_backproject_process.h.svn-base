// This is brcv/trk/dbmrf/pro/dbmrf_backproject_process.h
#ifndef dbmrf_backproject_process_h_
#define dbmrf_backproject_process_h_

//:
// \file
// \brief back project episegs into space
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 01/12/07
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>

#include <bpro1/bpro1_parameters.h>


class dbmrf_backproject_process : public bpro1_process
{
public:

  dbmrf_backproject_process();
  ~dbmrf_backproject_process();

  dbmrf_backproject_process(const dbmrf_backproject_process& other);

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();


private:

};

#endif // dbmrf_backproject_process_h_

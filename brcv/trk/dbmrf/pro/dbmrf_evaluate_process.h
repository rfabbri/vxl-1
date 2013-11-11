// This is brcv/trk/dbmrf/pro/dbmrf_evaluate_process.h
#ifndef dbmrf_evaluate_process_h_
#define dbmrf_evaluate_process_h_

//:
// \file
// \brief predict the motion of the curves 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 01/09/07
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>

#include <bpro1/bpro1_parameters.h>


class dbmrf_evaluate_process : public bpro1_process
{
public:

  dbmrf_evaluate_process();
  ~dbmrf_evaluate_process();

  dbmrf_evaluate_process(const dbmrf_evaluate_process& other);

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

#endif // dbmrf_evaluate_process_h_

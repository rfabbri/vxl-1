// This is brcv/trk/dbmrf/pro/dbmrf_rocdata_process.h
#ifndef dbmrf_rocdata_process_h_
#define dbmrf_rocdata_process_h_

//:
// \file
// \brief export data for building ROC curves
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 01/16/07
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>

#include <bpro1/bpro1_parameters.h>


class dbmrf_rocdata_process : public bpro1_process
{
public:

  dbmrf_rocdata_process();
  ~dbmrf_rocdata_process();

  dbmrf_rocdata_process(const dbmrf_rocdata_process& other);

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

#endif // dbmrf_rocdata_process_h_

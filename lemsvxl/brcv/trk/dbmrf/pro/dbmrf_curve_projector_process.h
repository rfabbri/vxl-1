// This is brcv/trk/dbmrf/pro/dbmrf_curve_projector_process.h
#ifndef dbmrf_curve_projector_process_h_
#define dbmrf_curve_projector_process_h_

//:
// \file
// \brief A process projects curve_3d into 2D vsol curves
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 7/19/04
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>


class dbmrf_curve_projector_process : public bpro1_process
{
public:

  dbmrf_curve_projector_process();
  ~dbmrf_curve_projector_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();
  
  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

};

#endif // dbmrf_curve_projector_process_h_

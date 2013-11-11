// This is brl/vidpro/process/vidpro_global_harris_corners_process.h
#ifndef vidpro_global_harris_corners_process_h_
#define vidpro_global_harris_corners_process_h_

//:
// \file
// \brief A process that computes global_harris Corners
// \author Mark Johnson (mrj@lems.brown.edu)
// \date 7/21/03
//
// \verbatim
//  Modifications
//   Matt Leotta  1/26/04    Updated comments in Doxygen form
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro/bpro_process.h>

#include <bpro/bpro_parameters.h>


class vidpro_global_harris_corners_process : public bpro_process {

public:

  vidpro_global_harris_corners_process();
  ~vidpro_global_harris_corners_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();
  
  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

protected:

private:

};

#endif // vidpro_global_harris_corners_process_h_

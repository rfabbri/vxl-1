// This is brl/vidpro/process/vidpro_harris_corners_process.h
#ifndef vidpro_harris_corners_process_h_
#define vidpro_harris_corners_process_h_

//:
// \file
// \brief A process that computes Harris Corners
// \author Mark Johnson (mrj@lems.brown.edu)
// \date 7/21/03
//
// \verbatim
//  Modifications
//   Matt Leotta  1/26/04    Updated comments in Doxygen form
// \endverbatim


#include <vector>
#include <string>
#include <bpro/bpro_process.h>

#include <bpro/bpro_parameters.h>


class vidpro_harris_corners_process : public bpro_process {

public:

  vidpro_harris_corners_process();
  ~vidpro_harris_corners_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  std::string name();

  int input_frames();
  int output_frames();
  
  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

protected:

private:

};

#endif // vidpro_harris_corners_process_h_

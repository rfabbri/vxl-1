// This is brl/vidpro1/process/vidpro1_harris_corners_process.h
#ifndef vidpro1_harris_corners_process_h_
#define vidpro1_harris_corners_process_h_

//:
// \file
// \brief A process that computes Harris Corners
// \author  Based on original code by Mark Johnson
// \date 7/21/03
//
// \verbatim
//  Modifications
//   Matt Leotta  1/26/04    Updated comments in Doxygen form
// \endverbatim


#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>

#include <bpro1/bpro1_parameters.h>


class vidpro1_harris_corners_process : public bpro1_process {

public:

  vidpro1_harris_corners_process();
  ~vidpro1_harris_corners_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

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

#endif // vidpro1_harris_corners_process_h_

// This is breye/vidpro/process/vidpro_gaussian_blur_process.h
#ifndef vidpro_gaussian_blur_process_h_
#define vidpro_gaussian_blur_process_h_

//:
// \file
// \brief A process that smooths each image with a Gaussian filter
// \author Mark Johnson (mrj@lems.brown.edu)
// \date 8/27/03
//
// \verbatim
//  Modifications
//   Matt Leotta  1/26/04    Updated comments in Doxygen format
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro/bpro_process.h>


class vidpro_gaussian_blur_process : public bpro_process {

public:

  vidpro_gaussian_blur_process();
  virtual ~vidpro_gaussian_blur_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
  
protected:

private:

};

#endif // vidpro_gaussian_blur_process_h_

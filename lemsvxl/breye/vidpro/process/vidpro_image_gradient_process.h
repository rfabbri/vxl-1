// This is breye/vidpro/process/vidpro_image_gradient_process.h
#ifndef vidpro_image_gradient_process_h_
#define vidpro_image_gradient_process_h_

//:
// \file
// \brief A process that computes the gradient of an image
// \author Amir Tamrakar
// \date 09/26/06
//
// \verbatim
//  Modifications
//   
// \endverbatim


#include <vector>
#include <string>
#include <bpro/bpro_process.h>


class vidpro_image_gradient_process : public bpro_process {

public:

  vidpro_image_gradient_process();
  virtual ~vidpro_image_gradient_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
  
protected:

private:

};

#endif // vidpro_image_gradient_process_h_

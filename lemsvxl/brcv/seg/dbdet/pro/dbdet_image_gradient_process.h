// This is breye1/vidpro1/process/dbdet_image_gradient_process.h
#ifndef dbdet_image_gradient_process_h_
#define dbdet_image_gradient_process_h_

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
#include <bpro1/bpro1_process.h>


class dbdet_image_gradient_process : public bpro1_process {

public:

  dbdet_image_gradient_process();
  virtual ~dbdet_image_gradient_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

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

#endif // dbdet_image_gradient_process_h_

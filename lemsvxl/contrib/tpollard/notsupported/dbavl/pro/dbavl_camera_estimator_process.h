#ifndef dbavl_camera_estimator_process_h_
#define dbavl_camera_estimator_process_h_

//:
// \file
// \brief A process for dbavl_camera_estimator
// \author Thomas Pollard
// \date 8/29/06
//

#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include "../dbavl_camera_estimator.h"


class dbavl_camera_estimator_process : public bpro1_process {

public:

  dbavl_camera_estimator_process();
  ~dbavl_camera_estimator_process();

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
  
  dbavl_camera_estimator ce;

private:

};

#endif // dbavl_camera_estimator_process_h_

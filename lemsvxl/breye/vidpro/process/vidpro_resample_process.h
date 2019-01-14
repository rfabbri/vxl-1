// This is breye/vidpro/process/vidpro_resample_process.h
#ifndef vidpro_resample_process_h_
#define vidpro_resample_process_h_

//:
// \file
// \brief A process that resamples an image
// \author Matt Leotta
// \date 8/26/05
//
// \verbatim
// \endverbatim


#include <vector>
#include <string>
#include <bpro/bpro_process.h>


class vidpro_resample_process : public bpro_process {

public:

  vidpro_resample_process();
  virtual ~vidpro_resample_process();

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

#endif // vidpro_resample_process_h_

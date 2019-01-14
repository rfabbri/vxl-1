// This is brcv/mvg/dvpgl/algo/pro/vidpro_vsol_scale_process.h
#ifndef vidpro_vsol_scale_process_h_
#define vidpro_vsol_scale_process_h_

//:
// \file
// \brief A process that scales vsol objects
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 8/26/05
//
// \verbatim
//  Modifications
// \endverbatim


#include <vector>
#include <string>
#include <bpro/bpro_process.h>

#include <bpro/bpro_parameters.h>


class vidpro_vsol_scale_process : public bpro_process
{
public:

  vidpro_vsol_scale_process();
  ~vidpro_vsol_scale_process(){}

  //: Clone the process
  virtual bpro_process* clone() const;
  
  std::string name();

  int input_frames();
  int output_frames();
  
  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();


};

#endif // vidpro_vsol_scale_process_h_

// This is brl/vidpro/process/vidpro_Rothwell_edge_process.h
#ifndef vidpro_Rothwell_edge_process_h_
#define vidpro_Rothwell_edge_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Computes Rothwell-Mundy edges on each video frame
//
// \author Amir Tamrakar
// \date 03/29/06
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------

#include <vector>
#include <string>
#include <osl/osl_canny_rothwell_params.h>
#include <bpro/bpro_process.h>

class vidpro_Rothwell_edge_process : public bpro_process 
{
public:

  vidpro_Rothwell_edge_process();
  virtual ~vidpro_Rothwell_edge_process();

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
  osl_canny_rothwell_params dp;

};


#endif // vidpro_Rothwell_edge_process_h_

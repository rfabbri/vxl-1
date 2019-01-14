// This is brl/vidpro/process/vidpro_VD_edge_process.h
#ifndef vidpro_VD_edge_process_h_
#define vidpro_VD_edge_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Computes VD edges on each video frame
//
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 11, 2002    Initial version.
//   Amir Tamrakar Nov 11, 2003     Update version for brown eyes
// \endverbatim
//--------------------------------------------------------------------------------

#include <vector>
#include <string>
#include <sdet/sdet_detector_params.h>
#include <bpro/bpro_process.h>


class vidpro_VD_edge_process : public bpro_process 
{
public:

  vidpro_VD_edge_process();
  virtual ~vidpro_VD_edge_process();

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
  sdet_detector_params dp;

};


#endif // vidpro_VD_edge_process_h_

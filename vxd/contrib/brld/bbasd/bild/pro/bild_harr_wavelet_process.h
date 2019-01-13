// This is basic/bild/pro/bild_harr_wavelet_process.h
#ifndef bild_harr_wavelet_process_h_
#define bild_harr_wavelet_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief  A process for the Harr Wavelet Transform and its inverse
//
// \author Based on original code by  Matt Leotta
// \date 10/20/05
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------

#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>

class bild_harr_wavelet_process : public bpro1_process 
{
public:

  bild_harr_wavelet_process();
  virtual ~bild_harr_wavelet_process();

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
};


#endif // bild_harr_wavelet_process_h_

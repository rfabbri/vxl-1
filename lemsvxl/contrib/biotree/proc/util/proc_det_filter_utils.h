#ifndef proc_det_filter_utils_h_
#define proc_det_filter_utils_h_

//: 
// \file   proc_det_filter_utils.h
// \brief  utility methods to handle the filter responses, such as reading and 
//         reformatting the responses for different needs
// \author    Gamze D. Tunali
// \date      2005-10-01
// 

#include <vnl/vnl_matrix_fixed.h>
#include <vector>
#include <xmvg/xmvg_icosafilter_response.h>


class proc_det_filter_utils
{
public:
  proc_det_filter_utils(void);
  ~proc_det_filter_utils(void);
  std::vector<xmvg_filter_response<double> >  read_responses(std::string response_file,
                                          int &dimx, int &dimy, 
                                          int &dimz, int &filter_size);

  void diff_responses(std::string f1, std::string f2, std::string out);

  void crop_responses(std::string response_file,
                                          std::string output_file,
                                          int &dimx, int &dimy, 
                                          int &dimz, int &filter_size,
                                          int xmar, int ymar, int zmar);
};

#endif


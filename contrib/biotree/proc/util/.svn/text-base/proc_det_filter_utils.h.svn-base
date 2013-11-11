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
#include <vcl_vector.h>
#include <xmvg/xmvg_icosafilter_response.h>


class proc_det_filter_utils
{
public:
  proc_det_filter_utils(void);
  ~proc_det_filter_utils(void);
  vcl_vector<xmvg_filter_response<double> >  read_responses(vcl_string response_file,
                                          int &dimx, int &dimy, 
                                          int &dimz, int &filter_size);

  void diff_responses(vcl_string f1, vcl_string f2, vcl_string out);

  void crop_responses(vcl_string response_file,
                                          vcl_string output_file,
                                          int &dimx, int &dimy, 
                                          int &dimz, int &filter_size,
                                          int xmar, int ymar, int zmar);
};

#endif


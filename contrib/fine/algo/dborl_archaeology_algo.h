//:
// \file
// \brief An algorithm to run JSEG segmentation, create a segmentation structure, and extract the contour of the fragment
//
// \author Eli Fine (eli_fine@brown.edu)
// \date 09/23/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_archaeology_algo_h)
#define _dborl_archaeology_algo_h

#include "dborl_archaeology_algo_params_sptr.h"
#include <vcl_string.h>
#include <vcl_vector.h>
#include <dborl/dborl_index_sptr.h>
#include <dborl/dborl_index_node_sptr.h>
#include <vil/vil_load.h>
#include <structure/dbseg_seg_object_base.h>
#include <pro/dbseg_jseg_process.h>
#include <pro/dbseg_seg_process.h>
#include <pro/dbseg_seg_save_process.h>
#include <pro/dbseg_seg_save_contour_process.h>
#include <vil/vil_image_view_base.h>
#include <vul/vul_file.h>

class dborl_archaeology_algo 
{
public:
  dborl_archaeology_algo(dborl_archaeology_algo_params_sptr params) : params_(params) {}
  bool initialize();
  bool process();
  bool finalize();
  bool parse_index(vcl_string index_file);

protected:
  dborl_archaeology_algo_params_sptr params_;

  dborl_index_sptr ind_;
  dborl_index_node_sptr root_;
};

#endif  //_dborl_archaeology_algo_h





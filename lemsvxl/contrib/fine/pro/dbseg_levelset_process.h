// This is fine/contrib/dbseg_levelset_process.h

#ifndef dbseg_levelset_process_h_
#define dbseg_levelset_process_h_

//:
// \file
// \brief A process for running the level set segmentation algorithm on an image
// \author Eli Fine
// \date 8/28/08
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <vcl_list.h>
#include <vul/vul_timer.h>

#include <vcl_iostream.h>
#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>

/*#include <algos/levelset/dbseg_levelset_LevelSetFunction.h>
#include <algos/levelset/dbseg_levelset_NormalDistributionForce.h>
#include <algos/levelset/dbseg_levelset_ParzenDistributionForce.h>
#include <algos/levelset/dbseg_levelset_SingleRegionAlgorithm.h>
#include <algos/levelset/dbseg_levelset_MultiRegionAlgorithm.h>
#include <algos/levelset/dbseg_levelset_Region.h>
#include <algos/levelset/dbseg_levelset_simple_tools.h>
#include <algos/levelset/dbseg_levelset_cimg_dependent.h>
#include <algos/levelset/dbseg_levelset_level_set.h>
#include <algos/levelset/dbseg_levelset_Filters.h>
#include <algos/levelset/dbseg_levelset_image_utils.h>
#include <algos/levelset/dbseg_levelset_colordefs.h>
#include <time.h>
#include <string>
#include <assert.h>
#include <stdexcept>*/

#include <dbacm/dbacm_standard_levelset_func.h>
 


class dbseg_levelset_process : public bpro1_process
{
 public:

  dbseg_levelset_process();
 ~dbseg_levelset_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  void clear_output();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

};

#endif // dbseg_levelset_process



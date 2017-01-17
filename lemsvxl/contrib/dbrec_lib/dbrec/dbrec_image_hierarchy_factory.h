// This is lemsvxlsrc/contrib/dbrec/dbrec_image_hierarchy_factory.h
#ifndef dbrec_image_hierarchy_factory_h_
#define dbrec_image_hierarchy_factory_h_
//:
// \file
// \brief class to build part hierarchies for 2D images using gaussian primitives manually
//
// Note: higher layer parts are positioned wrt to a coordinate system origined at the center of the central part
//       so during training, create samples by subtracting other parts' centers from central part's center after measuring from the image
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 05/26/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <dbrec/dbrec_part_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vcl_vector.h>

class dbrec_image_hierarchy_factory
{
 public:

  //: for recognition of vehicles on Steeple Street Data
  static dbrec_hierarchy_sptr construct_detector_steeple0(); // vehicles

  //: Detector for short vehicles
  //  Construct a hierarchy manually for ROI 1
  static dbrec_hierarchy_sptr construct_detector_roi1_0();
  static dbrec_hierarchy_sptr construct_detector_roi1_0_prims();  // a hierarchy which contains only the prims for roi1_0 detector
  static dbrec_hierarchy_sptr construct_detector_roi1_0_1();  // a hierarchy which breaks roi1_0 so that there are two compositional parts with 2 prims, and they are composed such that their centers overlap
  static dbrec_hierarchy_sptr construct_detector_roi1_0_1_part();  // the level 1 compositional part of roi1_0_1 with discrete pairwise model for training

  //: Detector for longer vehicles
  //  Construct a hierarchy manually for ROI 1
  static dbrec_hierarchy_sptr construct_detector_roi1_1();

  static dbrec_hierarchy_sptr construct_detector_roi1_0_downsampled_by_2();

  //: for balloon videos, v2, lane 1, 
  //  see: vision/docs/mundy/voxel/ozge/v2_analysis.pptx, 
  //  video id: 10080, video dir: vision\projects\kimia\categorization\vehicles\camera_ready_expts\v\v2-data\road429-f50-f229\430-267-380 
  //static dbrec_hierarchy_sptr construct_detector_10080_car(); // for both bright and dark vehicles

  //: manual hierarchies with rotationally invariant parts for sampling experiments (e.g. we will use these hierarchies first to sample instances from them, and second to detect those instances)
  static dbrec_hierarchy_sptr construct_detector_rot_inv_1();
  static dbrec_hierarchy_sptr construct_detector_rot_inv_1_prims();  // only primitives of the hierarchy # 1

  static dbrec_hierarchy_sptr construct_detector_rot_inv_roi1_0();
  static dbrec_hierarchy_sptr construct_detector_rot_inv_roi1_0_level1();  // only level 1 of the rot inv hierarchy of roi1_0
  static dbrec_hierarchy_sptr construct_detector_rot_inv_roi1_0_prims(); // only prims of roi1_0

  static dbrec_hierarchy_sptr construct_detector_rot_inv_roi1_2(); // a new detector for both short and longer vehicles

  //: a manual detector for haifa dataset, same as roi1_0 detector, the direction for the primitives is set to 0, the orientation is set during parsing
  static dbrec_hierarchy_sptr construct_detector_haifa(float orientation_angle);

  static dbrec_hierarchy_sptr construct_detector_rot_inv_haifa_prims();
  static dbrec_hierarchy_sptr construct_detector_rot_inv_haifa_prims2();

};

#endif // brec_part_hierarchy_builder_h_


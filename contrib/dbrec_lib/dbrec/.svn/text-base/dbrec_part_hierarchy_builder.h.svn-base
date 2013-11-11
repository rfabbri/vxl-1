// This is lemsvxlsrc/contrib/dbrec/dbrec_part_hierarchy_builder.h
#ifndef dbrec_part_hierarchy_builder_h_
#define dbrec_part_hierarchy_builder_h_
//:
// \file
// \brief class to build a hierarchy of composable parts for recognition
//
// Includes tools to learn co-occurrence statistics or manual construction of hierarchies for specific purposes
//
// Note: higher layer parts are positioned wrt to a coordinate system origined at the center of the central part
//       so during training, create samples by subtracting other parts' centers from central part's center after measuring from the image
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 04/07/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <brec/brec_part_hierarchy_sptr.h>
#include <brec/brec_part_base_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vcl_vector.h>

class dbrec_part_hierarchy_builder
{
 public:

  //: for note recognition
  static brec_part_hierarchy_sptr construct_mi_detector();

  //: for steeple street vehicles
  static brec_part_hierarchy_sptr construct_detector_steeple0(); // vehicles
  static brec_part_hierarchy_sptr construct_detector_steeple1(); // dark vehicles only

  //: for balloon videos, v2, lane 1, 
  //  see: vision/docs/mundy/voxel/ozge/v2_analysis.pptx, 
  //  video id: 10080, video dir: vision\projects\kimia\categorization\vehicles\camera_ready_expts\v\v2-data\road429-f50-f229\430-267-380 
  static brec_part_hierarchy_sptr construct_detector_10080_car(); // for both bright and dark vehicles

};

#endif // brec_part_hierarchy_builder_h_


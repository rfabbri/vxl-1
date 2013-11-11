/*
 * dbdet_graddt_levelset2d_volume_segmentation_process.h
 *
 *  Created on: Feb 24, 2012
 *      Author: firat
 */

#ifndef DBDET_GRADDT_LEVELSET2D_VOLUME_SEGMENTATION_PROCESS_H_
#define DBDET_GRADDT_LEVELSET2D_VOLUME_SEGMENTATION_PROCESS_H_

//:
// \file
// \brief Process to segment 3D volumes using the graddt based 2D slice-by-slice
//        levelset segmentation
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

class dbdet_graddt_levelset2d_volume_segmentation_process : public bpro1_process
{

public:
  //: Constructor
  dbdet_graddt_levelset2d_volume_segmentation_process();

  //: Destructor
  virtual ~dbdet_graddt_levelset2d_volume_segmentation_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

};



#endif /* DBDET_GRADDT_LEVELSET2D_VOLUME_SEGMENTATION_PROCESS_H_ */

/*
 * dbdet_third_order_3d_edge_detector_process.h
 *
 *  Created on: Oct 16, 2011
 *      Author: firat
 */

#ifndef DBDET_THIRD_ORDER_3D_EDGE_DETECTOR_PROCESS_H_
#define DBDET_THIRD_ORDER_3D_EDGE_DETECTOR_PROCESS_H_

//:
// \file
// \brief Process to detect 3d third order edges
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

class dbdet_third_order_3d_edge_detector_process : public bpro1_process
{

public:
  //: Constructor
  dbdet_third_order_3d_edge_detector_process();

  //: Destructor
  virtual ~dbdet_third_order_3d_edge_detector_process();

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


#endif /* DBDET_THIRD_ORDER_3D_EDGE_DETECTOR_PROCESS_H_ */

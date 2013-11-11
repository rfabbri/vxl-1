/*
 * dbdet_pb_edge_detector_process.h
 *
 *  Created on: Feb 22, 2010
 *      Author: firat
 */

#ifndef DBDET_PB_EDGE_DETECTOR_PROCESS_H_
#define DBDET_PB_EDGE_DETECTOR_PROCESS_H_

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <vcl_string.h>
#include <vcl_vector.h>

class dbdet_pb_edge_detector_process : public bpro1_process
{
public:

    dbdet_pb_edge_detector_process();
  virtual ~dbdet_pb_edge_detector_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

};

#endif /* DBDET_PB_EDGE_DETECTOR_PROCESS_H_ */

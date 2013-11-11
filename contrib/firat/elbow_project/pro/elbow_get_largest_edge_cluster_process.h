/*
 * elbow_get_largest_edge_cluster_process.h
 *
 *  Created on: Oct 18, 2011
 *      Author: firat
 */

#ifndef ELBOW_GET_LARGEST_EDGE_CLUSTER_PROCESS_H_
#define ELBOW_GET_LARGEST_EDGE_CLUSTER_PROCESS_H_

//:
// \file
// \brief Process to get the largest edge cluster
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

class elbow_get_largest_edge_cluster_process : public bpro1_process
{

public:
  //: Constructor
  elbow_get_largest_edge_cluster_process();

  //: Destructor
  virtual ~elbow_get_largest_edge_cluster_process();

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


#endif /* ELBOW_GET_LARGEST_EDGE_CLUSTER_PROCESS_H_ */

/*
 * elbow_mesh_post_process.h
 *
 *  Created on: Oct 23, 2011
 *      Author: firat
 */

#ifndef ELBOW_MESH_POST_PROCESS_H_
#define ELBOW_MESH_POST_PROCESS_H_

//:
// \file
// \brief Mesh post processing
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

class elbow_mesh_post_process : public bpro1_process
{

public:
  //: Constructor
  elbow_mesh_post_process();

  //: Destructor
  virtual ~elbow_mesh_post_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

};


#endif /* ELBOW_MESH_POST_PROCESS_H_ */

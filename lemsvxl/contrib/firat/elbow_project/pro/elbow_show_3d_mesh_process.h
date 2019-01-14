/*
 * elbow_show_3d_mesh_process.h
 *
 *  Created on: Oct 23, 2011
 *      Author: firat
 */

#ifndef ELBOW_SHOW_3D_MESH_PROCESS_H_
#define ELBOW_SHOW_3D_MESH_PROCESS_H_

//:
// \file
// \brief Mesh visualization
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

class elbow_show_3d_mesh_process : public bpro1_process
{

public:
  //: Constructor
  elbow_show_3d_mesh_process();

  //: Destructor
  virtual ~elbow_show_3d_mesh_process();

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



#endif /* ELBOW_SHOW_3D_MESH_PROCESS_H_ */

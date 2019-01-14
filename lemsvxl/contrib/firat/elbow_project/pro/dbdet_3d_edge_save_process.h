/*
 * dbdet_3d_edge_save_process.h
 *
 *  Created on: Oct 24, 2011
 *      Author: firat
 */

#ifndef DBDET_3D_EDGE_SAVE_PROCESS_H_
#define DBDET_3D_EDGE_SAVE_PROCESS_H_

//:
// \file
// \brief Save 3d edges
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

class dbdet_3d_edge_save_process : public bpro1_process
{

public:
  //: Constructor
  dbdet_3d_edge_save_process();

  //: Destructor
  virtual ~dbdet_3d_edge_save_process();

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



#endif /* DBDET_3D_EDGE_SAVE_PROCESS_H_ */

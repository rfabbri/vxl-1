/*
 * poisson_reconstruction_process.h
 *
 *  Created on: Oct 21, 2011
 *      Author: firat
 */

#ifndef POISSON_RECONSTRUCTION_PROCESS_H_
#define POISSON_RECONSTRUCTION_PROCESS_H_

//:
// \file
// \brief Wrapper process for poisson reconstruction
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

class poisson_reconstruction_process : public bpro1_process
{

public:
  //: Constructor
  poisson_reconstruction_process();

  //: Destructor
  virtual ~poisson_reconstruction_process();

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




#endif /* POISSON_RECONSTRUCTION_PROCESS_H_ */

// This is brcv/trk/dbmrf/pro/dbmrf_network_builder_process.h
#ifndef dbmrf_network_builder_process_h_
#define dbmrf_network_builder_process_h_

//:
// \file
// \brief A process that incrementely builds a bmrf_network
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/27/04
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>

#include <bpro1/bpro1_parameters.h>
#include <bmrf/bmrf_network_builder_params.h>
#include <bmrf/bmrf_network_builder.h>


class dbmrf_network_builder_process : public bpro1_process
{
public:

  dbmrf_network_builder_process();
  ~dbmrf_network_builder_process();

  dbmrf_network_builder_process(const dbmrf_network_builder_process& other);

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  vcl_string name();

  int input_frames();
  int output_frames();
  
  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

protected:
  //: The parameters for the builder
  bmrf_network_builder_params net_params_;
  //: A pointer to the builder
  bmrf_network_builder* builder_;

private:

};

#endif // dbmrf_network_builder_process_h_

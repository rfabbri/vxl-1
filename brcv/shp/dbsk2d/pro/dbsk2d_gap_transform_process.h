// This is brcv/shp/dbsk2d/pro/dbsk2d_gap_transform_process.h
#ifndef dbsk2d_gap_transform_process_h_
#define dbsk2d_gap_transform_process_h_
//:
// \file
// \brief This process performs gap transforms on a given shock graph
//        in a rank ordered fashion, based on cost of gap transform
// \author Ozge Can Ozcanli
// \date 10/15/06
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

class dbsk2d_gap_transform_process : public bpro1_process 
{
public:

  dbsk2d_gap_transform_process();
  virtual ~dbsk2d_gap_transform_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
  
protected:

private:

};

#endif //dbsk2d_gap_transform_process_h_

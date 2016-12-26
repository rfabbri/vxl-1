//---------------------------------------------------------------------
// This is brcv/trk/dbinfo/pro/dbinfo_imatch_minfo_process.h
//:
// \file
// \brief process to match image snippets based on mutual information
//
// \author
//  J. L. Mundy April 22, 2006
//
// \verbatim
//  Modifications
//   Adapted from dbinfo_mutual_info_process by Ozge Ozcanli
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dbinfo_imatch_minfo_process_h_
#define dbinfo_imatch_minfo_process_h_

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

class dbinfo_imatch_minfo_process : public bpro1_process 
{
public:
  dbinfo_imatch_minfo_process();
  virtual ~dbinfo_imatch_minfo_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Minfo Match Images";
  }

  vcl_vector< vcl_string > get_input_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    to_return.push_back( "image" );
    return to_return;
  }
  vcl_vector< vcl_string > get_output_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    to_return.push_back( "image" );
    to_return.push_back( "image" );
    return to_return;
  }

  int input_frames() {
    return 1;
  }
  int output_frames() {
    return 1;
  }

  bool execute();
  bool finish() {
    return true;
  }

  float get_total_info()        { return total_info_; }

protected:
  float total_info_;
  
private:
  

};

#endif

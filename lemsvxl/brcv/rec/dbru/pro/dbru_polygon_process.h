
#ifndef dbru_polygon_process_h_
#define dbru_polygon_process_h_
//---------------------------------------------------------------------
// This is brcv/rec/dbru/pro/dbru_polygon_process.h
//:
// \file
// \brief process to load polygons and fit livewire polygons
//
// \author
//  O.C. Ozcanli - June 08, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <dbdet/lvwr/dbdet_lvwr.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

class dbru_polygon_process : public bpro1_process 
{
public:
  dbru_polygon_process();
  virtual ~dbru_polygon_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name() {
    return "Livewire Polygons";
  }

  std::vector< std::string > get_input_type() {
    std::vector< std::string > to_return;
    to_return.push_back( "image" );
    return to_return;
  }

  std::vector< std::string > get_output_type() {
    std::vector< std::string > to_return;
    to_return.push_back( "vsol2D" );
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

protected:
  int n_;  // n frames before and n frames after
  std::vector<std::vector<vsol_polygon_2d_sptr> > polygons_;
  std::vector< vsol_spatial_object_2d_sptr > contours_;

  osl_canny_ox_params canny_params_;
  dbdet_lvwr_params iparams_;
  int video_id_;
  std::string poly_filename_;
private:
  

};

#endif





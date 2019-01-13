// This is bmvgd/dbbecl/becld_epiband_builder.h
#ifndef becld_epiband_builder_h
#define becld_epiband_builder_h
//:
//\file
//\brief Code to generate epipolar lines with confidence band
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri.github.io)
//\date Thu Nov  2 23:55:07 EST 2006
//
#include <vector>
#include <becld/becld_epiband.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>

class becld_subpixel_point_set;

// Comments are in the .cxx file
class becld_epiband_builder {
  public:
  static void 
  build_epibands_iteratively(
      bool reinitialize, unsigned v,
      const std::vector<bool> &is_specified,
      const std::vector<vsol_point_2d_sptr> &points,
      std::vector< std::vector<becld_epiband *> > &epband,
      const std::vector <vsol_box_2d_sptr> &bbox_,
      const std::vector< std::vector<vpgl_fundamental_matrix<double> > > &fm_,
      double err_pos
        );
};

#endif // becld_epiband_builder_h

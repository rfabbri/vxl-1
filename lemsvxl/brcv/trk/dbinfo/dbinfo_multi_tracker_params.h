// This is brl/bseg/dbinfo/dbinfo_multi_tracker_params.h
#ifndef dbinfo_multi_tracker_params_h_
#define dbinfo_multi_tracker_params_h_
//:
// \file
// \brief parameter mixin for dbinfo_multi_tracker
//
// \author
//    Joseph L. Mundy - May 09, 2005
//    Brown University
//
//-----------------------------------------------------------------------------
#include <vcl_iosfwd.h>
#include <vcl_iostream.h>
class dbinfo_multi_tracker_params
{
 public:
  dbinfo_multi_tracker_params(const int n_samples = 10,
                           const float search_radius = 5.0,
                           const float angle_range = 0.0,
                           const float scale_range = 0.0,
                           const float sigma = 1.0,
                           const float scorethresh = 1.0,
                           const bool intensity_info = true,
                           const bool gradient_info = true,
                           const bool color_info = false,
                           const float min_gradient = 0,
                           const float min_saturation = 0,
                           const float parzen_sigma = 0,
                           const unsigned int intensity_hist_bins = 16,
                           const unsigned int gradient_dir_hist_bins = 8,
                           const unsigned int color_hist_bins = 8,
                           const bool use_background = false,
                           const bool renyi_joint_entropy = false,
                           const bool save_tracks = false,
                           const bool optimize = false,
                           const bool verbose = false,
                           const bool debug = false);

  dbinfo_multi_tracker_params(const dbinfo_multi_tracker_params& old_params);
 ~dbinfo_multi_tracker_params() {}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream& os,const dbinfo_multi_tracker_params& tp);
 protected:
  void InitParams(int n_samples,
                  float search_radius,
                  float angle_range,
                  float scale_range,
                  float sigma,
                  float scorethresh,
                  bool intensity_info,
                  bool gradient_info,
                  bool color_info,
                  float min_gradient,
                  float min_saturation,
                  float parzen_sigma,
                  unsigned int intensity_hist_bins,
                  unsigned int gradient_dir_hist_bins,
                  unsigned int color_hist_bins,
                  bool use_background,
                  bool renyi_joint_entropy,
                  bool save_tracks,
                  bool optimize,
                  bool verbose,
                  bool debug);
 public:
  //
  // Parameter blocks and parameters
  //
  int n_samples_;           //!< number of samples generated per iteration
  float search_radius_;     //!< window size for generating samples
  float angle_range_;       //!< angle range for generating samples
  float scale_range_;       //!< scale range for generating samples
  float sigma_;             //!< smoothing kernel radius for estimating gradient
  float scorethresh_;
  bool intensity_info_;      //!< process intensity info
  bool gradient_info_;      //!<  process gradient info
  bool color_info_;         //!<  process color info
  float min_gradient_;      //!< minimum gradient magnitude to be considered
  float min_saturation_;      //!< minimum color saturation  to be considered
  float parzen_sigma_;      //!< smoothing for the histogram density
  unsigned int intensity_hist_bins_; //!< number of intensity histogram bins
  unsigned int gradient_dir_hist_bins_;//!< number of grad dir histogram bins
  unsigned int color_hist_bins_;//!< number of color histogram bins
  bool use_background_;     //!< employ a background model
  bool renyi_joint_entropy_;//!< use renyi entropy for joint distributions
  bool save_tracks_;            //!< save the track binary storage class
  bool optimize_;           //!< optimize the transformation using amoeba
  bool verbose_;            //!< informative messages to cout
  bool debug_;              //!< informative debug messages to cout
};

#endif // dbinfo_multi_tracker_params_h_

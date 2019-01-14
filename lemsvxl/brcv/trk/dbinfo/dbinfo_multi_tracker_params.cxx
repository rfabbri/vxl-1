// This is brl/bseg/dbinfo/dbinfo_multi_tracker_params.cxx
#include <dbinfo/dbinfo_multi_tracker_params.h>
//:
// \file
// See dbinfo_multi_tracker_params.h
//
//-----------------------------------------------------------------------------
#include <sstream>

//------------------------------------------------------------------------
// Constructors
//

dbinfo_multi_tracker_params::
dbinfo_multi_tracker_params(const dbinfo_multi_tracker_params& tp){
  InitParams(tp.n_samples_,
             tp.search_radius_,
             tp.angle_range_,
             tp.scale_range_,
             tp.sigma_,
             tp.scorethresh_,
             tp.intensity_info_,
             tp.gradient_info_,
             tp.color_info_,
             tp.min_gradient_,
             tp.min_saturation_,
             tp.parzen_sigma_,
             tp.intensity_hist_bins_,
             tp.gradient_dir_hist_bins_,
             tp.color_hist_bins_,
             tp.use_background_,
             tp.renyi_joint_entropy_,
             tp.save_tracks_,
             tp.optimize_,
             tp.verbose_,
             tp.debug_);
}

dbinfo_multi_tracker_params::
dbinfo_multi_tracker_params(const int n_samples,
                            const float search_radius,
                            const float angle_range,
                            const float scale_range,
                            const float sigma,
                            const float scorethresh,
                            const bool intensity_info,
                            const bool gradient_info,
                            const bool color_info,
                            const float min_gradient,
                            const float min_saturation,
                            const float parzen_sigma,
                            const unsigned int intensity_hist_bins,
                            const unsigned int gradient_dir_hist_bins,
                            const unsigned int color_hist_bins,
                            const bool use_background,
                            const bool renyi_joint_entropy,
                            const bool save_tracks,
                            const bool optimize,
                            const bool verbose,
                            const bool debug)
{
  InitParams(n_samples,
             search_radius,
             angle_range,
             scale_range,
             sigma,
             scorethresh,
             intensity_info,
             gradient_info,
             color_info,
             min_gradient,
             min_saturation,
             parzen_sigma,
             intensity_hist_bins,
             gradient_dir_hist_bins,
             color_hist_bins,
             use_background,
             renyi_joint_entropy,
             save_tracks,
             optimize,
             verbose,
             debug);
}

void dbinfo_multi_tracker_params::InitParams(int n_samples,
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
                                          bool debug)
{
  n_samples_ = n_samples;
  search_radius_ = search_radius;
  angle_range_ = angle_range;
  scale_range_ = scale_range;
  sigma_ = sigma;
  scorethresh_ = scorethresh;
  intensity_info_ = intensity_info;
  gradient_info_ = gradient_info;
  color_info_ = color_info;
  min_gradient_ = min_gradient;
  min_saturation_ = min_saturation;
  parzen_sigma_ = parzen_sigma;
  intensity_hist_bins_=intensity_hist_bins; 
  gradient_dir_hist_bins_=gradient_dir_hist_bins;
  color_hist_bins_=color_hist_bins;
  use_background_ =   use_background;
  renyi_joint_entropy_ = renyi_joint_entropy;
  save_tracks_ = save_tracks;
  optimize_ = optimize;
  verbose_ = verbose;
  debug_ = debug;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    std::endl otherwise.
bool dbinfo_multi_tracker_params::SanityCheck()
{
  std::stringstream msg;
  bool valid = true;

  msg << std::ends;

  return valid;
}

std::ostream& operator << (std::ostream& os, const dbinfo_multi_tracker_params& tp)
{
  return
  os << "dbinfo_multi_tracker_params:" << std::endl << "[---\n"
     << "n_samples " << tp.n_samples_ << std::endl
     << "search_radius " << tp.search_radius_ << std::endl
     << "angle_range " << tp.angle_range_ << std::endl
     << "scale_range " << tp.scale_range_ << std::endl
     << "sigma " << tp.sigma_ << std::endl
     << "scorethresh " << tp.scorethresh_ << std::endl
     << "intensity_info " << tp.intensity_info_ << std::endl
     << "gradient_info " << tp.gradient_info_ << std::endl
     << "color_info " << tp.color_info_ << std::endl
     << "min_gradient " << tp.min_gradient_ << std::endl
     << "parzen_sigma " << tp.parzen_sigma_ << std::endl
     << "n_intensity_bins " << tp.intensity_hist_bins_ << std::endl
     << "n_gradient_dir_bins " << tp.gradient_dir_hist_bins_ << std::endl
     << "n_color_bins " << tp.color_hist_bins_ << std::endl
     << "use background model " << tp.use_background_ << std::endl
     << "renyi joint entropy " << tp.renyi_joint_entropy_ << std::endl
     << "save tracks " << tp.save_tracks_ << std::endl
     << "optimize (aoemba) " << tp.optimize_ << std::endl
     << "verbose " << tp.verbose_ << std::endl
     << "debug " << tp.debug_ << std::endl
     << "---]" << std::endl;
}

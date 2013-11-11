// This is brcv/seg/dbdet/dbdet_scale_space_peaks.h
#ifndef dbdet_scale_space_peaks_h_
#define dbdet_scale_space_peaks_h_
//:
// \file
// \brief Find peaks in a scale space image
//   This peak detection in image pyramids is described in
//  \verbatim
//   "Distinctive Image Features from Scale-Invariant Keypoints"
//    David G. Lowe,
//    International Journal of Computer Vision, 60, 2 (2004), pp. 91-110.
//  \endverbatim
//   This file contains a function to find the peaks in a scale space
//   representation an image.  Typically the difference of gaussians (DoG)
//   is used.
//
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date Sat Nov 8 2003
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <bil/algo/bil_scale_image.h>
#include <vgl/vgl_point_3d.h>

//: Find the peaks in a scale space
// \param scale_image is the scale space to search
// \param peak_pts is the vector of points in which results are populated.
// \param curve_ratio is the threshold on the maximum ratio of principle curvatures.
//                    This helps to reduce the response of edges and find more
//                    corner-like points.
// \param contrast_thresh is the minimum contrast allow (used to prune weak peaks)
void dbdet_scale_space_peaks(const bil_scale_image<float>& scale_image,
                             vcl_vector<vgl_point_3d<float> >& peak_pts,
                             float curve_ratio = 10.0f,
                             float contrast_thresh = 0.03f);

                             
//: The parameters for computing orientations on scale space points
struct dbdet_ssp_orientation_params
{
  //: Constructor using a scale space image
  dbdet_ssp_orientation_params(const bil_scale_image<float>& scale_image,
                               unsigned int bins=36, float sig=1.5f, float thresh=0.8f);
                               
  //: Constructor using precomputed gradients
  dbdet_ssp_orientation_params(const bil_scale_image<float>& si_grad_dir,
                               const bil_scale_image<float>& si_grad_mag,
                               unsigned int bins=36, float sig=1.5f, float thresh=0.8f);
                               
  //: scale image of gradient directions
  bil_scale_image<float> scale_grad_dir;
  //: scale image of gradient magnitudes
  bil_scale_image<float> scale_grad_mag;
  //: the threshold for finding secondary peaks relative to primary peaks
  float peak_thresh;
  //: controls the size of the neighborhood to consider
  float sigma;
  //: the number of bins in the orientation histogram
  unsigned int num_bins;
};

   
//: Compute the peak orientations of a scale space point
vcl_vector<float>
dbdet_ssp_orientations(vgl_point_3d<float> ssp,
                       const dbdet_ssp_orientation_params& params);

                       
#endif // dbdet_scale_space_peaks_h_

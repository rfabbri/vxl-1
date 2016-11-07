// This is contrib/mleotta/vidreg/vidreg_detector.h
#ifndef vidreg_detector_h_
#define vidreg_detector_h_

//:
// \file
// \brief A detector object to produce edgel and point features
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 7/19/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <gevd/gevd_bufferxy.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <rgrl/rgrl_feature_sptr.h>

#include <vidreg/vidreg_feature_group.h>

//: A detector object to produce edgel and point features
class vidreg_detector
{
  public:
    //: Constructor
    vidreg_detector();
    //: Destructor
    ~vidreg_detector();

    //: Detect both edgel and corner features
    bool detect_features(const vil_image_resource_sptr& image);

    //: Detect edgel features only
    bool detect_edgels(const vil_image_resource_sptr& image);

    //: Return the detected features
    vidreg_feature_group_sptr features() const { return features_; }

    //: Remove the smart pointer to the current set of features
    void clear_features() { features_ = NULL; }

  protected:
    //: Detect corner features only
    //  can not be called by itself, must follow detect_edgels
    bool detect_corners(const vil_image_resource_sptr& image);

    vcl_vector<double> orient_point(double x, double y);

    void make_descriptor(double x, double y, vnl_vector<double>& desc, double max_comp = 0.2);

  private:
    //: Make sure buf is the right size and wrap the memory in view
    void make_valid_image(gevd_bufferxy*& buf, vil_image_view<float>& view,
                          int ni, int nj);

    vil_image_view<float> smooth, work, vdx, vdy, vmag, vedgel, corners;
    vil_image_view<vxl_byte>  vorient;
    gevd_bufferxy *edgel, *dx, *dy, *mag, *direction, *locationx, *locationy;
    vidreg_feature_group_sptr features_;

    double point_sigma_;
    double peak_thresh_;
    unsigned max_pts_to_describe_;
    vxl_byte num_bins_;
};


#endif // vidreg_detector_h_

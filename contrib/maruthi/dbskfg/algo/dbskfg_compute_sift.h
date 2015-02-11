// This is a class to represent 
#ifndef dbskfg_compute_sift_h_
#define dbskfg_compute_sift_h_

//:
// \file
// \brief A class to hold the sift parameters
//  
// \author Maruthi Narayanan
// \date  12/10/13
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgl/vgl_point_2d.h>
#include <vil/vil_image_view.h>
#include <vl/sift.h>
#include <vnl/vnl_vector.h>

class dbskfg_compute_sift
{

public:

    // Constructor
    dbskfg_compute_sift(
        vgl_point_2d<double> model_pt,
        double model_radius,
        double model_theta,
        double model_scale_ratio,
        VlSiftFilt* model_sift_filter,
        vgl_point_2d<double> query_pt,
        double query_radius,
        double query_theta,
        double query_scale_ratio,
        VlSiftFilt* query_sift_filter,
        vl_sift_pix* model_red_grad_data,
        vl_sift_pix* query_red_grad_data,
        vl_sift_pix* model_green_grad_data,
        vl_sift_pix* query_green_grad_data,
        vl_sift_pix* model_blue_grad_data,
        vl_sift_pix* query_blue_grad_data);
        
    // Destructor
    ~dbskfg_compute_sift()
    {
        
    };

    // Compute descriptors
    void compute_descriptors(
        vgl_point_2d<double> pt,
        double radius,
        double theta,
        VlSiftFilt* filter,
        double scale_ratio,
        vil_image_view<vl_sift_pix>& red_grad_mod,
        vil_image_view<vl_sift_pix>& red_grad_angle,
        vil_image_view<vl_sift_pix>& green_grad_mod,
        vil_image_view<vl_sift_pix>& green_grad_angle,
        vil_image_view<vl_sift_pix>& blue_grad_mod,
        vil_image_view<vl_sift_pix>& blue_grad_angle,
        vnl_vector<vl_sift_pix>& output);

    double distance(){return distance_;}

private:

    // Model pt
    vgl_point_2d<double> model_pt_;
    
    // Model radius
    double model_radius_;

    // Model theta
    double model_theta_;

    // Model scale ratio
    double model_scale_ratio_;

    // Model Filter 
    VlSiftFilt* model_sift_filter_;

    // Query pt
    vgl_point_2d<double> query_pt_;
    
    // Query radius
    double query_radius_;

    // Query theta
    double query_theta_;

    // Query scale ratio
    double query_scale_ratio_;

    // Query Filter 
    VlSiftFilt* query_sift_filter_;

    // Gradient data

    vil_image_view<vl_sift_pix> model_red_grad_mod_;
    vil_image_view<vl_sift_pix> model_red_grad_angle_;

    vil_image_view<vl_sift_pix> model_green_grad_mod_;
    vil_image_view<vl_sift_pix> model_green_grad_angle_;

    vil_image_view<vl_sift_pix> model_blue_grad_mod_;
    vil_image_view<vl_sift_pix> model_blue_grad_angle_;

    vil_image_view<vl_sift_pix> query_red_grad_mod_;
    vil_image_view<vl_sift_pix> query_red_grad_angle_;

    vil_image_view<vl_sift_pix> query_green_grad_mod_;
    vil_image_view<vl_sift_pix> query_green_grad_angle_;

    vil_image_view<vl_sift_pix> query_blue_grad_mod_;
    vil_image_view<vl_sift_pix> query_blue_grad_angle_;

    // Final distance
    double distance_;

    // Copy ctor private
    dbskfg_compute_sift(const dbskfg_compute_sift& );
    
    // Make assigned operate private
    const dbskfg_compute_sift& operator=(const dbskfg_compute_sift& );
};

#endif // dbskfg_compute_sift_h_



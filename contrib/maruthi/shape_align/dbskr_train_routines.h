// This is brcv/shp/dbskfg/algo/dbskr_train_routines.h
#ifndef dbskr_train_routines_h_
#define dbskr_train_routines_h_
//:
// \file
// \brief Algorithm to train for bag of words
//
// \author Maruthi Narayanan
// \date 08/07/2015
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 08/07/2015    Initial version.
//
// \endverbatim 

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_polygon.h>

extern "C" {
#include <vl/sift.h>
}

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>

#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_vector.h>

//: Form Composite Graph algorithm
class dbskr_train_routines
{

public:

    //: Enum
    enum DescriptorType
    {
        GRADIENT=0,
        COLOR
    };

    //: Enum
    enum ColorSpace
    {
        RGB=0,
        OPP,
        NOPP,
        LAB
    };

    //: Constructor
    dbskr_train_routines(vcl_string model_filename,
                         DescriptorType descr_type,
                         ColorSpace color_space,
                         int keywords=1024,
                         int pca=128,
                         int stride=8);

    //: Destructor
    ~dbskr_train_routines();

    //: Write out data
    void write_out();

private:

    // Descritor type
    DescriptorType descr_type_;

    // Color space
    ColorSpace color_space_;

    // keywords for gmm
    int keywords_;

    // int pca dimensitoinaly reduction
    int pca_;

    // int stride
    int stride_;

    // Keep track of masks per image
    vcl_vector<vgl_polygon<double> > masks_;

    // Keep track of original files
    vcl_vector<vil_image_view<double> > model_chan_1_;
    vcl_vector<vil_image_view<double> > model_chan_2_;
    vcl_vector<vil_image_view<double> > model_chan_3_;

    // Keep track of gradients
    vcl_vector<vl_sift_pix* > grad_chan_1_;
    vcl_vector<vl_sift_pix* > grad_chan_2_;
    vcl_vector<vl_sift_pix* > grad_chan_3_;

    // Keep all descriptors
    vnl_matrix<vl_sift_pix> descriptor_matrix_;

    // Keep pca data
    vnl_matrix<vl_sift_pix> PCA_M_;
    vnl_vector<vl_sift_pix> PCA_mean_;

    //: Match
    void train(vcl_string& gmm_filename);

    // Load model file
    void load_model_file(vcl_string& filename);

    vnl_vector<vl_sift_pix> linear_embed(vnl_vector<vl_sift_pix>& descr)
    {
        vnl_vector<vl_sift_pix> zero_mean=descr-PCA_mean_;
        
        return zero_mean*PCA_M_; 
        
    }

    void convert_to_color_space(
        vil_image_resource_sptr& input_image,
        vil_image_view<double>& o1,
        vil_image_view<double>& o2,
        vil_image_view<double>& o3,
        ColorSpace color_space);

    // Compute boundary 
    vgl_polygon<double> compute_boundary(
        dbsk2d_shock_graph_sptr& sg);

    void compute_grad_descriptors();
    
    void compute_gradients();

    void compute_pca(vcl_string& M_filename,vcl_string& mean_filename);

    void compute_sift_descr(
        vgl_point_2d<double>& pt,
        double& radius,
        double& theta,
        vl_sift_pix* red_grad_data,
        vl_sift_pix* green_grad_data,
        vl_sift_pix* blue_grad_data,
        VlSiftFilt* model_filter,
        vnl_vector<vl_sift_pix>& descriptor);

    void compute_grad_color_maps(vil_image_view<double>& orig_image,
                                 vl_sift_pix** grad_data,
                                 vgl_polygon<double>& poly,
                                 bool mask_poly=true,
                                 bool fliplr=false);

    // Make copy ctor private
    dbskr_train_routines(const dbskr_train_routines&);

    // Make assign operator private
    dbskr_train_routines& operator
        =(const dbskr_train_routines& );

};

#endif //dbsk2d_ishock_prune_h_

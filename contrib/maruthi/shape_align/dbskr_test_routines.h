// This is brcv/shp/dbskfg/algo/dbskr_test_routines.h
#ifndef dbskr_test_routines_h_
#define dbskr_test_routines_h_
//:
// \file
// \brief Algorithm to test for bag of words
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
#include <vcl_utility.h>

//: Form Composite Graph algorithm
class dbskr_test_routines
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

    //: Enum
    enum TaskSet
    {
        MODEL=0,
        QUERY
    };

    //: Constructor
    dbskr_test_routines(
        vcl_string query_esf_file,
        vcl_string query_dc_file,
        vcl_string model_filename,
        vcl_string gmm_file,
        vcl_string pca_M_file,
        vcl_string pca_mean_file,
        DescriptorType descr_type,
        ColorSpace color_space,
        int stride,
        double powernorm,
        bool write_out);
    
    //: Destructor
    ~dbskr_test_routines();
    
    //: perform test
    void test();
private:

    // Keep track of size of query image size
    int query_ni_;
    int query_nj_;

    // Descritor type
    DescriptorType descr_type_;

    // Color space
    ColorSpace color_space_;

    // keywords for gmm
    int keywords_;

    // Bool writeout
    bool write_out_;

    // Keep track of masks per image
    vcl_vector<vgl_polygon<double> > model_masks_;

    // Keep track of masks per image
    vcl_vector<vgl_polygon<double> > query_masks_;

    // Keep track of original files
    vcl_vector<vil_image_view<double> > model_chan_1_;
    vcl_vector<vil_image_view<double> > model_chan_2_;
    vcl_vector<vil_image_view<double> > model_chan_3_;

    // Keep track of original files
    vcl_vector<vil_image_view<double> > query_chan_1_;
    vcl_vector<vil_image_view<double> > query_chan_2_;
    vcl_vector<vil_image_view<double> > query_chan_3_;
    
    /* // Keep track of gradients */
    /* vcl_vector<vl_sift_pix* > model_grad_chan_1_; */
    /* vcl_vector<vl_sift_pix* > model_grad_chan_2_; */
    /* vcl_vector<vl_sift_pix* > model_grad_chan_3_; */

    // Keep track of gradients
    vcl_vector<vl_sift_pix* > query_grad_chan_1_;
    vcl_vector<vl_sift_pix* > query_grad_chan_2_;
    vcl_vector<vl_sift_pix* > query_grad_chan_3_;

    // Keep all descriptors
    vnl_matrix<vl_sift_pix> descriptor_matrix_;

    // Keep pca data
    vnl_matrix<vl_sift_pix> PCA_M_;
    vnl_vector<vl_sift_pix> PCA_mean_;

    // Keep track of gmm data
    float* means_cg_;
    float* covariances_cg_;
    float* priors_cg_;

    // Keep track of dense correspondence
    vcl_vector<vcl_vector<vcl_pair<float,float> > > query_points_;
   
    // Keep track of query test points
    vcl_vector<vcl_vector<vgl_point_2d<double> > > query_test_points_;

    // Keep track of model points
    vcl_vector< vcl_vector< vcl_vector<vcl_pair<float,float> > > >
        model_points_;

    // Keep track of stride
    int stride_;

    // Keep track of powernorm
    double powernorm_;

    // Load model file and train data
    void load_dc_file(vcl_string& filename);
    void load_query_file(vcl_string& filename);
    void load_model_file(vcl_string& filename);
    void load_pca_data(vcl_string& M_filename,vcl_string& mean_filename);
    void load_gmm_data(vcl_string& gmm_filename);

    vnl_vector<vl_sift_pix> linear_embed(vnl_vector<vl_sift_pix>& descr)
    {
        vnl_vector<vl_sift_pix> zero_mean=descr-PCA_mean_;

        return zero_mean*PCA_M_; 
        
    }

    void explicit_alignment(
        vcl_vector<vcl_pair<float,float> >& q_pts, 
        vcl_vector<vcl_pair<float,float> >& m_pts,
        vil_image_view<double>& model_chan_1,
        vil_image_view<double>& model_chan_2,
        vil_image_view<double>& model_chan_3,
        vil_image_view<double>& mapped_img);

    void convert_to_color_space(
        vil_image_resource_sptr& input_image,
        vil_image_view<double>& o1,
        vil_image_view<double>& o2,
        vil_image_view<double>& o3,
        ColorSpace color_space);

    // Compute boundary 
    vgl_polygon<double> compute_boundary(
        dbsk2d_shock_graph_sptr& sg);

    void compute_query_gradients();

    void compute_query_test_points();

    void compute_fvs(
        vcl_vector<vgl_point_2d<double> >& stride_points,
        vl_sift_pix* chan1_grad_data,
        vl_sift_pix* chan2_grad_data,
        vl_sift_pix* chan3_grad_data,
        VlSiftFilt* sift_filter,
        vnl_matrix<vl_sift_pix>& descriptor_matrix);
    

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
    dbskr_test_routines(const dbskr_test_routines&);

    // Make assign operator private
    dbskr_test_routines& operator
        =(const dbskr_test_routines& );

};

#endif //dbsk2d_ishock_prune_h_

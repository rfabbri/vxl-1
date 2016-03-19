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

#include <vil/vil_image_view.h>
#include <vgl/vgl_polygon.h>

#include <vl/sift.h>

#include <vnl/vnl_vector.h>

#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_vector.h>

//: Form Composite Graph algorithm
class dbskr_train_routines
{

public:

    //: Constructor
    dbskr_train_routines(vcl_string model_filename,
                         int bag_of_words_sift=1024,
                         int bag_of_words_color=1024);

    //: Destructor
    ~dbskr_train_routines();

    //: Match
    void train();

private:

    // bag of words grad
    int sift_words_;

    // bag of words color
    int color_words_;

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

    // Load model file
    void load_model_file(vcl_string& filename);

    // Compute boundary 
    vgl_polygon<double> compute_boundary(
        dbsk2d_shock_graph_sptr& sg);

    void compute_descriptors();
    
    void compute_gradients();

    void compute_descr(
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

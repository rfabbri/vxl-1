// This is brcv/shp/dbsk2d/algo/dbsk2d_transform_manager.h
#ifndef dbsk2d_transform_manager_h_
#define dbsk2d_transform_manager_h_
//:
// \file
// \brief Algorithm to detect transforms 
// \author Maruthi Narayanan
// \date 07/08/10
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 07/08/2010    Initial version.
//
// \endverbatim 


#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil3d/vil3d_image_view.h>
#include <vcl_string.h>
#include <vgl/vgl_polygon.h>
#include <dbsk2d/dbsk2d_bnd_contour_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <dbsk2d/dbsk2d_ishock_bpoint.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <vgl/vgl_distance.h>
#include <vcl_set.h>
#include <bbas/bil/algo/bil_color_conversions.h>

class dbsk2d_ishock_belm;


//: Form Composite Graph algorithm
class dbsk2d_transform_manager 
{

public:

    // Set singleton
    static dbsk2d_transform_manager& Instance()
    {
        static dbsk2d_transform_manager manager;
        return manager;
    }
  
    // ************************ Setters/Getters *******************************

    // Set image
    void set_image(vil_image_resource_sptr image)
    {
        image_=image;

        L_img_.set_size(image_->ni(),image_->nj());
        a_img_.set_size(image_->ni(),image_->nj());
        b_img_.set_size(image_->ni(),image_->nj());
        

        convert_RGB_to_Lab(image_->get_view(),
                           L_img_,
                           a_img_,
                           b_img_);

    }

    // Set threshold cost
    void set_threshold(double threshold){threshold_ = threshold;}

    // Set output folder
    void set_output_frag_folder(vcl_string folder){out_folder_= folder;}

    // Set output prefix
    void set_output_prefix(vcl_string prefix){out_prefix_=prefix;}

    // Set prob normalization
    void set_normalization(double normalization){normalization_=normalization;}

    // Start binary file output
    void start_binary_file(vcl_string binary_file_output);

    // Start binary file output
    void start_region_file(vcl_string binary_file_output);

    // Start region stats file
    void start_region_stats_file(vcl_string binary_file_output)
    {output_region_stats_file_=binary_file_output;}

    // Get image
    vil_image_resource_sptr get_image()
    {return image_;}

    void get_appearance_stats(vcl_vector<dbsk2d_ishock_edge*>& region,
                              vcl_vector<dbsk2d_ishock_belm*>& belms,
                              double area,
                              vcl_vector<double>& app_stats);

    // Get threshold cost
    double get_threshold(){return threshold_;}

    // Get extra belms
    void get_extra_belms(vcl_vector<dbsk2d_ishock_belm*>& region,
                         vcl_set<int>& key,
                         vcl_set<int>& closed_region_key,
                         vcl_map<int,dbsk2d_ishock_bline*>& final_belms);

    // Get output folder
    vcl_string get_output_frag_folder(){return out_folder_;}

    // Get output prefix
    vcl_string get_output_prefix(){return out_prefix_;}

    // Get output stream
    void write_output_polygon(vgl_polygon<double>& poly);
 
    // Write output region stats
    void write_output_region_stats(vcl_vector<double>& region_stats);

    // Get output stream
    void write_output_region(vcl_vector<dbsk2d_ishock_belm*>& region);

    // Get output stream
    void write_output_region(vcl_vector<dbsk2d_bnd_contour_sptr>& contours,
                             vcl_vector<vgl_point_2d<double> >& gap_filler);

    // Get output stream
    void write_output_polygon(vcl_vector<dbsk2d_bnd_contour_sptr>& contours,
                              vcl_vector<vgl_point_2d<double> >& gap_filler);

    // Get output stream
    void write_output_region(vgl_polygon<double>& poly);

    // Save individual fragment to image
    void save_image_poly(vgl_polygon<double>& poly,
                         vcl_string filename);

    // Destroy singleton 
    void destroy_singleton();

    // Read in training data
    void read_in_training_data(vcl_string filename);

    // Read in training data
    void read_in_gpb_data(vcl_string filename);

    // Read in texton data
    void read_in_texton_data(vcl_string filename);

    // Set beta0 for logitic function
    void set_beta0_logit(double beta0){logistic_beta0_ = beta0;}

    // Set beta1 for logitic function
    void set_beta1_logit(double beta1){logistic_beta1_ = beta1;}

    // Determine gap cost
    double transform_probability(double gamma_norm,double k0_norm, 
                                 double length);

    // Determine gap cost
    double transform_probability(vcl_vector<vgl_point_2d<double> >& curve);

    // Determine gap cost
    double transform_probability( vsol_polyline_2d_sptr& curve);

    //: increment the id counter and return new id
    unsigned int nextAvailableID() { id_++; return id_; }

    //: compute stats closed polygon
    void write_stats_closed(vgl_polygon<double>& closed);

    //: compute stats closed polygon
    void write_stats_closed(vcl_vector<dbsk2d_ishock_belm*>& belms);

    // : get closest point
    dbsk2d_ishock_bpoint* get_anchor_pt(vcl_pair<dbsk2d_ishock_bpoint*,
                                        dbsk2d_ishock_bline*>& pair );

    //: see if gpb data loaded
    bool gPb_loaded()
    {
	if ( gPb_image_.ni() )
	{
	    return true;
	    
	}
	else
	{
	    return false;
	}
    }
    
private:

    //: store image
    vil_image_resource_sptr image_; 
 
    // Keep threshold for transforms
    double threshold_;

    // Keep a 3d volume of distances from separation surface
    vil3d_image_view<double> dist_volume_;

    // Keeps a 2d image for holding gpb values
    vil_image_view<double> gPb_image_;

    // Keeps a 2d image for holding gpb values
    vil_image_view<double> texton_image_;

    // Keep output folder for fragments
    vcl_string out_folder_;

    // Keep output prefix for fragments
    vcl_string out_prefix_;

    // Keep binary file
    vcl_string output_binary_file_;

    // Keep binary file
    vcl_string output_region_file_;

    // Keep binary file
    vcl_string output_region_stats_file_;

    //: First Coefficient of logistic function 
    double logistic_beta0_;

    //: Second Coefficient of logistic function 
    double logistic_beta1_;
    
    //: Keep next available id
    unsigned int id_;

    //: Keep track of normalization
    double normalization_;
    
    // Keep track of a LAB channels
    vil_image_view<double> L_img_;

    // A channel
    vil_image_view<double> a_img_;

    // B channel
    vil_image_view<double> b_img_;

    // get internal grid points
    void grid_points(vcl_vector<dbsk2d_ishock_edge*>& region,
                     vcl_vector<dbsk2d_ishock_belm*>& belms,
                     vcl_vector<vgl_point_2d<double> >& foreground_grid,
                     vcl_vector<vgl_point_2d<double> >& background_grid);

    // edge gpb value
    double contour_gpb_value(vcl_vector<dbsk2d_ishock_belm*>& frag_belms);
    
    // area gpb value
    double region_gpb_value(vcl_vector<vgl_point_2d<double> >& grid);

    // chi squared distance
    double chi_squared_color_distance(
        vcl_vector<vgl_point_2d<double> >& foreground,
        vcl_vector<vgl_point_2d<double> >& background,
        vil_image_view<double>& channel,
        double min, double max,unsigned int nbins,bool flip=false);

    // compute average LAB difference
    double mean_LAB_distance(        
        vcl_vector<vgl_point_2d<double> >& foreground,
        vcl_vector<vgl_point_2d<double> >& background);

    bool gap_endpoint(dbsk2d_ishock_bpoint* bp);

    void ellipse_fitting(  
        vcl_vector<vgl_point_2d<double> >& foreground,
        vcl_vector<double>& stats);


    // Make default constructor private
    dbsk2d_transform_manager();

    // Make default dtor private
    ~dbsk2d_transform_manager();

    // Make copy ctor private
    dbsk2d_transform_manager(const dbsk2d_transform_manager&);

    // Make assign operator private
    dbsk2d_transform_manager& operator
        =(const dbsk2d_transform_manager& );
   
};

#endif //dbsk2d_ishock_prune_h_

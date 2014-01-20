// This is brcv/shp/dbskfg/algo/dbskfg_match_bag_of_fragments.h
#ifndef dbskfg_match_bag_of_fragments_h_
#define dbskfg_match_bag_of_fragments_h_
//:
// \file
// \brief Algorithm to extract fragments out of an image 
// \author Maruthi Narayanan
// \date 10/18/10
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 10/18/2010    Initial version.
//
// \endverbatim 

#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_vector.h>
#include <vcl_set.h>
#include <vnl/vnl_matrix.h>

#include <dbskfg/algo/dbskfg_cgraph_directed_tree_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

#include <vil/vil_image_resource_sptr.h>
#include <vl/sift.h>

#include <dbskr/dbskr_scurve.h>
#include <vcl_utility.h>
#include <dbskr/dbskr_edit_distance_base.h>

#include <dbskfg/algo/dbskfg_sift_data.h>

#include <vsol/vsol_box_2d.h>

//: Form Composite Graph algorithm
class dbskfg_match_bag_of_fragments
{

public:

    //: Constructor
    dbskfg_match_bag_of_fragments
        ( vcl_string model_file,
          vcl_string query_file,
          vcl_string output_file,
          bool elastic_splice_cost    = false, 
          float scurve_sample_ds      = 0.0f, 
          float scurve_interpolate_ds = 1.0f, 
          bool localized_edit         = false,
          double scurve_matching_R    = 6.0f, 
          bool circular_ends          = false, 
          bool combined_edit          = false, 
          bool use_approx             = true,
          bool scale_bbox             = false,
          bool scale_root             = false,
          bool app_sift               = false,
          vil_image_resource_sptr model_image=0,
          vil_image_resource_sptr query_image=0);

    //: Destructor
    ~dbskfg_match_bag_of_fragments();

    //: Extract the fragments
    bool match();

    //: Match the fragments
    bool binary_match();

    //: Match the fragments
    bool binary_app_match();

    //: Match the fragments
    bool binary_scale_match();

private:

    // Keep track of model trees
    vcl_map<unsigned int,vcl_pair<vcl_string,
        dbskfg_cgraph_directed_tree_sptr> > 
        model_trees_;

    // Keep track of query trees
    vcl_map<unsigned int,vcl_pair<vcl_string,
        dbskfg_cgraph_directed_tree_sptr> > 
        query_trees_;
     
    // Keep a map of all model patches
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> > 
        model_fragments_;

    // Keep a map of all query patches
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> > 
        query_fragments_;

    // Keep a map of all model contours
    vcl_map<unsigned int,vcl_vector< vsol_spatial_object_2d_sptr > >
        model_contours_;

    // Keep a map of all model contours
    vcl_map<unsigned int,vcl_vector< vsol_spatial_object_2d_sptr > >
        query_contours_;
    
    // Keep track of model contour ids
    vcl_map<unsigned int,vcl_set<unsigned int> > model_con_ids_;

    // Keep track of query contour ids
    vcl_map<unsigned int,vcl_set<unsigned int> > query_con_ids_;

    // Keep track of model image size
    vcl_pair<unsigned int,unsigned int> model_image_size_;
    
    // Keep track of query image size
    vcl_pair<unsigned int,unsigned int> query_image_size_;

    // Keep track of gradient image data
    vl_sift_pix* model_grad_data_;

    // Keep track of gradient color image data
    vl_sift_pix* model_grad_red_data_;

    // Keep track of gradient color image data
    vl_sift_pix* model_grad_green_data_;

    // Keep track of gradient color image data
    vl_sift_pix* model_grad_blue_data_;
    
    // Keep track of query image data
    vl_sift_pix* query_grad_data_;

    // Keep track of gradient color image data
    vl_sift_pix* query_grad_red_data_;

    // Keep track of gradient color image data
    vl_sift_pix* query_grad_green_data_;

    // Keep track of gradient color image data
    vl_sift_pix* query_grad_blue_data_;
    
    // Keep track of model root filters
    VlSiftFilt* model_sift_filter_;
  
    // Keep track of query root filer
    VlSiftFilt* query_sift_filter_;
    
    vil_image_resource_sptr model_image_;
    
    vil_image_resource_sptr query_image_;

    // Create a map of all cost
    vcl_vector<vcl_map<double,vcl_pair<unsigned int,unsigned int> > > 
        sim_matrix_;

    // Holds shape data
    vnl_matrix<double> binary_sim_matrix_;

    // Holds shape data normalized by length
    vnl_matrix<double> binary_sim_length_matrix_;

    // Holds appearance data
    vnl_matrix<double> binary_app_sim_matrix_;

    // Holds normalization constant
    vnl_matrix<double> binary_app_norm_sim_matrix_;

    // Holds normalization constant
    vnl_matrix<double> binary_app_rgb_sim_matrix_;

    // Keep output file
    vcl_string output_match_file_;
    vcl_string output_html_file_;
    vcl_string output_binary_file_;
    vcl_string output_removed_regions_;

    // Amirs elastic splice cost 
    bool elastic_splice_cost_;

    // Sample ds to reconstruct curve
    float scurve_sample_ds_;

    // Interpolate ds
    float scurve_interpolate_ds_;

    // Localized edit not really used
    bool localized_edit_;

    // Curve matching r that weighs bending over stretching
    double scurve_matching_R_; 

    // Use circular ends in completion
    bool circular_ends_;

    // Combined edit during shock matching
    bool combined_edit_;

    // Use approximation for edit distance
    bool use_approx_;

    // Use scale version of edit distance
    bool scale_bbox_;

    // Use radius root node distance
    bool scale_root_;

    // Use appearance 
    bool app_sift_;

    void load_model(vcl_string model_dir);
    void load_query(vcl_string query_dir);

    void load_binary_model(vcl_string model_dir);
    void load_binary_query(vcl_string query_dir);

    void create_html_match_file();
    void write_out_sim_matrix();

    // Load a composite graph
    dbskfg_composite_graph_sptr load_composite_graph(vcl_string filename);

    //Match two graphs
    double match_two_graphs(dbskfg_composite_graph_sptr& model,
                            dbskfg_composite_graph_sptr& query,
                            unsigned int root_scheme = 0);

    //Match two graphs
    void match_two_graphs(dbskfg_cgraph_directed_tree_sptr& model_tree,
                          dbskfg_cgraph_directed_tree_sptr& query_tree,
                          double& norm_shape_cost,
                          double& norm_shape_length,
                          double& app_diff,
                          double& norm_app_cost,
                          double& rgb_avg_cost);
    
    void write_binary_fragments(vcl_ofstream& binary_sim_file,
                                vcl_map<unsigned int,
                                vcl_pair<vcl_string,
                                dbskfg_composite_graph_sptr> >& fragments );

    void compute_grad_maps(vil_image_resource_sptr& input_image,
                           vl_sift_pix** grad_data,
                           VlSiftFilt** filter);

    void compute_grad_color_maps(vil_image_resource_sptr& input_image,
                                 vl_sift_pix** grad_data,
                                 unsigned int channel);


    vcl_pair<double,double> compute_sift_cost(
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        vcl_vector< pathtable_key >& path_map,
        bool flag=false);

    vcl_pair<double,double> compute_rgb_sift_cost(
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        vcl_vector< pathtable_key >& path_map,
        bool flag=false);
    
    vnl_vector<double> compute_second_order_pooling(
        vcl_map<int,vcl_vector<dbskfg_sift_data> >& fragments,
        vl_sift_pix* grad_data,
        VlSiftFilt* filter,
        vsol_box_2d_sptr& bbox);

    // Make copy ctor private
    dbskfg_match_bag_of_fragments(const dbskfg_match_bag_of_fragments&);

    // Make assign operator private
    dbskfg_match_bag_of_fragments& operator
        =(const dbskfg_match_bag_of_fragments& );

};

#endif //dbsk2d_ishock_prune_h_

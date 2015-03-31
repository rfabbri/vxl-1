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
#include <vl/mathop.h>
#include <vl/kdtree.h>

#include <dbskr/dbskr_scurve.h>
#include <vcl_utility.h>
#include <dbskr/dbskr_edit_distance_base.h>

#include <dbskfg/algo/dbskfg_sift_data.h>

#include <vsol/vsol_box_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vil/vil_image_view.h>


//: Form Composite Graph algorithm
class dbskfg_match_bag_of_fragments
{

public:

    //: Enum
    enum ShapeAlgorithmArea
    {
        SCALE_TO_REF=0,
        SCALE_TO_MEAN,
        SCALE_TO_MAX,
        SCALE_TO_MIN
    };

    //: Enum
    enum GradColorSpace
    {
        RGB=0,
        OPP,
        NOPP,
        LAB_2
    };

    //: Enum
    enum RawColorSpace
    {
        LAB=0,
        HSV,
        RGB_2,
        OPP_2
    };

    //: Enum
    enum LabBinType
    {
        DEFAULT=0,
        SPHERE
    };

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
          bool scale_area             = false,
          bool scale_length           = false,
          bool app_sift               = false,
          bool mirror                 = false,
          bool outside_shock          = false,
          bool mask_grad              = false,
          double area_weight          = 0.0f,
          double ref_area             = 10000.0f,
          ShapeAlgorithmArea shape_alg= SCALE_TO_MEAN,
          GradColorSpace  grad_color_space = OPP,
          RawColorSpace   raw_color_space = LAB,
          vil_image_resource_sptr model_image=0,
          vil_image_resource_sptr query_image=0,
          vcl_string model_image_path="");

    //: Destructor
    ~dbskfg_match_bag_of_fragments();

    //: Extract the fragments
    bool match();

    //: Match the fragments
    bool binary_match();

    //: Match the fragments
    bool binary_debug_match();

    //: Match the fragments
    bool binary_app_match();

    //: Match the fragments
    bool binary_scale_match();

    //: Match the fragments
    bool binary_scale_root_match();

    //: train bag of words computation
    bool train_bag_of_words(int keywords);
 
    //: upload training
    void set_bow_train(vcl_string& file_path);

    //: upload training
    void set_part_file(vcl_string& file_path);

    //: binary scale to mean shape 
    bool binary_scale_mean_shape();

    //: Match the fragments
    bool binary_scale_root_debug_match();

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

    // Keep a map of all model patches
    vcl_map<unsigned int,vcl_pair<vcl_string,double> > 
        model_fragments_area_;

    // Keep a map of all model patches
    vcl_map<unsigned int,vcl_pair<vcl_string,double> > 
        model_fragments_length_;

    // Keep a map of all model patches
    vcl_map<unsigned int,vcl_pair<vcl_string,vgl_polygon<double> > > 
        model_fragments_polys_;

    // Keep a map of all query patches
    vcl_map<unsigned int,vcl_pair<vcl_string,double> > 
        query_fragments_area_;

    // Keep a map of all query patches
    vcl_map<unsigned int,vcl_pair<vcl_string,double> > 
        query_fragments_length_;

    // Keep a map of all query patches
    vcl_map<unsigned int,vcl_pair<vcl_string,vgl_polygon<double> > > 
        query_fragments_polys_;

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

    // Keep track of mulitple model grad data
    vcl_map<vcl_string,VlSiftFilt*> model_images_sift_filter_;

    // Keep track of mulitple model grad data
    vcl_map<vcl_string,vl_sift_pix*> model_images_grad_data_;

    // Keep track of mulitple model grad data
    vcl_map<vcl_string,vl_sift_pix*> model_images_grad_data_red_;

    // Keep track of mulitple model grad data
    vcl_map<vcl_string,vl_sift_pix*> model_images_grad_data_blue_;

    // Keep track of mulitple model grad data
    vcl_map<vcl_string,vl_sift_pix*> model_images_grad_data_green_;

    // Keep track of mulitple model grad data
    vcl_map<vcl_string,vl_sift_pix*> model_fliplr_images_grad_data_;

    // Keep track of mulitple model grad data
    vcl_map<vcl_string,vl_sift_pix*> model_fliplr_images_grad_data_red_;

    // Keep track of mulitple model grad data
    vcl_map<vcl_string,vl_sift_pix*> model_fliplr_images_grad_data_blue_;

    // Keep track of mulitple model grad data
    vcl_map<vcl_string,vl_sift_pix*> model_fliplr_images_grad_data_green_;

    // Model channel lab data
    vcl_map<vcl_string,vil_image_view<double> > model_images_chan1_data_;
    vcl_map<vcl_string,vil_image_view<double> > model_images_chan2_data_;
    vcl_map<vcl_string,vil_image_view<double> > model_images_chan3_data_;

    // Query channel lab data
    vcl_map<vcl_string,vil_image_view<double> > query_images_chan1_data_;
    vcl_map<vcl_string,vil_image_view<double> > query_images_chan2_data_;
    vcl_map<vcl_string,vil_image_view<double> > query_images_chan3_data_;


    // Keep track of gradient image data
    vl_sift_pix* model_grad_data_;

    // Keep track of gradient color image data
    vl_sift_pix* model_grad_red_data_;

    // Keep track of gradient color image data
    vl_sift_pix* model_grad_green_data_;

    // Keep track of gradient color image data
    vl_sift_pix* model_grad_blue_data_;

    // Keep track of gradient image data
    vl_sift_pix* model_grad_fliplr_data_;
 
    // Keep track of grad_fliplrient color image data
    vl_sift_pix* model_grad_fliplr_red_data_;

    // Keep track of grad_fliplrient color image data
    vl_sift_pix* model_grad_fliplr_green_data_;

    // Keep track of grad_fliplrient color image data
    vl_sift_pix* model_grad_fliplr_blue_data_;
   
    // keep track of model L channel
    vil_image_view<double> model_chan1_data_;

    // keep track of model L channel
    vil_image_view<double> model_chan2_data_;

    // keep track of model L channel
    vil_image_view<double> model_chan3_data_;

    // Keep track of query image data
    vl_sift_pix* query_grad_data_;

    // Keep track of gradient color image data
    vl_sift_pix* query_grad_red_data_;

    // Keep track of gradient color image data
    vl_sift_pix* query_grad_green_data_;

    // Keep track of gradient color image data
    vl_sift_pix* query_grad_blue_data_;

    // Keep track of query image data
    vl_sift_pix* query_grad_fliplr_data_;

    // Keep track of grad_fliplrient color image data
    vl_sift_pix* query_grad_fliplr_red_data_;

    // Keep track of grad_fliplrient color image data
    vl_sift_pix* query_grad_fliplr_green_data_;

    // Keep track of grad_fliplrient color image data
    vl_sift_pix* query_grad_fliplr_blue_data_;

    // keep track of query L channel
    vil_image_view<double> query_chan1_data_;

    // keep track of query L channel
    vil_image_view<double> query_chan2_data_;

    // keep track of query L channel
    vil_image_view<double> query_chan3_data_;
    
    // Keep track of model root filters
    VlSiftFilt* model_sift_filter_;
  
    // Keep track of query root filer
    VlSiftFilt* query_sift_filter_;
    
    vil_image_resource_sptr model_image_;
    
    vil_image_resource_sptr query_image_;

    // Keep track of model parts
    vcl_map<unsigned int,vcl_vector<vgl_point_2d<double> > > model_parts_;

    // Keep track of query parts
    vcl_map<unsigned int,vcl_vector<vgl_point_2d<double> > > query_parts_;

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

    // Holds dart matrix
    vcl_map<unsigned int,vcl_vector< vcl_pair<vcl_pair<unsigned int,
        unsigned int>,double> > > 
        model_dart_distances_;
    
    vcl_map<unsigned int,
        vcl_vector< vcl_pair<vcl_pair<unsigned int,unsigned int>,
        dbskr_scurve_sptr > > >
        model_dart_curves_;
        
    vcl_map<vcl_pair<unsigned ,unsigned int>,
        vcl_vector<vgl_point_2d<double> > > 
        query_dart_curves_;

    // Keep output file
    vcl_string output_match_file_;
    vcl_string output_html_file_;
    vcl_string output_binary_file_;
    vcl_string output_binary_h_file_;
    vcl_string output_removed_regions_;
    vcl_string output_parts_file_;

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

    // Use area to scale graphs
    bool scale_area_;

    // Use contour length to scale graphs
    bool scale_length_;

    // Use appearance 
    bool app_sift_;

    // Mirror matching of query shape
    bool mirror_;

    // Use outside shock in shock computation
    bool outside_shock_;

    // Weighting of area cost term
    double area_weight_;

    // Reference area
    double ref_area_;

    // Algorithm for shape coice
    ShapeAlgorithmArea shape_alg_;

    // Color Space to use for computing gradients
    GradColorSpace grad_color_space_;

    // Raw color space to use for computing color histograms
    RawColorSpace raw_color_space_;

    // Store h matrix
    vnl_matrix_fixed<double,3,3> H_matrix_;

    // Store kd forest
    VlKDForest* forest_;

    // Store searcher
    VlKDForestSearcher* searcher_;

    void load_model(vcl_string model_dir);
    void load_query(vcl_string query_dir);

    void load_binary_model(vcl_string model_dir);
    void load_binary_query(vcl_string query_dir);

    void create_html_match_file();
    void write_out_sim_matrix();

    void compute_transformation(vgl_h_matrix_2d<double>& H,
                                vcl_vector<dbskr_scurve_sptr>& curve_list1,
                                vcl_vector<dbskr_scurve_sptr>& curve_list2,
                                vcl_vector< vcl_vector 
                                < vcl_pair <int,int> > >& map_list,
                                vcl_vector< pathtable_key >& path_map,
                                unsigned int sampling_interval,
                                bool flag);

    void compute_similarity(vgl_h_matrix_2d<double>& H,
                            vcl_vector<dbskr_scurve_sptr>& curve_list1,
                            vcl_vector<dbskr_scurve_sptr>& curve_list2,
                            vcl_vector< vcl_vector 
                            < vcl_pair <int,int> > >& map_list,
                            vcl_vector< pathtable_key >& path_map,
                            unsigned int sampling_interval,
                            bool flag,
                            double model_scale_ratio=1.0,
                            double query_scale_ratio=1.0,
                            double width=0.0);

    vcl_pair<double,double> compute_transformed_polygon(
        vgl_h_matrix_2d<double>& H,
        dbskfg_cgraph_directed_tree_sptr& model_tree,
        dbskfg_cgraph_directed_tree_sptr& query_tree);

    double compute_outer_shock_edit_distance(
        dbskfg_cgraph_directed_tree_sptr& model_tree,
        dbskfg_cgraph_directed_tree_sptr& query_tree);

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

    //Match two graphs using original root node selection scheme
    void match_two_graphs_root_node_orig(
        dbskfg_cgraph_directed_tree_sptr& model_tree,
        dbskfg_cgraph_directed_tree_sptr& query_tree,
        double& norm_shape_cost,
        double& norm_shape_length,
        double& app_diff,
        double& norm_app_cost,
        double& rgb_avg_cost,
        double& frob_norm,
        vcl_string match_file_prefix="",
        bool mirror=false,
        double orig_edit_distance=1e6);

    //Match two graphs
    void match_two_debug_graphs(dbskfg_cgraph_directed_tree_sptr& model_tree,
                                dbskfg_cgraph_directed_tree_sptr& query_tree,
                                double& norm_shape_cost,
                                double& norm_shape_length,
                                double& app_diff,
                                double& norm_app_cost,
                                double& rgb_avg_cost,
                                vcl_string prefix);
    
    void write_binary_fragments(vcl_ofstream& binary_sim_file,
                                vcl_map<unsigned int,
                                vcl_pair<vcl_string,
                                dbskfg_composite_graph_sptr> >& fragments );

    void compute_grad_maps(vil_image_resource_sptr& input_image,
                           vl_sift_pix** grad_data,
                           VlSiftFilt** filter,
                           vgl_polygon<double>& poly,
                           bool mask_poly=false,
                           bool fliplr=false);

    void compute_edge_maps(vil_image_resource_sptr& input_image,
                           vl_sift_pix** grad_data,
                           VlSiftFilt** filter);

    void compute_grad_color_maps(vil_image_resource_sptr& input_image,
                                 vl_sift_pix** grad_data,
                                 unsigned int channel);

    void compute_grad_color_maps(vil_image_view<double>& orig_image,
                                 vl_sift_pix** grad_data,
                                 vgl_polygon<double>& poly,
                                 bool mask_poly=false,
                                 bool fliplr=false);
    
    double compute_curve_matching_cost(
        dbskfg_cgraph_directed_tree_sptr& model_tree,
        dbskfg_cgraph_directed_tree_sptr& query_tree);
                          
    vcl_pair<double,double> compute_sift_cost(
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        vcl_vector< pathtable_key >& path_map,
        bool flag=false,
        double width=0.0,
        vl_sift_pix* model_grad_data=0,
        VlSiftFilt*  model_sift_filter=0,
        vl_sift_pix* query_grad_data=0,
        VlSiftFilt*  query_sift_filter=0,
        double model_scale_ratio=1.0,
        double query_scale_ratio=1.0);

    vcl_pair<double,double> compute_rgb_sift_cost(
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        vcl_vector< pathtable_key >& path_map,
        vcl_vector<double>& dart_distances,
        bool flag=false,
        double width=0.0,
        vl_sift_pix* model_red_grad_data=0,
        vl_sift_pix* query_red_grad_data=0,
        vl_sift_pix* model_green_grad_data=0,
        vl_sift_pix* query_green_grad_data=0,
        vl_sift_pix* model_blue_grad_data=0,
        vl_sift_pix* query_blue_grad_data=0,
        VlSiftFilt* model_sift_filter=0,
        VlSiftFilt* query_sift_filter=0,
        double model_scale_ratio=1.0,
        double query_scale_ratio=1.0,
        vcl_string prefix="");

    vcl_pair<double,double> compute_app_alignment_cost(
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        vcl_vector< pathtable_key >& path_map,
        vcl_vector<double>& dart_distances,
        bool flag=false,
        double width=0.0,
        vl_sift_pix* model_red_grad_data=0,
        vl_sift_pix* query_red_grad_data=0,
        vl_sift_pix* model_green_grad_data=0,
        vl_sift_pix* query_green_grad_data=0,
        vl_sift_pix* model_blue_grad_data=0,
        vl_sift_pix* query_blue_grad_data=0,
        VlSiftFilt* model_sift_filter=0,
        VlSiftFilt* query_sift_filter=0,
        double model_scale_ratio=1.0,
        double query_scale_ratio=1.0,
        double model_sift_scale=1.0,
        double query_sift_scale=1.0,
        vcl_string prefix="");

    void compute_app_alignment(
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        vil_image_view<double>& model_channel_1,
        vil_image_view<double>& query_channel_1,
        bool flag=false,
        double width=0.0,
        vl_sift_pix* model_grad_data=0,
        VlSiftFilt*  model_sift_filter=0,
        vl_sift_pix* query_grad_data=0,
        VlSiftFilt*  query_sift_filter=0,
        double model_scale_ratio=1.0,
        double query_scale_ratio=1.0);

    vcl_pair<double,double> compute_dense_rgb_sift_cost(
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        vcl_vector< pathtable_key >& path_map,
        vcl_vector<double>& dart_distances,
        vil_image_view<double>& model_channel_1,
        vil_image_view<double>& model_channel_2,
        vil_image_view<double>& model_channel_3,
        vil_image_view<double>& query_channel_1,
        vil_image_view<double>& query_channel_2,
        vil_image_view<double>& query_channel_3,
        bool flag=false,
        double width=0.0,
        vl_sift_pix* model_red_grad_data=0,
        vl_sift_pix* query_red_grad_data=0,
        vl_sift_pix* model_green_grad_data=0,
        vl_sift_pix* query_green_grad_data=0,
        vl_sift_pix* model_blue_grad_data=0,
        vl_sift_pix* query_blue_grad_data=0,
        VlSiftFilt* model_sift_filter=0,
        VlSiftFilt* query_sift_filter=0,
        double model_scale_ratio=1.0,
        double query_scale_ratio=1.0,
        double model_sift_scale=1.0,
        double query_sift_scale=1.0,
        vcl_string prefix="");

    vgl_point_2d<double> find_part_correspondences(
        vgl_point_2d<double> query_pt,
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        bool flag=false,
        double width=0.0,
        double model_scale_ratio=1.0,
        double query_scale_ratio=1.0);


    void draw_part_correspondence(
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        bool flag=false,
        double width=0.0,
        double model_scale_ratio=1.0,
        double query_scale_ratio=1.0,
        vcl_string prefix="");

    void warp_image(
        dbskfg_cgraph_directed_tree_sptr& model_tree,
        dbskfg_cgraph_directed_tree_sptr& query_tree,
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        bool flag=false,
        double width=0.0,
        double model_scale_ratio=1.0,
        double query_scale_ratio=1.0);                         

    vnl_vector<vl_sift_pix> compress_sift(vl_sift_pix* red_sift,
                                          vl_sift_pix* green_sift,
                                          vl_sift_pix* blue_sift,
                                          int nbp);

    vcl_pair<double,double> compute_o2p_dense(
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        vcl_vector< pathtable_key >& path_map,
        vcl_vector<double>& dart_distances,
        bool flag=false,
        double width=0.0,
        vl_sift_pix* model_red_grad_data=0,
        vl_sift_pix* query_red_grad_data=0,
        vl_sift_pix* model_green_grad_data=0,
        vl_sift_pix* query_green_grad_data=0,
        vl_sift_pix* model_blue_grad_data=0,
        vl_sift_pix* query_blue_grad_data=0,
        VlSiftFilt* model_sift_filter=0,
        VlSiftFilt* query_sift_filter=0,
        double model_scale_ratio=1.0,
        double query_scale_ratio=1.0);

    vcl_pair<double,double> compute_mi(
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        vcl_vector< pathtable_key >& path_map,
        vcl_vector<double>& dart_distances,
        vil_image_view<double>& model_channel_1,
        vil_image_view<double>& model_channel_2,
        vil_image_view<double>& model_channel_3,
        vil_image_view<double>& query_channel_1,
        vil_image_view<double>& query_channel_2,
        vil_image_view<double>& query_channel_3,
        bool flag=false,
        double width=0.0,
        vl_sift_pix* model_red_grad_data=0,
        vl_sift_pix* query_red_grad_data=0,
        vl_sift_pix* model_green_grad_data=0,
        vl_sift_pix* query_green_grad_data=0,
        vl_sift_pix* model_blue_grad_data=0,
        vl_sift_pix* query_blue_grad_data=0,
        VlSiftFilt* model_sift_filter=0,
        VlSiftFilt* query_sift_filter=0,
        double model_scale_ratio=1.0,
        double query_scale_ratio=1.0);


    vcl_pair<double,double> compute_bow(
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        vcl_vector< pathtable_key >& path_map,
        vcl_vector<double>& dart_distances,
        vil_image_view<double>& model_channel_1,
        vil_image_view<double>& model_channel_2,
        vil_image_view<double>& model_channel_3,
        vil_image_view<double>& query_channel_1,
        vil_image_view<double>& query_channel_2,
        vil_image_view<double>& query_channel_3,
        bool flag=false,
        double width=0.0,
        vl_sift_pix* model_red_grad_data=0,
        vl_sift_pix* query_red_grad_data=0,
        vl_sift_pix* model_green_grad_data=0,
        vl_sift_pix* query_green_grad_data=0,
        vl_sift_pix* model_blue_grad_data=0,
        vl_sift_pix* query_blue_grad_data=0,
        VlSiftFilt* model_sift_filter=0,
        VlSiftFilt* query_sift_filter=0,
        double model_scale_ratio=1.0,
        double query_scale_ratio=1.0);

    vcl_pair<double,double> compute_3d_hist_color(
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        vcl_vector< pathtable_key >& path_map,
        vcl_vector<double>& dart_distances,
        vil_image_view<double>& model_channel_1,
        vil_image_view<double>& model_channel_2,
        vil_image_view<double>& model_channel_3,
        vil_image_view<double>& query_channel_1,
        vil_image_view<double>& query_channel_2,
        vil_image_view<double>& query_channel_3,
        double model_scale_ratio=1.0,
        double query_scale_ratio=1.0,
        bool flag=false,
        double width=0.0);

    vcl_pair<double,double> compute_body_centric_sift(
        vcl_vector<dbskr_scurve_sptr>& curve_list1,
        vcl_vector<dbskr_scurve_sptr>& curve_list2,
        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
        vcl_vector< pathtable_key >& path_map,
        vcl_vector<double>& dart_distances,
        vl_sift_pix* model_red_grad_data,
        vl_sift_pix* query_red_grad_data,
        vl_sift_pix* model_green_grad_data,
        vl_sift_pix* query_green_grad_data,
        vl_sift_pix* model_blue_grad_data,
        vl_sift_pix* query_blue_grad_data,
        VlSiftFilt* model_sift_filter=0,
        VlSiftFilt* query_sift_filter=0,
        double model_scale_ratio=1.0,
        double query_scale_ratio=1.0,
        bool flag=false,
        double width=0.0);
    
    vnl_vector<double> compute_second_order_pooling(
        vcl_map<int,vcl_vector<dbskfg_sift_data> >& fragments,
        vl_sift_pix* grad_data,
        VlSiftFilt* filter,
        vsol_box_2d_sptr& bbox);

    double descr_cost(
        vgl_point_2d<double>& model_pt,
        double& model_radius,
        double& model_theta,
        vgl_point_2d<double>& query_pt,
        double& query_radius,
        double& query_theta,
        vl_sift_pix* model_red_grad_data=0,
        vl_sift_pix* query_red_grad_data=0,
        vl_sift_pix* model_green_grad_data=0,
        vl_sift_pix* query_green_grad_data=0,
        vl_sift_pix* model_blue_grad_data=0,
        vl_sift_pix* query_blue_grad_data=0,
        VlSiftFilt* model_sift_filter=0,
        VlSiftFilt* query_sift_filter=0);

    double descr_cost_enriched_sift(
        vgl_point_2d<double>& model_pt,
        double& model_radius,
        double& model_theta,
        vgl_point_2d<double>& query_pt,
        double& query_radius,
        double& query_theta,
        vil_image_view<double>& model_channel1,
        vil_image_view<double>& model_channel2,
        vil_image_view<double>& model_channel3,
        vil_image_view<double>& query_channel1,
        vil_image_view<double>& query_channel2,
        vil_image_view<double>& query_channel3,
        vl_sift_pix* model_red_grad_data=0,
        vl_sift_pix* query_red_grad_data=0,
        vl_sift_pix* model_green_grad_data=0,
        vl_sift_pix* query_green_grad_data=0,
        vl_sift_pix* model_blue_grad_data=0,
        vl_sift_pix* query_blue_grad_data=0,
        VlSiftFilt* model_sift_filter=0,
        VlSiftFilt* query_sift_filter=0);

    void compute_descr(
        vgl_point_2d<double>& pt,
        double& radius,
        double& theta,
        vl_sift_pix* red_grad_data,
        vl_sift_pix* green_grad_data,
        vl_sift_pix* blue_grad_data,
        VlSiftFilt* model_filter,
        vnl_vector<vl_sift_pix>& descriptor);

    void compute_descr(
        vgl_point_2d<double>& pt,
        double& radius,
        double& theta,
        vl_sift_pix* grad_data,
        VlSiftFilt* sift_filter,
        vnl_vector<vl_sift_pix>& descriptor);

    void convert_to_color_space(
        vil_image_resource_sptr& input_image,
        vil_image_view<double>& o1,
        vil_image_view<double>& o2,
        vil_image_view<double>& o3,
        GradColorSpace color_space);

    void write_out_dart_data();

    inline void compute_color_over_sift(
        VlSiftFilt const *f,
        int width, int height,
        double x, double y,
        double sigma,
        double angle0,
        vcl_set<vcl_pair<double,double> >& samples);

    void compute_grad_region_hist(
        vcl_set<vcl_pair<double,double> >& samples,
        vil_image_view<double>& o1_grad_map,
        vil_image_view<double>& o1_angle_map,
        vil_image_view<double>& o2_grad_map,
        vil_image_view<double>& o2_angle_map,
        vil_image_view<double>& o3_grad_map,
        vil_image_view<double>& o3_angle_map,
        vcl_vector<double>& descr,
        vcl_string tile="");
                           
    void compute_color_region_hist(
        vcl_set<vcl_pair<double,double> >& samples,
        vil_image_view<double>& o1,
        vil_image_view<double>& o2,
        vil_image_view<double>& o3,
        vcl_vector<double>& descr,
        LabBinType bin_type,
        vcl_string title="");
       
    void compute_sift_along_curve(dbskr_scurve_sptr scurve,
                                  vnl_matrix<vl_sift_pix>& descriptors,
                                  vl_sift_pix* red_grad_data,
                                  vl_sift_pix* green_grad_data,
                                  vl_sift_pix* blue_grad_data,
                                  VlSiftFilt* sift_filter,
                                  double sift_scale,
                                  double scale_ratio=1.0,
                                  double width=0.0);

    double chi_squared_distance(vnl_vector<double>& descr1,
                                vnl_vector<double>& descr2)
    {
     

        VlDoubleVectorComparisonFunction Chi2_distance =    
            vl_get_vector_comparison_function_d (VlDistanceChi2) ;

        
        double local_distance[1];
        vl_eval_vector_comparison_on_all_pairs_d(local_distance,
                                                 descr1.size(),
                                                 descr1.data_block(),
                                                 1,
                                                 descr2.data_block(),
                                                 1,
                                                 Chi2_distance);


        return 0.5*local_distance[0];
    }

    double LAB_distance(vgl_point_2d<double> model_pt,
                        vgl_point_2d<double> query_pt,
                        vil_image_view<double>& model_channel_1,
                        vil_image_view<double>& model_channel_2,
                        vil_image_view<double>& model_channel_3,
                        vil_image_view<double>& query_channel_1,
                        vil_image_view<double>& query_channel_2,
                        vil_image_view<double>& query_channel_3);

    // Make copy ctor private
    dbskfg_match_bag_of_fragments(const dbskfg_match_bag_of_fragments&);

    // Make assign operator private
    dbskfg_match_bag_of_fragments& operator
        =(const dbskfg_match_bag_of_fragments& );

};

#endif //dbsk2d_ishock_prune_h_

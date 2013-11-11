// This is brcv/shp/dbskfg/pro/dbskfg_match_composite_graph_process.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/pro/dbskfg_match_composite_graph_process.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/algo/dbskfg_cgraph_directed_tree.h>
#include <dbskfg/algo/dbskfg_cgraph_directed_tree_sptr.h>
#include <dbskfg/dbskfg_composite_graph.h>

// dbskfr headers
#include <dbskr/dbskr_tree_edit.h>

// vcl headers
#include <math.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vl/imopv.h>
#include <vl/generic.h>
#include <vil/vil_transpose.h>
#include <vl/mathop.h>
#include <vcl_fstream.h>

//: Constructor
dbskfg_match_composite_graph_process::dbskfg_match_composite_graph_process()
    :final_cost_(0.0)
{

    vcl_vector<vcl_string> root_node_choices;
    root_node_choices.push_back("Largest Radius vs Centroid");         //0
    root_node_choices.push_back("Largest Radius Both");                //1
    root_node_choices.push_back("Centroid Both");                      //2
    
    vcl_string interp_ds = "Interpolation ds to get densely interpolated versions of the scurves: meaningful if localized_edit option is ON";
    vcl_string local_edit = "Local Edit Improves elastic matching cost of scurves using the densely interpolated version";
    
    if (
        !parameters()->add("Sampling ds to reconstruct the scurve", 
                           "-scurve_sample_ds", (float) 5.0f ) ||
        !parameters()->add(interp_ds,
                           "-scurve_interpolate_ds" , (float) 1.0f ) ||
        !parameters()->add(local_edit, "-localized_edit", (bool) false ) ||
        !parameters()->add("use the elastic splice cost computation",
                           "-elastic_splice_cost", (bool) false) ||
        !parameters()->add(
            "curve matching R that weighs bending over stretching",
            "-curve_matching_R", (double) 6.0f) ||
        !parameters()->add(
           "use circular completions at leaf branches during shock matching",
           "-circular_ends", (bool) true) ||
        !parameters()->add("use combined edit cost during shock matching",
                          "-combined_edit", (bool) false) ||
        !parameters()->add("use cost approximation for speed purposes",
                           "-use_approx", (bool) true) ||
        !parameters()->add("root node selection scheme",
                           "-root_scheme", root_node_choices,0) ||
        !parameters()->add("scale root node correspondences",
                           "-scale_root", (bool) false) ||
        !parameters()->add("compute sift appearance cost",
                           "-app_sift", (bool) false) 
        )

    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }

}

//: Destructor
dbskfg_match_composite_graph_process::~dbskfg_match_composite_graph_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_match_composite_graph_process::clone() const
{
    return new dbskfg_match_composite_graph_process(*this);
}

vcl_string
dbskfg_match_composite_graph_process::name()
{
    return "Match Composite Graphs";
}

vcl_vector< vcl_string >
dbskfg_match_composite_graph_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "composite_graph" );
    to_return.push_back( "composite_graph" );
    to_return.push_back( "image" );
    to_return.push_back( "image" );

    return to_return;

}

vcl_vector< vcl_string >
dbskfg_match_composite_graph_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.clear();
    return to_return;
}

int dbskfg_match_composite_graph_process::input_frames()
{
    return 1;
}

int dbskfg_match_composite_graph_process::output_frames()
{
    return 1;
}

bool dbskfg_match_composite_graph_process::execute()
{
    // get input composite graph storage class
    dbskfg_composite_graph_storage_sptr graph1,graph2;
    graph1.vertical_cast(input_data_[0][0]);
    graph2.vertical_cast(input_data_[0][1]);

    //1) get input storage classes
    vidpro1_image_storage_sptr frame_image1,frame_image2;
    frame_image1.vertical_cast(input_data_[0][2]);
    frame_image2.vertical_cast(input_data_[0][3]);

    // get composite graph
    dbskfg_composite_graph_sptr cg1 = 
      graph1->get_composite_graph();
    dbskfg_composite_graph_sptr cg2 = 
      graph2->get_composite_graph();

    //get the subsampling parameter
    bool elastic_splice_cost    = false; 
    float scurve_sample_ds      = 0.0f; 
    float scurve_interpolate_ds = 1.0f; 
    bool localized_edit         = false;
    double scurve_matching_R    = 6.0f; 
    bool circular_ends          = false; 
    bool combined_edit          = false; 
    bool use_approx             = true;
    bool scale_root             = false;
    bool app_sift               = false;
    unsigned int root_scheme    = 0;
    
    parameters()->get_value("-elastic_splice_cost"  , elastic_splice_cost); 
    parameters()->get_value("-scurve_sample_ds"     , scurve_sample_ds);
    parameters()->get_value("-scurve_interpolate_ds", scurve_interpolate_ds); 
    parameters()->get_value("-localized_edit"       , localized_edit ); 
    parameters()->get_value("-curve_matching_R"     , scurve_matching_R ); 
    parameters()->get_value("-circular_ends"        , circular_ends); 
    parameters()->get_value("-combined_edit"        , combined_edit); 
    parameters()->get_value("-use_approx"           , use_approx);
    parameters()->get_value("-scale_root"           , scale_root);
    parameters()->get_value("-app_sift"             , app_sift);
    parameters()->get_value("-root_scheme"          , root_scheme);

    //: prepare the trees also
    dbskfg_cgraph_directed_tree_sptr tree1 = new 
        dbskfg_cgraph_directed_tree(scurve_sample_ds, 
                                    scurve_interpolate_ds, 
                                    scurve_matching_R);

    dbskfg_cgraph_directed_tree_sptr tree2 = new 
        dbskfg_cgraph_directed_tree(scurve_sample_ds, 
                                    scurve_interpolate_ds, 
                                    scurve_matching_R);

  
    bool f1=tree1->acquire
        (cg1, elastic_splice_cost, circular_ends, combined_edit);

    // Get f1 radius
    double f1_root_node_radius=tree1->get_root_node_radius();

    // Create model f2
    bool f2 = tree2->acquire_tree_topology(cg2);
        
    if ( f2 )
    {
        if ( scale_root )
        {
            double f2_root_node_radius=tree2->get_root_node_radius();
            double scale_ratio = f1_root_node_radius/f2_root_node_radius;
            
            vcl_cout<<"Scaling tree 2 to tree 1 ratio of root node radii"
                    <<vcl_endl;
            vcl_cout<<"Tree 1 root node radius: "<<
                f1_root_node_radius<<vcl_endl;
            vcl_cout<<"Tree 2 root node radius: "<<
                f2_root_node_radius<<vcl_endl;
            vcl_cout<<"scale ratio "<<scale_ratio<<vcl_endl;

            tree2->set_scale_ratio(scale_ratio);

        }

        tree2->compute_delete_and_contract_costs(elastic_splice_cost,
                                                 circular_ends,
                                                 combined_edit);
    }
    

    if (!f1 || !f2 ) 
    {
        vcl_cerr << "Problems acquiring trees"<<vcl_endl;
        return false;
    }

    //instantiate the edit distance algorithms
    dbskr_tree_edit edit(tree1.ptr(), tree2.ptr(), circular_ends, 
                         localized_edit);
    
    edit.save_path(true);
    edit.set_curvematching_R(scurve_matching_R);
    edit.set_use_approx(use_approx);
    dbskr_edit_distance_base::RootNodeSelection value=
        (dbskr_edit_distance_base::RootNodeSelection)root_scheme;
    edit.set_root_node_selection(value);

    if ( scale_root )
    {
        
        edit.set_root_node_selection(
            dbskr_edit_distance_base::LARGEST_RADIUS_BOTH);
    }

    if (!edit.edit()) 
    {
        vcl_cerr << "Problems in editing trees"<<vcl_endl;
        return false;
    }

    double val = edit.final_cost();
    
    // Only doing by splice cost

    // Get splice costs
    // isnan found from math.h
    double tree1_splice_cost = ( isnan(tree1->total_splice_cost()) )
        ? 0.0 : tree1->total_splice_cost();
    double tree2_splice_cost = ( isnan(tree2->total_splice_cost()) )
        ? 0.0 : tree2->total_splice_cost();
    
    double norm_val = val/(tree1_splice_cost+tree2_splice_cost );
    
    vcl_cout << "final cost: " << val 
             << " final norm cost: " << norm_val 
             << "( tree1 tot splice: " << tree1_splice_cost
             << ", tree2: " << tree2_splice_cost
             << ")" << vcl_endl;

    final_cost_ = norm_val;

    if ( !app_sift)
    {
        return true;
    }

    if ( app_sift && frame_image1 && frame_image2 )
    {

        // Grab image to see boundaries
        vil_image_resource_sptr image1 = frame_image1->get_image();
        vil_image_resource_sptr image2 = frame_image2->get_image();

        vl_sift_pix* grad_data1(0);
        vl_sift_pix* grad_data2(0);
        
        VlSiftFilt* filter1(0);
        VlSiftFilt* filter2(0);

        vcl_cout<<"Computing Appearance Cost"<<vcl_endl;

        // Compute gradient and angle
        compute_grad_maps(image1,&grad_data1,&filter1);
        compute_grad_maps(image2,&grad_data2,&filter2);
        
        vl_sift_set_magnif(filter1,1.0);
        vl_sift_set_magnif(filter2,1.0);

        // Get correspondece
        edit.get_correspondence(curve_list1_,
                                curve_list2_,
                                map_list_,
                                path_map_);

        double final_app_cost = 
            compute_app_cost(grad_data1,grad_data2,filter1,filter2);

        vcl_cout<<"Final app cost: "<<final_app_cost<<vcl_endl;

        vl_sift_delete(filter1);
        vl_sift_delete(filter2);
        vl_free(grad_data1);
        vl_free(grad_data2);
    }

    return true;
}

double dbskfg_match_composite_graph_process::compute_app_cost(
    vl_sift_pix* grad_data1,
    vl_sift_pix* grad_data2,
    VlSiftFilt* filter1,
    VlSiftFilt* filter2)
{
 

    VlFloatVectorComparisonFunction L2_distance =    
      vl_get_vector_comparison_function_f (VlDistanceL2) ;
    double sift_diff= 0.0;
 
    // Get matching pairs
    for (unsigned i = 0; i < map_list_.size(); i++) 
    {
        dbskr_scurve_sptr sc1 = curve_list1_[i];
        dbskr_scurve_sptr sc2 = curve_list2_[i];

        double local_distance=0.0;

        vcl_vector< vcl_vector<vl_sift_pix> > model_sift;
        vcl_vector< vcl_vector<vl_sift_pix> > query_sift;
        
        for (unsigned j = 0; j < map_list_[i].size(); ++j) 
        {
            vcl_pair<int, int> cor = map_list_[i][j];
            
            // Compute sift for both images
           
            // Shock Point 1 from Model
            vgl_point_2d<double> ps1 = sc1->sh_pt(cor.first);
            double radius_ps1        = sc1->time(cor.first);
            double theta_ps1         = sc1->theta(cor.first);
            vl_sift_pix descr_ps1[128];
            
            vl_sift_calc_raw_descriptor(filter1,
                                        grad_data1,
                                        descr_ps1,
                                        filter1->width,
                                        filter1->height,
                                        ps1.x(),
                                        ps1.y(),
                                        radius_ps1/2,
                                        theta_ps1);

            vcl_vector<vl_sift_pix> descr_vec1;
            descr_vec1.assign(descr_ps1,descr_ps1+128);
            descr_vec1.push_back(ps1.x());
            descr_vec1.push_back(ps1.y());
            descr_vec1.push_back(radius_ps1);
            descr_vec1.push_back(theta_ps1);

            // Shock Point 2 from Query
            vgl_point_2d<double> ps2 = sc2->sh_pt(cor.second);
            double radius_ps2        = sc2->time(cor.second);
            double theta_ps2         = sc2->theta(cor.second);
            vl_sift_pix descr_ps2[128];

            vl_sift_calc_raw_descriptor(filter2,
                                        grad_data2,
                                        descr_ps2,
                                        filter2->width,
                                        filter2->height,
                                        ps2.x(),
                                        ps2.y(),
                                        radius_ps2/2,
                                        theta_ps2);

            vcl_vector<vl_sift_pix> descr_vec2;    
            descr_vec2.assign(descr_ps2,descr_ps2+128);
            descr_vec2.push_back(ps2.x());
            descr_vec2.push_back(ps2.y());
            descr_vec2.push_back(radius_ps2);
            descr_vec2.push_back(theta_ps2);
        
            vl_sift_pix result[1];

            vl_eval_vector_comparison_on_all_pairs_f(result,
                                                     128,
                                                     descr_ps1,
                                                     1,
                                                     descr_ps2,
                                                     1,
                                                     L2_distance);

            sift_diff=sift_diff+result[0];
            local_distance=local_distance+result[0];

            model_sift.push_back(descr_vec1);
            query_sift.push_back(descr_vec2);
            
        }
        vcl_cout<<"Tree 1 dart ("
                <<path_map_[i].first.first
                <<","
                <<path_map_[i].first.second
                <<") Tree 2 dart ("
                <<path_map_[i].second.first
                <<","
                <<path_map_[i].second.second
                <<") L2 distance: "
                <<local_distance<<vcl_endl;

        {
            vcl_stringstream model_stream;
            model_stream<<"Dart_model_"<<i<<"_app_correspondence.txt";
            vcl_stringstream query_stream;
            query_stream<<"Dart_query_"<<i<<"_app_correspondence.txt";

            vcl_ofstream model_file(model_stream.str().c_str());
            model_file<<model_sift.size()<<vcl_endl;
            for ( unsigned int b=0; b < model_sift.size() ; ++b)
            {
                vcl_vector<vl_sift_pix> vec=model_sift[b];
                for  ( unsigned int c=0; c < vec.size() ; ++c)
                {
                    model_file<<vec[c]<<vcl_endl;
                }
            }
            model_file.close();

            vcl_ofstream query_file(query_stream.str().c_str());
            query_file<<query_sift.size()<<vcl_endl;
            for ( unsigned int b=0; b < query_sift.size() ; ++b)
            {
                vcl_vector<vl_sift_pix> vec=query_sift[b];
                for  ( unsigned int c=0; c < vec.size() ; ++c)
                {
                    query_file<<vec[c]<<vcl_endl;
                }
            }
            query_file.close();
 
        }
    }

    return sift_diff;
}

void dbskfg_match_composite_graph_process::compute_grad_maps(
    vil_image_resource_sptr& input_image,
    vl_sift_pix** grad_data,
    VlSiftFilt** filter)
{
    vil_image_view<vxl_byte> temp = 
        vil_convert_to_grey_using_rgb_weighting(input_image->get_view());
    vil_image_view<vxl_byte> temp2=
        vil_transpose(temp);
    
    vil_image_view<double> image;
    vil_convert_cast(temp2,image);

    unsigned int width  = image.ni();
    unsigned int height = image.nj();

    double* gradient_magnitude = (double*) 
        vl_malloc(width*height*sizeof(double));
    double* gradient_angle     = (double*) 
        vl_malloc(width*height*sizeof(double));

    double* image_data=image.top_left_ptr();

    vl_imgradient_polar_d(
        gradient_magnitude, // gradient magnitude 
        gradient_angle,     // gradient angle
        1,                  // output width
        width,              // output height
        image_data,         // input image
        width,              // input image width
        height,             // input image height
        width);             // input image stride

    *filter = vl_sift_new(width,height,3,3,0);
    *grad_data=(vl_sift_pix*) vl_malloc(sizeof(vl_sift_pix)*width*height*2);
    
    unsigned int index=0;
    for ( unsigned int i=0; i < width*height; ++i)
    {
        double value=gradient_magnitude[i];
        (*grad_data)[index]=value;
        ++index;
    }

    for ( unsigned int i=0; i < width*height; ++i)
    {
        double value=gradient_angle[i];
        (*grad_data)[index]=value;
        ++index;
    }

    vl_free(gradient_magnitude);
    vl_free(gradient_angle);

}

bool dbskfg_match_composite_graph_process::finish()
{
  return true;
}



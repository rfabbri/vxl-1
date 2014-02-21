// This is brcv/shp/dbskfg/pro/dbskfg_match_bag_of_fragments_process.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/pro/dbskfg_match_bag_of_fragments_process.h>
#include <dbskfg/algo/dbskfg_match_bag_of_fragments.h>

// vidpro1 headers
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

//: Constructor
dbskfg_match_bag_of_fragments_process::dbskfg_match_bag_of_fragments_process()
{    
    vcl_string interp_ds = "Interpolation ds to get densely interpolated versions of the scurves: meaningful if localized_edit option is ON";
    vcl_string local_edit = "Local Edit Improves elastic matching cost of scurves using the densely interpolated version";
    
    if (!parameters()->add( "Model folder:" , 
                            "-model_folder" , bpro1_filepath("", "")) ||
        !parameters()->add( "Query folder:" , 
                            "-query_folder" , bpro1_filepath("", "")) ||
        !parameters()->add( "Output folder:", 
                            "-output_folder", bpro1_filepath("", "")) ||
        !parameters()->add( "Output file prefix:" , 
                            "-output_prefix", vcl_string("")) ||
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
        !parameters()->add("scale shock graphs bbox during matching",
                           "-scale_bbox", (bool) false) ||
        !parameters()->add("scale root node correspondences",
                           "-scale_root", (bool) false) ||
        !parameters()->add("compute sift appearance cost",
                           "-app_sift", (bool) false) ||
        !parameters()->add("horizontal mirror query shape",
                           "-mirror", (bool) false) ||
        !parameters()->add("weight area cost term",
                           "-area_weight", (double) 0.0f) 

        )

    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }

}

//: Destructor
dbskfg_match_bag_of_fragments_process::~dbskfg_match_bag_of_fragments_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_match_bag_of_fragments_process::clone() const
{
    return new dbskfg_match_bag_of_fragments_process(*this);
}

vcl_string
dbskfg_match_bag_of_fragments_process::name()
{
    return "Match Bag of Fragments";
}

vcl_vector< vcl_string >
dbskfg_match_bag_of_fragments_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    to_return.push_back( "image" );

    return to_return;
    
}

vcl_vector< vcl_string >
dbskfg_match_bag_of_fragments_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.clear();
    return to_return;
}

int dbskfg_match_bag_of_fragments_process::input_frames()
{
    return 1;
}

int dbskfg_match_bag_of_fragments_process::output_frames()
{
    return 1;
}

bool dbskfg_match_bag_of_fragments_process::execute()
{

    //1) get input storage classes
    vidpro1_image_storage_sptr frame_image1,frame_image2;
    frame_image1.vertical_cast(input_data_[0][0]);
    frame_image2.vertical_cast(input_data_[0][1]);

    vil_image_resource_sptr model_image = frame_image1->get_image();
    vil_image_resource_sptr query_image = frame_image2->get_image();
    
    bpro1_filepath model_folder_filepath;
    this->parameters()->get_value("-model_folder", model_folder_filepath);
    vcl_string model_dir = model_folder_filepath.path;
    
    bpro1_filepath query_folder_filepath;
    this->parameters()->get_value("-query_folder", query_folder_filepath);
    vcl_string query_dir = query_folder_filepath.path;

    bpro1_filepath output_folder_filepath;
    this->parameters()->get_value("-output_folder", output_folder_filepath);
    vcl_string output_folder = output_folder_filepath.path;

    vcl_string output_prefix;
    parameters()->get_value("-output_prefix", output_prefix);
 
    vcl_string output_file = output_folder+"/"+output_prefix;

    //get the subsampling parameter
    bool elastic_splice_cost    = false; 
    float scurve_sample_ds      = 0.0f; 
    float scurve_interpolate_ds = 1.0f; 
    bool localized_edit         = false;
    double scurve_matching_R    = 6.0f; 
    bool circular_ends          = false; 
    bool combined_edit          = false; 
    bool use_approx             = true;
    bool scale_bbox             = false;
    bool scale_root             = false;
    bool app_sift               = false;
    bool mirror                 = false;
    double area_weight          = 0.0f;

    parameters()->get_value("-elastic_splice_cost"  , elastic_splice_cost); 
    parameters()->get_value("-scurve_sample_ds"     , scurve_sample_ds);
    parameters()->get_value("-scurve_interpolate_ds", scurve_interpolate_ds); 
    parameters()->get_value("-localized_edit"       , localized_edit ); 
    parameters()->get_value("-curve_matching_R"     , scurve_matching_R ); 
    parameters()->get_value("-circular_ends"        , circular_ends); 
    parameters()->get_value("-combined_edit"        , combined_edit); 
    parameters()->get_value("-use_approx"           , use_approx);
    parameters()->get_value("-scale_bbox"           , scale_bbox);
    parameters()->get_value("-scale_root"           , scale_root);
    parameters()->get_value("-app_sift"             , app_sift);
    parameters()->get_value("-mirror"               , mirror);
    parameters()->get_value("-area_weight"          , area_weight);
 
    dbskfg_match_bag_of_fragments match_frags(model_dir,
                                              query_dir,
                                              output_file,
                                              elastic_splice_cost, 
                                              scurve_sample_ds, 
                                              scurve_interpolate_ds, 
                                              localized_edit,
                                              scurve_matching_R, 
                                              circular_ends, 
                                              combined_edit, 
                                              use_approx,
                                              scale_bbox,
                                              scale_root,
                                              app_sift,
                                              mirror,
                                              area_weight,
                                              model_image,
                                              query_image);
   
    bool status(true);
    
    if (!scale_bbox)
    {
        if ( !scale_root) 
        {
            match_frags.binary_match();
        }
        else
        {
            match_frags.binary_scale_root_match();
        }
    }
    else
    {
        match_frags.binary_scale_match();
    }

    //Set storage with new transform graph
    clear_output();

    return status;
}

bool dbskfg_match_bag_of_fragments_process::finish()
{
    return true;
}



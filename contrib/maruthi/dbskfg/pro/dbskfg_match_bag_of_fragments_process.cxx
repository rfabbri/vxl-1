// This is brcv/shp/dbskfg/pro/dbskfg_match_bag_of_fragments_process.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/pro/dbskfg_match_bag_of_fragments_process.h>
#include <dbskfg/algo/dbskfg_match_bag_of_fragments.h>

// vidpro1 headers
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <dbskfg/algo/dbskfg_match_bag_of_fragments.h>

//: Constructor
dbskfg_match_bag_of_fragments_process::dbskfg_match_bag_of_fragments_process()
{    
    vcl_string interp_ds = "Interpolation ds to get densely interpolated versions of the scurves: meaningful if localized_edit option is ON";
    vcl_string local_edit = "Local Edit Improves elastic matching cost of scurves using the densely interpolated version";
    
    vcl_vector<vcl_string> choices;
    choices.push_back("Scale to Ref Area");
    choices.push_back("Scale to Mean Shape");
    choices.push_back("Scale to Larger Shape");
    choices.push_back("Scale to Smaller Shape");

    vcl_vector<vcl_string> grad_color_choices;
    grad_color_choices.push_back("RGB");
    grad_color_choices.push_back("Opponent");
    grad_color_choices.push_back("Normalized Opponent");
    grad_color_choices.push_back("LAB");
    
    vcl_vector<vcl_string> raw_color_choices;
    raw_color_choices.push_back("LAB");
    raw_color_choices.push_back("HSV");
    raw_color_choices.push_back("RGB");
    raw_color_choices.push_back("OPP");
    raw_color_choices.push_back("NOPP");

    if (!parameters()->add( "Model folder:" , 
                            "-model_folder" , bpro1_filepath("", "")) ||
        !parameters()->add( "Query folder:" , 
                            "-query_folder" , bpro1_filepath("", "")) ||
        !parameters()->add( "Model image file:" , 
                            "-model_image_file" , bpro1_filepath("", "")) ||
        !parameters()->add( "Train BOW File:" , 
                            "-bow_file" , bpro1_filepath("", "")) ||
        !parameters()->add( "Part File:" , 
                            "-part_file" , bpro1_filepath("", "")) ||
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
        !parameters()->add("scale by area",
                           "-scale_area", (bool) false) ||
        !parameters()->add("scale by contour length",
                           "-scale_length", (bool) false) ||
        !parameters()->add("compute sift appearance cost",
                           "-app_sift", (bool) false) ||
        !parameters()->add("horizontal mirror query shape",
                           "-mirror", (bool) false) ||
        !parameters()->add("use outside shock",
                           "-outside_shock", (bool) false) ||
        !parameters()->add("weight area cost term",
                           "-area_weight", (double) 0.0f) ||
        !parameters()->add("reference area",
                           "-ref_area", (double) 10000.0f)||
        !parameters()->add("mask gradient", "-mask",(bool)false) ||
        !parameters()->add("train bow", "-bow",(bool)false) ||
        !parameters()->add("keywords", "-centers",(int)256) ||
        !parameters()->add("shape algorithm", "-shape_alg",choices,1) ||
        !parameters()->add("grad color space", "-grad_color_space",
                           grad_color_choices,1) ||
        !parameters()->add("raw color space", "-raw_color_space",
                           raw_color_choices,0)        

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

    bpro1_filepath model_image_file;
    this->parameters()->get_value("-model_image_file" , model_image_file);
    vcl_string model_image_path=model_image_file.path;


    bpro1_filepath bow_file;
    this->parameters()->get_value("-bow_file" , bow_file);
    vcl_string bow_path=bow_file.path;

    bpro1_filepath part_file;
    this->parameters()->get_value("-part_file" , part_file);
    vcl_string part_path=part_file.path;

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
    bool scale_area             = false;
    bool scale_length           = false;
    bool app_sift               = false;
    bool mirror                 = false;
    bool outside_shock          = false;
    double area_weight          = 0.0f;
    double ref_area             = 10000.0f;
    unsigned int shape_alg      = 1;
    unsigned int color_alg      = 1;
    unsigned int raw_color      = 1;
    bool mask_grad              = false;
    bool bow                    = false;
    int centers                 = 256;

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
    parameters()->get_value("-scale_area"           , scale_area);
    parameters()->get_value("-scale_length"         , scale_length);
    parameters()->get_value("-app_sift"             , app_sift);
    parameters()->get_value("-mirror"               , mirror);
    parameters()->get_value("-outside_shock"        , outside_shock);
    parameters()->get_value("-area_weight"          , area_weight);
    parameters()->get_value("-ref_area"             , ref_area);
    parameters()->get_value("-shape_alg"            , shape_alg );
    parameters()->get_value("-grad_color_space"     , color_alg );
    parameters()->get_value("-mask"                 , mask_grad );
    parameters()->get_value("-raw_color_space"      , raw_color );
    parameters()->get_value("-bow"                  , bow);
    parameters()->get_value("-centers"              , centers);

    dbskfg_match_bag_of_fragments::ShapeAlgorithmArea shape_alg_area=
        dbskfg_match_bag_of_fragments::SCALE_TO_MEAN;

    if ( shape_alg == 0 )
    {
        shape_alg_area=dbskfg_match_bag_of_fragments::SCALE_TO_REF;

    }
    else if ( shape_alg == 1 )
    {
        shape_alg_area=dbskfg_match_bag_of_fragments::SCALE_TO_MEAN;
        
    }
    else if ( shape_alg == 2 )
    {
        shape_alg_area=dbskfg_match_bag_of_fragments::SCALE_TO_MAX;
    }
    else if (shape_alg ==  3)
    {
        shape_alg_area=dbskfg_match_bag_of_fragments::SCALE_TO_MIN;
        
    }

    dbskfg_match_bag_of_fragments::GradColorSpace grad_color_space =
        dbskfg_match_bag_of_fragments::OPP;
   
    if ( color_alg == 0 )
    {
        grad_color_space = dbskfg_match_bag_of_fragments::RGB;
    }
    else if ( color_alg == 1 )
    {
        grad_color_space = dbskfg_match_bag_of_fragments::OPP;
    }
    else if ( color_alg == 2 )
    {
        grad_color_space = dbskfg_match_bag_of_fragments::NOPP;
    }
    else
    {

        grad_color_space = dbskfg_match_bag_of_fragments::LAB_2;
    }

    dbskfg_match_bag_of_fragments::RawColorSpace raw_color_space =
        dbskfg_match_bag_of_fragments::LAB;
   
    if ( raw_color == 0 )
    {
        raw_color_space = dbskfg_match_bag_of_fragments::LAB;
    }
    else if ( raw_color == 1 )
    {
        raw_color_space = dbskfg_match_bag_of_fragments::HSV;
    }
    else if ( raw_color == 2 )
    {
        raw_color_space = dbskfg_match_bag_of_fragments::RGB_2;
    }
    else if ( raw_color == 3 )
    {
        raw_color_space = dbskfg_match_bag_of_fragments::OPP_2;
    }
    else
    {
        raw_color_space = dbskfg_match_bag_of_fragments::NOPP_2;
    }

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
                                              scale_area,
                                              scale_length,
                                              app_sift,
                                              mirror,
                                              outside_shock,
                                              mask_grad,
                                              area_weight,
                                              ref_area,
                                              shape_alg_area,
                                              grad_color_space,
                                              raw_color_space,
                                              model_image,
                                              query_image,
                                              model_image_path);
   
    bool status(true);
    
    if ( bow )
    {
        match_frags.train_gmm(centers);
    }
    else
    {
        if ( bow_path.size() )
        {
            match_frags.set_gmm_train(bow_path);
        }

        if ( part_path.size() )
        {
            match_frags.set_part_file(part_path);
        }

        if (!scale_bbox)
        {
            if ( !scale_root && !scale_area && !scale_length) 
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
    }
    //Set storage with new transform graph
    clear_output();

    return status;
}

bool dbskfg_match_bag_of_fragments_process::finish()
{
    return true;
}



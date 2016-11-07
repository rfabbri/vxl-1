// This is brcv/shp/dbskfg/pro/dbskfg_extract_fragments_process.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/pro/dbskfg_extract_fragments_process.h>

// vidpro1 headers
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <dbskfg/algo/dbskfg_extract_fragments.h>

//: Constructor
dbskfg_extract_fragments_process::dbskfg_extract_fragments_process()
{
    if (
        !parameters()->add( "Detect Gaps  " , "-b_gaps", bool(true)) ||
        !parameters()->add( "Detect Loops " , "-b_loops", bool(true)) ||
        !parameters()->add( "Alpha" , "-alpha" , (double)0.5) || 
        !parameters()->add( "Euler Sprial Completion" , "-ess" , 
                            (double)0.25) ||
        !parameters()->add( "Transform Threshold:  " , 
                            "-thresh", double(0.5f)) ||
        !parameters()->add( 
            "Prune Threshold:" , "-prune_thres" ,(double) 1.0f) ||
        !parameters()->add( 
            "Contour Ratio:" ,  "-con_ratio" ,  (double) 0.5f ) ||
        !parameters()->add( "Output folder:" , 
                            "-output_folder", bpro1_filepath("", "")) ||
        !parameters()->add( "Output file prefix:" , 
                            "-output_prefix", vcl_string(""))
        )

    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }

}

//: Destructor
dbskfg_extract_fragments_process::~dbskfg_extract_fragments_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_extract_fragments_process::clone() const
{
    return new dbskfg_extract_fragments_process(*this);
}

vcl_string
dbskfg_extract_fragments_process::name()
{
    return "Extract Fragments";
}

vcl_vector< vcl_string >
dbskfg_extract_fragments_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    to_return.push_back( "vsol2D" );
    return to_return;
    
}

vcl_vector< vcl_string >
dbskfg_extract_fragments_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.clear();
    return to_return;
}

int dbskfg_extract_fragments_process::input_frames()
{
    return 1;
}

int dbskfg_extract_fragments_process::output_frames()
{
    return 1;
}

bool dbskfg_extract_fragments_process::execute()
{
     //1) get input storage classes
    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][0]);

    vidpro1_vsol2D_storage_sptr input_vsol;
    input_vsol.vertical_cast(input_data_[0][1]);

    bpro1_filepath output_folder_filepath;
    this->parameters()->get_value("-output_folder", output_folder_filepath);
    vcl_string output_folder = output_folder_filepath.path;

    bool detect_gaps(true);
    bool detect_loops(true);
    double ess(0.25);
    double alpha(0.5);
    double transform_thresh(0.5f);
    vcl_string output_prefix;
    double prune_thres(1.0f);
    double contour_ratio(0.5f);

    parameters()->get_value( "-con_ratio" ,   contour_ratio);
    parameters()->get_value( "-prune_thres" , prune_thres);
    parameters()->get_value("-output_prefix", output_prefix);
    parameters()->get_value( "-thresh" ,      transform_thresh );
    parameters()->get_value( "-b_gaps" ,      detect_gaps );
    parameters()->get_value( "-b_loops" ,     detect_loops );
    parameters()->get_value( "-ess" ,         ess );
    parameters()->get_value( "-alpha" ,       alpha );
 
    
    dbskfg_extract_fragments get_frags(input_vsol,
                                       frame_image);
    get_frags.extract(detect_gaps,
                      detect_loops,
                      ess,
                      alpha,
                      transform_thresh,
                      contour_ratio,
                      prune_thres,
                      output_folder,
                      output_prefix);

    // {
    //     dbskfg_extract_fragments get_frags2(input_vsol,
    //                                         frame_image);
        
    //     get_frags2.extract_two(detect_gaps,
    //                           detect_loops,
    //                           ess,
    //                           alpha,
    //                           transform_thresh,
    //                           contour_ratio,
    //                           prune_thres,
    //                           output_folder,
    //                           output_prefix+"_2");

    // }

    // {
    //     dbskfg_extract_fragments get_frags3(input_vsol,
    //                                         frame_image);
        
    //     get_frags3.extract_three(detect_gaps,
    //                              detect_loops,
    //                              ess,
    //                              alpha,
    //                              transform_thresh,
    //                              contour_ratio,
    //                              prune_thres,
    //                              output_folder,
    //                              output_prefix+"_3");

    // }

    //Set storage with new transform graph
    clear_output();

    return true;
}

bool dbskfg_extract_fragments_process::finish()
{
    return true;
}



// This is brcv/shp/dbskfg/pro/dbskfg_detect_fragments_process.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/pro/dbskfg_detect_fragments_process.h>
#include <dbskfg/algo/dbskfg_detect_fragments.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <bsol/bsol_algs.h>

//: Constructor
dbskfg_detect_fragments_process::dbskfg_detect_fragments_process()
{

    if ( !parameters()->add( "Input Match file <filename...>" , "-sminput" , 
                             bpro1_filepath("","*.xml")) ||
         !parameters()->add( "threshold for detections", 
                             "-thres", (double) 0.5f ) ||
         !parameters()->
         add( "use top N matches of the image to each model patch",
              "-top_N",(unsigned int) 3 ) ||

         !parameters()->
         add("at least k model patches", "-con_k",(unsigned int) 2)     

        )

    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }

}

//: Destructor
dbskfg_detect_fragments_process::~dbskfg_detect_fragments_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_detect_fragments_process::clone() const
{
    return new dbskfg_detect_fragments_process(*this);
}

vcl_string
dbskfg_detect_fragments_process::name()
{
    return "Detect Composite Fragments";
}

vcl_vector< vcl_string >
dbskfg_detect_fragments_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.clear();
    return to_return;
    
}

vcl_vector< vcl_string >
dbskfg_detect_fragments_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    // ouptut the storage class
    to_return.push_back( "vsol2D" );
    return to_return;
}

int dbskfg_detect_fragments_process::input_frames()
{
    return 1;
}

int dbskfg_detect_fragments_process::output_frames()
{
    return 1;
}

bool dbskfg_detect_fragments_process::execute()
{

    // get input file path
    bpro1_filepath input;
    parameters()->get_value( "-sminput" , input);
    vcl_string input_file_path = input.path;

    // Read in parameters
    double threshold;
    parameters()->get_value("-thres",threshold);

    unsigned int top_N, con_k;
    parameters()->get_value("-top_N",top_N);
    parameters()->get_value("-con_k",con_k);

    //Set storage with new transform graph
    clear_output();

    dbskfg_detect_fragments detector(input_file_path);

    vsol_box_2d_sptr box;
    detector.detect(threshold,top_N,con_k,box);

    // Bouding box could be empty 
    if ( box )
    {   
        // Add to vidpro storage this new bounding box
        vsol_polygon_2d_sptr box_poly = bsol_algs::poly_from_box(box);

        vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
        output_vsol->add_object(box_poly->cast_to_spatial_object());
        output_data_[0].push_back(output_vsol);
        vcl_cout<<"Bounding box detected at this threshold"<<vcl_endl;
    }
    else
    {

        vcl_cout<<"No bounding box detected at this threshold"<<vcl_endl;

    }

    vcl_cout<<vcl_endl;
    return true;

}

bool dbskfg_detect_fragments_process::finish()
{
    return true;
}



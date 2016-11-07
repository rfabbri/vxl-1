/*
 * dbdet_pb_edge_detector_process.cxx
 *
 *  Created on: Feb 22, 2010
 *      Author: firat
 */

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

#include <dbdet/algo/dbdet_pb_edge_detector.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>

#include <vcl_iostream.h>

#include "dbdet_pb_edge_detector_process.h"

dbdet_pb_edge_detector_process::dbdet_pb_edge_detector_process()
{
    vcl_vector<vcl_string> detector_choices;
    detector_choices.push_back("BG (Brightness Gardient)");       //0
    detector_choices.push_back("CG (Color Gradient)");    //1
    detector_choices.push_back("TG (Texture Gradient)");    //2
    detector_choices.push_back("GM (Gradient Magnitude)");   //3
    detector_choices.push_back("GM2 (Gradient Magnitude - Multi-scale)"); //4
    detector_choices.push_back("BGTG (Brightness + Texture Gradient)"); //5
    detector_choices.push_back("CGTG (Color + Texture Gradient)"); //6

    vcl_vector<vcl_string> pres_choices;
    pres_choices.push_back("gray");
    pres_choices.push_back("color");

    if(     !parameters()->add( "Temporary location for storing pb octave files:"   , "-temp_path" , vcl_string("/vision/scratch/octave_tmp/pb")) ||

            !parameters()->add( "Detector type:"   , "-pb_detector_type" , detector_choices, 5) ||

            !parameters()->add( "Use default parameters:"  , "-use_default" , true ) ||

            !parameters()->add( "Radius vector[0] (BG, CG, TG, BGTG, CGTG):"    , "-rv_0"   , 0.01 ) ||
            !parameters()->add( "Radius vector[1] (BGTG, CGTG):"    , "-rv_1"   , 0.02 ) ||
            !parameters()->add( "Radius vector[2] (CGTG):"    , "-rv_2"   , 0.02 ) ||
            !parameters()->add( "Radius vector[3] (CGTG):"    , "-rv_3"   , 0.02 ) ||

            !parameters()->add( "Number of orientations (BG, CG, TG, BGTG, CGTG):"   , "-norient" , 8 ) ||

            !parameters()->add( "Presentation (BGTG):"   , "-pres" , pres_choices, 0) ||

            !parameters()->add( "Sigma (GM, GM2):"  , "-sigma" , 2.0 ))
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
}

//: Destructor
dbdet_pb_edge_detector_process::~dbdet_pb_edge_detector_process()
{
}


//: Clone the process
bpro1_process* dbdet_pb_edge_detector_process::clone() const
{
    return new dbdet_pb_edge_detector_process(*this);
}


//: Return the name of this process
vcl_string dbdet_pb_edge_detector_process::name()
{
    return "Pb Edge Detector (Octave)";
}


//: Return the number of input frame for this process
int dbdet_pb_edge_detector_process::input_frames()
{
    return 1;
}


//: Return the number of output frames for this process
int dbdet_pb_edge_detector_process::output_frames()
{
    return 1;
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_pb_edge_detector_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_pb_edge_detector_process::get_output_type()
{
    vcl_vector<vcl_string > to_return;
    to_return.push_back( "edge_map" );
    return to_return;
}


//: Execute the process
bool dbdet_pb_edge_detector_process::execute()
{
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbdet_pb_edge_detector_process::execute() - not exactly one"
                << " input images \n";
        return false;
    }
    clear_output();

    vcl_cout << "Pb edge detection..." << vcl_endl;;

    // get image from the storage class
    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][0]);
    vil_image_resource_sptr image_sptr = frame_image->get_image();
    vil_image_view<vxl_byte> image_view = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );

    //get the parameters
    unsigned detector_type, presentation;
    double rv0, rv1, rv2, rv3, sigma;
    int norient;
    bool use_default;
    vcl_string pb_temp_dir;

    parameters()->get_value( "-temp_path", pb_temp_dir);
    parameters()->get_value( "-pb_detector_type", detector_type);
    parameters()->get_value( "-pres", presentation);
    parameters()->get_value( "-rv_0" , rv0 );
    parameters()->get_value( "-rv_1" , rv1 );
    parameters()->get_value( "-rv_2" , rv2 );
    parameters()->get_value( "-rv_3" , rv3 );
    parameters()->get_value( "-norient" , norient );
    parameters()->get_value( "-sigma", sigma);
    parameters()->get_value( "-use_default", use_default);

    dbdet_edgemap_sptr edge_map;
    dbdet_pb_edge_detector pb(pb_temp_dir);

    switch(detector_type)
    {
    case 0: /* BG */
        if(use_default)
        {
            edge_map = pb.detect_bg(image_view);
        }
        else
        {
            edge_map = pb.detect_bg(image_view, rv0, norient);
        }
        break;
    case 1: /* CG */
        if(use_default)
        {
            edge_map = pb.detect_cg(image_view);
        }
        else
        {
            edge_map = pb.detect_cg(image_view, rv0, norient);
        }
        break;
    case 2: /* TG */
        if(use_default)
        {
            edge_map = pb.detect_tg(image_view);
        }
        else
        {
            edge_map = pb.detect_tg(image_view, rv0, norient);
        }
        break;
    case 3: /* GM */
        if(use_default)
        {
            edge_map = pb.detect_gm(image_view);
        }
        else
        {
            edge_map = pb.detect_gm(image_view, sigma);
        }
        break;
    case 4: /* GM2 */
        if(use_default)
        {
            edge_map = pb.detect_gm2(image_view);
        }
        else
        {
            edge_map = pb.detect_gm2(image_view, sigma);
        }
        break;
    case 5: /* BGTG */
        if(use_default)
        {
            edge_map = pb.detect_bgtg(image_view);
        }
        else
        {
            vcl_string pres;
            if(presentation == 1)
            {
                pres = "color";
            }
            else
            {
                pres = "gray";
            }
            double rv[] = {rv0, rv1};
            edge_map = pb.detect_bgtg(image_view, pres, rv, norient);
        }
        break;
    case 6: /* CGTG */
        if(use_default)
        {
            edge_map = pb.detect_cgtg(image_view);
        }
        else
        {
            double rv[] = {rv0, rv1, rv2, rv3};
            edge_map = pb.detect_cgtg(image_view, rv, norient);
        }
        break;
    }

    // create the output storage class
    dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
    output_edgemap->set_edgemap(edge_map);
    output_data_[0].push_back(output_edgemap);

    vcl_cout << "done!" << vcl_endl;
    vcl_cout << "#edgels = " << edge_map->num_edgels() << vcl_endl;

    vcl_cout.flush();

    return true;
}

bool dbdet_pb_edge_detector_process::finish()
{
    return true;
}


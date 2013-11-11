// This is /lemsvxl/brcv/rec/dborl/algo/vox_octave_pb_edge_detector/vox_octave_pb_edge_detector_main.cxx

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Feb 26, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include "vox_octave_pb_edge_detector_params.h"
#include "vox_octave_pb_edge_detector_params_sptr.h"

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <dbdet/algo/dbdet_load_edg.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    vox_octave_pb_edge_detector_params_sptr params =
            new vox_octave_pb_edge_detector_params("vox_octave_pb_edge_detector");

    if (!params->parse_command_line_args(argc, argv))
        return 1;

    //: always print the params file if an executable to work with ORL web
    // interface
    if (!params->print_params_xml(params->print_params_file()))
        vcl_cerr << "problems in writing params file to: " <<
        params->print_params_file() << vcl_endl;

    if (params->exit_with_no_processing() || params->print_params_only())
        return 0;

    //: always call this method to actually parse the input parameter file
    // whose name is extracted from the command line
    if (!params->parse_input_xml())
        return 1;


    dbdet_pb_edge_detector_process pb_pro;
    set_process_parameters_of_bpro1(*params, pb_pro, "PB");

    vidpro1_image_storage_sptr image_storage = new vidpro1_image_storage();

    vcl_vector<vil_image_resource_sptr> images_to_process;
    vcl_vector<vcl_string> image_file_names;

    if(params->use_pyramid_())
    {
        vcl_cout << "Pyramid mode: ON" << vcl_endl;
        vcl_string pyramid_dir = params->input_pyramid_dir_();
        vcl_string query_string = pyramid_dir + "/*" + params->input_extension_();
        vcl_cout << "Pyramid query: " << query_string << vcl_endl;
        vul_file_iterator fit(query_string);
        for(; fit; ++fit)
        {
            //load the input image
            vcl_string input_img = pyramid_dir + "/" + fit.filename();

            // Grab image
            vil_image_resource_sptr img_sptr = vil_load_image_resource(input_img.c_str());
            if (!img_sptr)
            {
                vcl_cerr << "Cannot load image: " << input_img << vcl_endl;
                return 1;
            }
            if(img_sptr->ni() >= params->min_width_() && img_sptr->nj() >= params->min_height_() &&
                    img_sptr->ni() <= params->max_width_() && img_sptr->nj() <= params->max_height_()
            )
            {
                images_to_process.push_back(img_sptr);
                image_file_names.push_back(input_img);
                vcl_cout << "Image added: " << input_img << vcl_endl;
            }
        }
    }
    else
    {
        vcl_cout << "Pyramid mode: OFF" << vcl_endl;
        //load the input image
        vcl_string input_img = params->input_object_dir_() + "/" + params->input_object_name_() + params->input_extension_();

        if (!vul_file::exists(input_img))
        {
            vcl_cerr << "Cannot find image: " << input_img << vcl_endl;
            return 1;
        }
        // Grab image
        vil_image_resource_sptr img_sptr = vil_load_image_resource(input_img.c_str());
        if (!img_sptr)
        {
            vcl_cerr << "Cannot load image: " << input_img << vcl_endl;
            return 1;
        }
        if(img_sptr->ni() >= params->min_width_() && img_sptr->nj() >= params->min_height_() &&
                img_sptr->ni() <= params->max_width_() && img_sptr->nj() <= params->max_height_()
        )
        {
            images_to_process.push_back(img_sptr);
            image_file_names.push_back(input_img);
            vcl_cout << "Image added: " << input_img << vcl_endl;
        }
    }

    int num_of_img = images_to_process.size();

    if(num_of_img == 0)
    {
        vcl_cerr << "There is no input image to process!" << vcl_endl;
        return 1;
    }

    vcl_vector<bpro1_storage_sptr> pb_results;



    for(int i = 0; i < num_of_img; i++)
    {
        vcl_cout << "Start processing image #" << i+1 << vcl_endl;
        // Before we start the process lets clean input output
        pb_pro.clear_input();
        pb_pro.clear_output();

        image_storage->set_image(images_to_process[i]);

        pb_pro.add_input(image_storage);
        bool pb_status = pb_pro.execute();
        pb_pro.finish();

        // Grab output
        // if process did not fail
        if(pb_status)
        {
            pb_results = pb_pro.get_output();
        }

        if (pb_results.size() != 1 )
        {
            vcl_cerr<< "Pb edge detection failed"<< vcl_endl;
            return 1;
        }

        if(i == 0)
        {
            if(!vul_file_exists(params->output_edgemap_dir_()))
            {
                vul_file::make_directory(params->output_edgemap_dir_());
            }
        }

        bpro1_storage_sptr result_storage = pb_results[0];
        dbdet_edgemap_storage_sptr edgemap_storage;
        edgemap_storage.vertical_cast(result_storage);

        dbdet_edgemap_sptr edgemap_sptr = edgemap_storage->get_edgemap();

        vcl_string image_file_name = image_file_names[i];
        vcl_string current_object_name = vul_file::strip_extension(vul_file::strip_directory(image_file_name));

        vcl_string destination_file = params->output_edgemap_dir_() + "/" + current_object_name + params->edgemap_extension_();

        vcl_cout << "Saving " << destination_file << vcl_endl;

        bool save_status = dbdet_save_edg(destination_file, edgemap_sptr);

        if(!save_status)
        {
            vcl_cerr<< "Saving edges has failed."<< vcl_endl;
            return 1;
        }
        vcl_cout << "Finish processing image #" << i+1 << vcl_endl;
    }

    pb_pro.clear_input();
    pb_pro.clear_output();


    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;

    // Just to be safe lets flush everything
    vcl_cerr.flush();
    vcl_cout.flush();

    //Success we made it this far
    return 0;
}


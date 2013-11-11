// This is brcv/rec/dborl/algo/vox_octave_create_image_pyramid/vox_octave_create_image_pyramid_main.cxx

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Feb 24, 2010
//



#include "vox_octave_create_image_pyramid_params.h"
#include "vox_octave_create_image_pyramid_params_sptr.h"

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>

#include <dbil/algo/dbil_octave.h>

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    vox_octave_create_image_pyramid_params_sptr params =
            new vox_octave_create_image_pyramid_params("vox_octave_create_image_pyramid");

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

    //load the input image
    vcl_string input_img = params->input_object_dir_() + "/"
            + params->input_object_name_() + params->input_extension_();

    // Grab image
    vil_image_resource_sptr img_sptr =
            vil_load_image_resource(input_img.c_str());
    if (!img_sptr)
    {
        vcl_cerr << "Cannot load image: " << input_img << vcl_endl;
        return 1;
    }
    vil_image_view<vxl_byte> image_view = img_sptr->get_view();
    double step = vcl_pow(params->step_base_(), params->step_power_());

    vcl_vector<vil_image_resource_sptr> vec =
            dbil_octave_create_image_pyramid(image_view, params->interp_type_(), step, params->num_steps_());

    int total_digits = vcl_floor(vcl_log10(params->num_steps_()));

    for(int i = 0; i < vec.size(); i++)
    {
        int index = i + 1;
        vcl_string out_file_name = params->output_pyramid_folder_() + "/" + params->input_object_name_() + "_";
        int number_of_digits = total_digits - vcl_floor(vcl_log10(index));
        for(int j = 0; j < number_of_digits; j++)
        {
            out_file_name += "0";
        }
        vcl_stringstream ss(vcl_stringstream::in | vcl_stringstream::out);
        ss << out_file_name;
        ss << index << params->output_extension_();
        ss >> out_file_name;
        vcl_cout << "Outfile: " << out_file_name << vcl_endl;
        vcl_string output_dir = vul_file::dirname(out_file_name);
        if(!vul_file_exists(output_dir))
        {
            vul_file::make_directory(output_dir);
        }
        vil_save(*(vec[i]->get_view()), out_file_name.c_str());
    }

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


//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 29, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include "vox_convert_and_associate_pb_output_params.h"
#include "vox_convert_and_associate_pb_output_params_sptr.h"

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>
#include <dbul/dbul_dir_file.h>
#include <vpl/vpl.h>

#include <dbdet/algo/dbdet_load_edg.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

#include <dbil/algo/dbil_octave.h>
#include <vil/vil_load.h>

dbdet_edgemap_sptr vox_convert_pb_outputs_to_edgemap(dbul_octave_double_array& pb_array, dbul_octave_double_array& theta_array)
{
    dim_vector dv = pb_array.dims();
    int height = dv(0);
    int width = dv(1);
    dbdet_edgemap_sptr edgemap_ptr = new dbdet_edgemap(width, height);
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            double strength = pb_array(y, x);
            if(strength > 0)
            {
                vgl_point_2d<double> loc(x, y);
                vgl_point_2d<int> loc2(x, y);
                dbdet_edgel* new_edgel = new dbdet_edgel(loc, theta_array(y, x), strength, 0, 0);
                new_edgel->gpt = loc2;
                edgemap_ptr->insert(new_edgel, x, y);
            }
        }
    }
    return edgemap_ptr;
}

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    vox_convert_and_associate_pb_output_params_sptr params =
            new vox_convert_and_associate_pb_output_params("vox_convert_and_associate_pb_output");

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

    vcl_string object_name = params->input_object_name_();
    vcl_string input_file_name = params->input_edgemap_folder_list_file_();

    vcl_ifstream xlists(input_file_name.c_str());

    vcl_string line;

    if (xlists.is_open())
    {
        while (! xlists.eof() )
        {
            getline(xlists, line);
            vcl_string query_object_name = line.substr(0, object_name.length());
            if(object_name == query_object_name)
            {
                vcl_string pb_output_folder = line.substr(object_name.length() + 1);
                vcl_cout << "Folder to be processed: " << pb_output_folder << vcl_endl;

                if(!vul_file_exists(params->output_edgemap_dir_()))
                {
                    vul_file::make_directory(params->output_edgemap_dir_());
                }

                vul_file_iterator fit(pb_output_folder + "/*" + params->input_edgemap_extension_());
                for(; fit; ++fit)
                {
                    vcl_string edgemap_file = fit.filename();
                    vcl_string objname = edgemap_file.substr(0, edgemap_file.length() - params->input_edgemap_extension_().length());
                    vcl_string edgeorient_file = pb_output_folder + "/"+ objname + params->input_edgeorient_extension_();
                    edgemap_file = pb_output_folder + "/" + edgemap_file;
                    vil_image_view<vxl_byte> edgemap_image = vil_load(edgemap_file.c_str());
                    if(edgemap_image.ni() < params->min_width_() || edgemap_image.nj() < params->min_height_())
                    {
                        vcl_cout << "Skip: " << objname << vcl_endl;
                        continue;
                    }
                    /////
                    dbul_octave_argument_list conversion_inargs;
                    dbul_octave_argument_list conversion_outargs;

                    dbul_octave_value octave_image = dbil_convert_image_view_to_octave_value<vxl_byte>(edgemap_image);
                    conversion_inargs(0) = octave_image;
                    dbul_octave.run("","im2double", conversion_inargs, conversion_outargs);
                    dbul_octave_double_array pb_array = dbul_octave_value_to_octave_double_array(conversion_outargs(0));
                    /////

                    /////
                    dbul_octave_argument_list inargs;
                    dbul_octave_argument_list outargs;

                    inargs(0) = edgeorient_file;
                    dbul_octave.run("","load", inargs, outargs);
                    dbul_octave_double_array theta_array = dbul_octave_value_to_octave_double_array(outargs(0));
                    /////
                    dbdet_edgemap_sptr EM = vox_convert_pb_outputs_to_edgemap(pb_array, theta_array);
                    vcl_string dest_file_name = params->output_edgemap_dir_() + "/" + objname + params->output_edgemap_extension_();
                    vcl_cout << "Assoc file:" << dest_file_name << vcl_endl;
                    bool save_status = dbdet_save_edg(dest_file_name, EM);

                    if(!save_status)
                    {
                        vcl_cerr<< "Saving edges has failed."<< vcl_endl;
                        return 1;
                    }
                }
                break;
            }
        }
    }
    else
    {
        vcl_cerr << "Error: cannot open file: " << input_file_name << vcl_endl;
        return 1;
    }

    xlists.close();


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


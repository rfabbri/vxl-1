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

#include "vox_convert_and_associate_kovesi_output_params.h"
#include "vox_convert_and_associate_kovesi_output_params_sptr.h"

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>
#include <vpl/vpl.h>

#include <dbdet/algo/dbdet_load_edg.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <dbdet/algo/dbdet_cem_file_io.h>

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    vox_convert_and_associate_kovesi_output_params_sptr params =
            new vox_convert_and_associate_kovesi_output_params("vox_convert_and_associate_kovesi_output");

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
    vcl_string input_file_name = params->input_kovesi_folder_list_file_();

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
                vcl_string kovesi_output_folder = line.substr(object_name.length() + 1);
                vcl_cout << "Folder to be processed: " << kovesi_output_folder << vcl_endl;

                if(!vul_file_exists(params->output_linked_edge_dir_()))
                {
                    vul_file::make_directory(params->output_linked_edge_dir_());
                }

                vul_file_iterator fit(kovesi_output_folder + "/*" + params->input_cem_extension_());
                for(; fit; ++fit)
                {
                    vcl_string cem_file = fit.filename();
                    vcl_string objname = cem_file.substr(0, cem_file.length() - params->input_cem_extension_().length());
                    vcl_string edgemap_file = kovesi_output_folder + "/"+ objname + params->input_edgeimg_extension_();
                    cem_file = kovesi_output_folder + "/" + cem_file;
                    vil_image_view<vxl_byte> edgemap_image = vil_load(edgemap_file.c_str());
                    if(edgemap_image.ni() < params->min_width_() || edgemap_image.nj() < params->min_height_())
                    {
                        vcl_cout << "Skip: " << objname << vcl_endl;
                        continue;
                    }
                    vcl_ifstream cems(cem_file.c_str());
                    dbdet_curve_fragment_graph out_CFG;
                    dbdet_edgemap_sptr out_EM = dbdet_load_cem_v1(cems, out_CFG, edgemap_image.ni(), edgemap_image.nj(), false);

                    vcl_string dest_file_name1 = params->output_linked_edge_dir_() + "/" + objname + params->output_edgemap_extension_();
                    vcl_cout << "Assoc file:" << dest_file_name1 << vcl_endl;
                    bool save_status = dbdet_save_edg(dest_file_name1, out_EM);
                    if(!save_status)
                    {
                        vcl_cerr<< "Saving edges has failed."<< vcl_endl;
                        return 1;
                    }

                    vcl_string dest_file_name2 = params->output_linked_edge_dir_() + "/" + objname + params->output_cem_extension_();
                    vcl_cout << "Assoc file:" << dest_file_name2 << vcl_endl;
                    save_status = dbdet_save_cem(dest_file_name2, out_EM, out_CFG);
                    if(!save_status)
                    {
                        vcl_cerr<< "Saving cem has failed."<< vcl_endl;
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


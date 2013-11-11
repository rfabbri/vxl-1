// This is /lemsvxl/brcv/rec/dborl/algo/vox_octave_create_image_pyramid/vox_octave_kovesi_edge_linker_main.cxx

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 2, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include "vox_octave_kovesi_edge_linker_params.h"
#include "vox_octave_kovesi_edge_linker_params_sptr.h"

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <dbdet/algo/dbdet_load_edg.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/algo/dbdet_cem_file_io.h>

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    vox_octave_kovesi_edge_linker_params_sptr params =
            new vox_octave_kovesi_edge_linker_params("vox_octave_kovesi_edge_linker");

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




    dbdet_kovesi_edge_linker_process kovesi_pro;
    set_process_parameters_of_bpro1(*params, kovesi_pro, "kovesi");

    dbdet_edgemap_storage_sptr edgemap_storage = new dbdet_edgemap_storage();

    vcl_vector<dbdet_edgemap_sptr> edgemaps_to_process;
    vcl_vector<vcl_string> edgemap_file_names;
    vcl_string input_edgemap_dir = params->input_edgemap_dir_();
    dbdet_edgemap_sptr input_em;
    if(params->use_pyramid_())
    {
        vcl_cout << "Pyramid mode: ON" << vcl_endl;
        vcl_string query_string = input_edgemap_dir + "/*" + params->input_edgemap_extension_();
        vcl_cout << "Pyramid query: " << query_string << vcl_endl;
        vul_file_iterator fit(query_string);
        for(; fit; ++fit)
        {
            //load the input edge map
            vcl_string input_em_file = input_edgemap_dir + "/" + fit.filename();

            bool load_status = dbdet_load_edg(input_em_file, true, 1.0, input_em);

            if (!load_status)
            {
                vcl_cerr << "Cannot load edge map: " << input_em_file << vcl_endl;
                return 1;
            }

            edgemaps_to_process.push_back(input_em);
            edgemap_file_names.push_back(input_em_file);
            vcl_cout << "Edge map added: " << input_em_file << vcl_endl;
        }
    }
    else
    {
        vcl_cout << "Pyramid mode: OFF" << vcl_endl;
        //load the input edge map
        vcl_string input_em_file  = input_edgemap_dir + "/" + params->input_object_name_() + params->input_edgemap_extension_();

        bool load_status = dbdet_load_edg(input_em_file, true, 1.0, input_em);

        if (!load_status)
        {
            vcl_cerr << "Cannot load edge map: " << input_em_file << vcl_endl;
            return 1;
        }

        edgemaps_to_process.push_back(input_em);
        edgemap_file_names.push_back(input_em_file);
        vcl_cout << "Edge map added: " << input_em_file << vcl_endl;
    }

    int num_of_img = edgemap_file_names.size();

    if(num_of_img == 0)
    {
        vcl_cerr << "There is no input edgemap to process!" << vcl_endl;
        return 1;
    }

    vcl_vector<bpro1_storage_sptr> kovesi_results;



    for(int i = 0; i < num_of_img; i++)
    {
        vcl_cout << "Start processing edgemap #" << i+1 << vcl_endl;
        // Before we start the process lets clean input output
        kovesi_pro.clear_input();
        kovesi_pro.clear_output();

        edgemap_storage->set_edgemap(edgemaps_to_process[i]);

        kovesi_pro.add_input(edgemap_storage);
        bool kovesi_status = kovesi_pro.execute();
        kovesi_pro.finish();

        // Grab output
        // if process did not fail
        if(kovesi_status)
        {
            kovesi_results = kovesi_pro.get_output();
        }

        if (kovesi_results.size() != 1 )
        {
            vcl_cerr<< "Kovesi edge linking failed"<< vcl_endl;
            return 1;
        }

        if(i == 0)
        {
            if(!vul_file_exists(params->output_linked_edge_dir_()))
            {
                vul_file::make_directory(params->output_linked_edge_dir_());
            }
        }

        bpro1_storage_sptr result_storage = kovesi_results[0];
        dbdet_sel_storage_sptr sel_storage;
        sel_storage.vertical_cast(result_storage);

        dbdet_edgemap_sptr output_edgemap_sptr = sel_storage->EM();
        dbdet_curve_fragment_graph& output_CFG = sel_storage->CFG();

        vcl_string edgemap_file_name = edgemap_file_names[i];
        vcl_string current_object_name = vul_file::strip_extension(vul_file::strip_directory(edgemap_file_name));

        vcl_string em_destination_file = params->output_linked_edge_dir_() + "/" + current_object_name + params->output_edgemap_extension_();
        vcl_string cem_destination_file = params->output_linked_edge_dir_() + "/" + current_object_name + params->output_cemv_extension_();

        vcl_cout << "Saving " << em_destination_file << vcl_endl;

        bool save_status = dbdet_save_edg(em_destination_file, output_edgemap_sptr);

        if(!save_status)
        {
            vcl_cerr<< "Saving edges has failed."<< vcl_endl;
            return 1;
        }

        vcl_cout << "Saving " << cem_destination_file << vcl_endl;

        save_status = dbdet_save_cem(cem_destination_file, output_edgemap_sptr, output_CFG);

        if(!save_status)
        {
            vcl_cerr<< "Saving cem has failed."<< vcl_endl;
            return 1;
        }

        vcl_cout << "Finish processing edgemap #" << i+1 << vcl_endl;
    }

    kovesi_pro.clear_input();
    kovesi_pro.clear_output();


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


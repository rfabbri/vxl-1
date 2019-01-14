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

#include <iostream>
#include <sstream>
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
        std::cerr << "problems in writing params file to: " <<
        params->print_params_file() << std::endl;

    if (params->exit_with_no_processing() || params->print_params_only())
        return 0;

    //: always call this method to actually parse the input parameter file
    // whose name is extracted from the command line
    if (!params->parse_input_xml())
        return 1;




    dbdet_kovesi_edge_linker_process kovesi_pro;
    set_process_parameters_of_bpro1(*params, kovesi_pro, "kovesi");

    dbdet_edgemap_storage_sptr edgemap_storage = new dbdet_edgemap_storage();

    std::vector<dbdet_edgemap_sptr> edgemaps_to_process;
    std::vector<std::string> edgemap_file_names;
    std::string input_edgemap_dir = params->input_edgemap_dir_();
    dbdet_edgemap_sptr input_em;
    if(params->use_pyramid_())
    {
        std::cout << "Pyramid mode: ON" << std::endl;
        std::string query_string = input_edgemap_dir + "/*" + params->input_edgemap_extension_();
        std::cout << "Pyramid query: " << query_string << std::endl;
        vul_file_iterator fit(query_string);
        for(; fit; ++fit)
        {
            //load the input edge map
            std::string input_em_file = input_edgemap_dir + "/" + fit.filename();

            bool load_status = dbdet_load_edg(input_em_file, true, 1.0, input_em);

            if (!load_status)
            {
                std::cerr << "Cannot load edge map: " << input_em_file << std::endl;
                return 1;
            }

            edgemaps_to_process.push_back(input_em);
            edgemap_file_names.push_back(input_em_file);
            std::cout << "Edge map added: " << input_em_file << std::endl;
        }
    }
    else
    {
        std::cout << "Pyramid mode: OFF" << std::endl;
        //load the input edge map
        std::string input_em_file  = input_edgemap_dir + "/" + params->input_object_name_() + params->input_edgemap_extension_();

        bool load_status = dbdet_load_edg(input_em_file, true, 1.0, input_em);

        if (!load_status)
        {
            std::cerr << "Cannot load edge map: " << input_em_file << std::endl;
            return 1;
        }

        edgemaps_to_process.push_back(input_em);
        edgemap_file_names.push_back(input_em_file);
        std::cout << "Edge map added: " << input_em_file << std::endl;
    }

    int num_of_img = edgemap_file_names.size();

    if(num_of_img == 0)
    {
        std::cerr << "There is no input edgemap to process!" << std::endl;
        return 1;
    }

    std::vector<bpro1_storage_sptr> kovesi_results;



    for(int i = 0; i < num_of_img; i++)
    {
        std::cout << "Start processing edgemap #" << i+1 << std::endl;
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
            std::cerr<< "Kovesi edge linking failed"<< std::endl;
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

        std::string edgemap_file_name = edgemap_file_names[i];
        std::string current_object_name = vul_file::strip_extension(vul_file::strip_directory(edgemap_file_name));

        std::string em_destination_file = params->output_linked_edge_dir_() + "/" + current_object_name + params->output_edgemap_extension_();
        std::string cem_destination_file = params->output_linked_edge_dir_() + "/" + current_object_name + params->output_cemv_extension_();

        std::cout << "Saving " << em_destination_file << std::endl;

        bool save_status = dbdet_save_edg(em_destination_file, output_edgemap_sptr);

        if(!save_status)
        {
            std::cerr<< "Saving edges has failed."<< std::endl;
            return 1;
        }

        std::cout << "Saving " << cem_destination_file << std::endl;

        save_status = dbdet_save_cem(cem_destination_file, output_edgemap_sptr, output_CFG);

        if(!save_status)
        {
            std::cerr<< "Saving cem has failed."<< std::endl;
            return 1;
        }

        std::cout << "Finish processing edgemap #" << i+1 << std::endl;
    }

    kovesi_pro.clear_input();
    kovesi_pro.clear_output();


    double vox_time = t.real()/1000.0;
    t.mark();
    std::cout<<std::endl;
    std::cout<<"************ Time taken: "<<vox_time<<" sec"<<std::endl;

    // Just to be safe lets flush everything
    std::cerr.flush();
    std::cout.flush();

    //Success we made it this far
    return 0;
}


// This is /lemsvxl/brcv/rec/dborl/algo/vox_convert_edg_to_img_and_orient/vox_convert_edg_to_img_and_orient_main.cxx

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 9, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include "vox_convert_edg_to_img_and_orient_params.h"
#include "vox_convert_edg_to_img_and_orient_params_sptr.h"

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vul/vul_file.h>

#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>
#include <vul/vul_file_iterator.h>

#include <dbdet/algo/dbdet_convert_edgemap.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/edge/dbdet_edgemap.h>

#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

#define ARRAY_ELEMENT(A, w, i, j) A[w*i + j]

bool dborl_write_edge_orient_to_file(const vcl_string& filename, dbdet_edgemap_sptr& in_EM)
{
    vcl_ofstream ems(filename.c_str());
    if(!ems.is_open())
    {
        vcl_cerr << "ERROR: Edgemap file cannot be opened!" << vcl_endl;
        return false;
    }

    int height = in_EM->height();
    int width = in_EM->width();

    double* orient_array = new double[height * width];

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            ARRAY_ELEMENT(orient_array, width, i, j) = 0;
        }
    }

    vcl_vector<dbdet_edgel*> edgels = in_EM->edgels;
    for(int i = 0; i < edgels.size(); i++)
    {
        dbdet_edgel* e = edgels[i];
        const vgl_point_2d<int>& gpt = (e->gpt);
        ARRAY_ELEMENT(orient_array, width, gpt.y(), gpt.x()) = e->tangent;
    }

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            ems << ARRAY_ELEMENT(orient_array, width, i, j);
            if(j != width-1)
            {
                ems << " ";
            }
        }
        if(i != height-1)
        {
            ems << vcl_endl;
        }
    }
    ems.close();
    delete[] orient_array;
    return true;
}

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    vox_convert_edg_to_img_and_orient_params_sptr params =
            new vox_convert_edg_to_img_and_orient_params("vox_convert_edg_to_img_and_orient");

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

    vcl_vector<dbdet_edgemap_sptr> edgemaps_to_process;
    vcl_vector<vcl_string> edgemap_file_names;
    vcl_string input_edgemap_dir;
    vcl_string input_edgemap_extension;
    if(params->use_linked_())
    {
        input_edgemap_dir = params->input_linked_edgemap_dir_();
        input_edgemap_extension = params->input_linked_edgemap_extension_();
    }
    else
    {
        input_edgemap_dir = params->input_edgemap_dir_();
        input_edgemap_extension = params->input_edgemap_extension_();
    }

    dbdet_edgemap_sptr input_em;
    if(params->use_pyramid_())
    {
        vcl_cout << "Pyramid mode: ON" << vcl_endl;
        vcl_string query_string = input_edgemap_dir + "/*" + input_edgemap_extension;
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
        vcl_string input_em_file  = input_edgemap_dir + "/" + params->input_object_name_() + input_edgemap_extension;

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

    for(int i = 0; i < num_of_img; i++)
    {
        vcl_cout << "Start processing edgemap #" << i+1 << vcl_endl;
        dbdet_edgemap_sptr em = edgemaps_to_process[i];
        vil_image_view<vxl_byte> edgemap_image;
        bool convert_status;
        if(params->is_gray_())
        {
            convert_status = dbdet_convert_edgemap_to_grayscale_image(*em, edgemap_image);
        }
        else
        {
            convert_status = dbdet_convert_edgemap_to_image(*em, edgemap_image);
        }
        if(!convert_status)
        {
            vcl_cerr << "ERROR: Conversion failed!" << vcl_endl;
            return 1;
        }
        vcl_string output_dir = params->output_edge_image_orient_dir_();
        if(!vul_file_exists(output_dir))
        {
            vul_file::make_directory(output_dir);
        }
        vcl_string edgemap_file_name = edgemap_file_names[i];
        vcl_string current_object_name = vul_file::strip_extension(vul_file::strip_directory(edgemap_file_name));
        vcl_string output_image_file = output_dir + "/" + current_object_name + params->output_edge_image_extension_();
        vcl_string output_orient_file = output_dir + "/" + current_object_name + params->output_edge_orient_extension_();
        bool img_save_status = vil_save(edgemap_image, output_image_file.c_str());
        if(!img_save_status)
        {
            vcl_cerr << "ERROR: Image file " << output_image_file << " cannot be written!" << vcl_endl;
            return 1;
        }
        bool orient_save_status = dborl_write_edge_orient_to_file(output_orient_file, em);
        if(!orient_save_status)
        {
            vcl_cerr << "ERROR: Edge orientation file " << output_orient_file << " cannot be written!" << vcl_endl;
            return 1;
        }
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



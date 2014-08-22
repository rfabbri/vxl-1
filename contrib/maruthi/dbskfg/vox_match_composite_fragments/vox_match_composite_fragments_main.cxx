//:
// \file
// \author Maruthi Narayanan ( mn@lems.brown.edu )
// \date June 17, 2009
// \brief An algorithm to match bag of fragments from compostie data structure
//  
// \verbatim
// \endverbatim


#include "vox_match_composite_fragments_params.h"
#include "vox_match_composite_fragments_params_sptr.h"
#include <dbskfg/pro/dbskfg_match_bag_of_fragments_process.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <dborl/algo/dborl_utilities.h>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>


int main(int argc, char *argv[]) 
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    // construct parameters with the default values;
    vox_match_composite_fragments_params_sptr params = 
        new vox_match_composite_fragments_params(
            "vox_match_composite_fragments");  
  
    // parse the command line arguments
    if (!params->parse_command_line_args(argc, argv))
        return 1;

    //: always print the params file if an executable to work with ORL web 
    // interface
    if (!params->print_params_xml(params->print_params_file()))
    {
        vcl_cerr << "problems in writing params file to: " 
                 << params->print_params_file() << vcl_endl;
    }

    // exit if there is nothing else to do
    if (params->exit_with_no_processing() || params->print_params_only())
    {
        return 0;
    }

    //: always call this method to actually parse the input parameter file 
    // whose name is extracted from the command line
    if (!params->parse_input_xml())
    {
        return 1;
    }

    vcl_string model_img=params->model_image_();
    vcl_string query_img=params->query_image_();
 
    // Grab image
    vil_image_resource_sptr model_img_sptr = 
        vil_load_image_resource(model_img.c_str());

    vil_image_resource_sptr query_img_sptr = 
        vil_load_image_resource(query_img.c_str());

    // Create vid pro storage
    vidpro1_image_storage_sptr model_img_storage = new vidpro1_image_storage();
    model_img_storage->set_image(model_img_sptr);

    vidpro1_image_storage_sptr query_img_storage = new vidpro1_image_storage();
    query_img_storage->set_image(query_img_sptr);

    // Matching a model set of fragments to a query set of fragments

    //******************** Match Bag of Fragments ****************************
    vcl_cout<<"************  Match Bag of Fragments  *************"<<vcl_endl;

    bool proc_status(false);
    


    //******************** Extract Cgraph Fragments ****************************
    vcl_cout<<"************ Match Medial Fragments *************"<<vcl_endl;
    
    dbskfg_match_bag_of_fragments_process matchsg_pro;
    set_process_parameters_of_bpro1(*params, 
                                    matchsg_pro, 
                                    params->tag_match_composite_fragments_);  
    set_process_parameters_of_bpro1(*params, 
                                    matchsg_pro, 
                                    "edit_data");  

    // Clear input output
    matchsg_pro.clear_input();
    matchsg_pro.clear_output();
   
    matchsg_pro.add_input(model_img_storage);
    matchsg_pro.add_input(query_img_storage);

    // No input needed just call process
    bool matchsg_status = matchsg_pro.execute();
    matchsg_pro.finish();
       
    //Clean up after ourselves
    matchsg_pro.clear_input();
    matchsg_pro.clear_output();
  
    if (!matchsg_status) 
    {
        vcl_cerr << "Matching of Composite Fragments Failed"
                 << vcl_endl;
        return false;
    }
  
    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;


    return 0;
}


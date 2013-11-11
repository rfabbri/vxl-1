//:
// \file



#include "georegister_cmd_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>

#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <pro/dbrl_edge_point_tangent_bg_model_process.h>
#include <pro/dbrl_edge_point_tangent_fg_detect_process.h>

//: Constructor
georegister_cmd_params::
georegister_cmd_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name),
    tag_third_order_color_("Third_Order_Color"),
    tag_bg_modeling_("BG_Modeling"),
    tag_fg_extraction_("FG_Extraction")
{ 
    //: passes the folder of the input object
    this->input_object_dir_.
	set_values(this->param_list_, 
		   "io", "input_object_dir",
		   "input object folder", "", 
		   "./input",
		   0, // for 0th input object
		   dborl_parameter_system_info::INPUT_OBJECT_DIR);

    // add the parameters of the third order color edge detector process
    dbdet_third_order_color_edge_detector_process pro1;
    vcl_vector<bpro1_param*> pars = pro1.parameters()->get_param_list();
    for(unsigned i=0; i<pars.size(); ++i)
    {
	this->param_list_.push_back(convert_parameter_from_bpro1(tag_third_order_color_,
								 "["+tag_third_order_color_+"]",
								 pars[i]));

    }

    // add the parameters of the background modeling process
    dbrl_edge_point_tangent_bg_model_process pro2;
    pars = pro2.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++) 
    {
	this->param_list_.push_back(convert_parameter_from_bpro1(tag_bg_modeling_, 
								 "[" + tag_bg_modeling_ + "]",
								 pars[i]));
    }

    // add the parameters of the foreground extraction process
    dbrl_edge_point_tangent_fg_detect_process pro3;
    pars = pro3.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++) 
    {
	this->param_list_.push_back(convert_parameter_from_bpro1(tag_fg_extraction_, 
								 "[" + tag_fg_extraction_ + "]",
								 pars[i]));
    }

 
}


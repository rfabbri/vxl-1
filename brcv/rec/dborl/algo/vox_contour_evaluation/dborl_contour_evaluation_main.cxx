
#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>
#include <vidpro1/process/vidpro1_save_image_process.h>

#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_sel_process.h>

#include <dbdet/pro/dbdet_evaluation_process.h>

#include <dbdet/pro/dbdet_sel_extract_contours_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>
#include <dbdet/pro/dbdet_prune_curves_process.h>
#include <dbdet/pro/dbdet_save_edg_process.h>
#include <dbdet/pro/dbdet_load_cem_process.h>
#include <dbdet/pro/dbdet_load_edg_process.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_save_cvlet_map_process.h>
#include <dbdet/pro/dbdet_convert_edgemap_to_image_process.h>

int main(int argc, char *argv[]) {
    // Let time how long this takes
    // Start timer
    vul_timer t;
    /*dborl_contour_evaluation_params_sptr params = 
        new dborl_contour_evaluation_params("dborl_contour_evaluation");*/
//****************************** Load the cem file **************************************

    vcl_cout<<"************ Load CEM file ************"<<vcl_endl;
    vcl_string cem_file_1, cem_file_2,file_append;
    
    //params->output_image_extension_() == ".bmp";
    cem_file_1 = argv[1];

    bpro1_filepath cem_path_1(cem_file_1, ".cem");
    vcl_vector<bpro1_storage_sptr> load_cem_results_1;
    dbdet_load_cem_process load_cem_pro_1;
    load_cem_pro_1.parameters()->set_value("-cem_filename",cem_path_1);
    // Before we start the process lets clean input output
    load_cem_pro_1.clear_input();
    load_cem_pro_1.clear_output();

    // Kick of process
    bool load_cem_status_1 = load_cem_pro_1.execute();
    load_cem_pro_1.finish();

	if ( load_cem_status_1 )
	{
	    load_cem_results_1 = load_cem_pro_1.get_output();
	}

    //Clean up after ourselves
    load_cem_pro_1.clear_input();

    if ( !load_cem_status_1 )
    {
        vcl_cerr << "Problems in loading GT cem file" 
                 << cem_file_1 << vcl_endl;
        return 1;

    }

    //params->output_image_extension_() == ".bmp";
    cem_file_2 = argv[2];
    file_append = argv[3];
    bpro1_filepath cem_path_2(cem_file_2, ".cem");
    vcl_vector<bpro1_storage_sptr> load_cem_results_2;
    dbdet_load_cem_process load_cem_pro_2;
    load_cem_pro_2.parameters()->set_value("-cem_filename",cem_path_2);
    // Before we start the process lets clean input output
    load_cem_pro_2.clear_input();
    load_cem_pro_2.clear_output();

    // Kick of process
    bool load_cem_status_2 = load_cem_pro_2.execute();
    load_cem_pro_2.finish();

	if ( load_cem_status_2 )
	{
	    load_cem_results_2 = load_cem_pro_2.get_output();
	}

    //Clean up after ourselves
    load_cem_pro_2.clear_input();

    if ( !load_cem_status_2 )
    {
        vcl_cerr << "Problems in loading computed cem file" 
                 << cem_file_2 << vcl_endl;
        return 1;

    }

    vcl_cout << "Start evaluation" << vcl_endl;
        dbdet_evaluation_process ev_pro;
        ev_pro.call_in_gui = 0;
	ev_pro.get_parameters();

	if(argc >=4)
	{
		ev_pro.bprune_cfg_1 = true;
		double len_th = atof(argv[3]);
		ev_pro.set_len_thresh_1(len_th);
	}

	if(argc >=5)
	{
		double str_th = atof(argv[4]);
		ev_pro.set_str_thresh_1(str_th);
	}
	//ev_pro.set_cost_thresh(2);
	//double e_th = atof(argv[3]);
	//ev_pro.set_edit_thresh(e_th);
        /*set_process_parameters_of_bpro1(*params, 
                                        ev_pro, 
                                        params->tag_contour_evaluation_); */
        // Before we start the process lets clean input output
        ev_pro.clear_input();
        ev_pro.clear_output();

        // Use input from load cems
        ev_pro.add_input(load_cem_results_1[1]);
        ev_pro.add_input(load_cem_results_2[1]);
	vcl_cout << "finish input cem files"<<vcl_endl;
        bool ev_status = ev_pro.execute();
        //double Precision = ev_pro.Precision;
	//double Recall = ev_pro.Recall;
	double TP_gt_l = ev_pro.TP_gt_l;
	double TP_cp_l = ev_pro.TP_cp_l;
	double gt_length = ev_pro.gt_length;
	double cp_length = ev_pro.cp_length;
	double Precision = TP_cp_l/cp_length;
	double Recall = TP_gt_l/gt_length;
	double F_measure = 2*Precision*Recall/(Precision+Recall);
	vcl_cout<<"Precision: " <<Precision <<" Recall: " << Recall <<" F Measure: "<< F_measure << vcl_endl;

	vcl_ofstream filestr;
        size_t found = cem_file_1.find_last_of("/");
	vcl_string name_1 = cem_file_1.substr(cem_file_1.size()-6,2);
	vcl_string name_2 = cem_file_2.substr(cem_file_2.size()-6,2);

	vcl_string output_file;
        output_file=file_append+"_PR_stats.txt";
        // if(cem_file_2.find("GEN")!=vcl_string::npos)
        // 	output_file = cem_file_1.substr(0,found+1)+ name_1 + "_"+file_append+"_GEN_PR_result.txt";
        // else if(cem_file_2.find("SEL")!=vcl_string::npos)
	// 	output_file = cem_file_1.substr(0,found+1)+ name_1 + "_"+file_append+"_SEL_PR_result.txt";
        // else if(cem_file_2.find("Kovesi")!=vcl_string::npos)
	// 	output_file = cem_file_1.substr(0,found+1)+ name_1 + "_"+file_append+"_Kovesi_PR_result.txt";
        // else if(cem_file_2.find("VD")!=vcl_string::npos)
	// 	output_file = cem_file_1.substr(0,found+1)+ name_1 + "_"+file_append+"_VD_PR_result.txt";
	// else
	// 	output_file = cem_file_1.substr(0,found+1)+ name_1 + "_" + name_2 + "_"+file_append+"_PR_result.txt";

	vcl_cout<<output_file<<vcl_endl;
	filestr.open (output_file.c_str(), vcl_ofstream::app);

	// >> i/o operations here <<
        filestr << TP_gt_l <<" "<< gt_length <<" "<< TP_cp_l <<" " << cp_length << " " << Precision << " "<< Recall << " " << ev_pro.total_edit_dist <<" " << 	F_measure<<vcl_endl;

	filestr.close();
	
        ev_pro.finish();

        ev_pro.clear_input();
        ev_pro.clear_output();
	load_cem_pro_1.clear_output();
	load_cem_pro_2.clear_output();
}

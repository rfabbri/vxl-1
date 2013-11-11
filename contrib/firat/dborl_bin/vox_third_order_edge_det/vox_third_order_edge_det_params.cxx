//:
// \file


#include "vox_third_order_edge_det_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <dborl/algo/dborl_utilities.h>


#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_sel_process.h>
#include <dbdet/pro/dbdet_sel_extract_contours_process.h>
#include <dbdet/pro/dbdet_prune_curves_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>

//: Constructor
vox_third_order_edge_det_params::
vox_third_order_edge_det_params(vcl_string algo_name) :
dborl_algo_params(algo_name),
tag_gray_edge_detection_("Edge_Detection_Gray"),
tag_color_edge_detection_("Edge_Detection_Color"),
tag_edge_linking_("Edge_Linking"),
tag_extract_contours_("Extract_Contours"),
tag_prune_contours_("Prune_Contours")
{ 

    //: Save the result intrinsic shock?
    this->save_edges_.set_values(this->param_list_, "io", "save_edges",
            "-io: save edge detection result ?", false, false);

    //: Save the curvlets, intermediate step of symbolic edge linking
    this->save_curvelets_.set_values(this->param_list_, "io", "save_curvelets",
            "-io: save curvelets ?", false, false);

    // Save result to the object folder?
    this->save_to_object_folder_.
    set_values(this->param_list_,
            "io",
            "save_to_object_folder",
            "-io: save result to object folder ?", true, true);

    //: Name of input object
    this->input_object_name_.set_values(
            this->param_list_,
            "io", "input_object_name",
            "input_object_name", "horse001", "horse001",
            0, // for 0th input object
            dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    //: passes the folder of the input object
    this->input_object_dir_.set_values(this->param_list_,
            "io", "input_object_dir",
            "input object folder", "",
            "/vision/projects/kimia/categorization/vox/weizmann_gray",
            0, // for 0th input object
            dborl_parameter_system_info::
            INPUT_OBJECT_DIR);

    //: extension of the image file
    this->input_extension_.set_values(this->param_list_, "io", "input_extention",
            "-io: input image extension", ".png", ".png");

    //: extension of the image file
    this->output_edgeorient_extension_.set_values(this->param_list_, "io", "output_edgeorient_extension",
            "-io: output edgeorient extension", "_3rd_order_orient.txt", "_3rd_order_orient.txt");

    //: extension of the image file
    this->output_edgemap_extension_.set_values(this->param_list_, "io", "output_edgemap_extension",
            "-io: output edgemap extension", "_3rd_order_edges.png", "_3rd_order_edges.png");

    //: extension of the image file
    this->output_cemv_tag_.set_values(this->param_list_, "io", "output_cemv_tag",
            "-io: output cemv tag", "_3rd_order_linked", "_3rd_order_linked");

    //: extension of output file
    this->output_extension_.
    set_values(this->param_list_, "io",
            "output_extension",
            "-io: output extension of contours (.con,.cem,.cemv) "
            , ".cem", ".cem");

    //: extension of edge file
    this->edge_extension_.
    set_values(this->param_list_, "io",
            "edge_extension",
            "-io: edge extension of edges (.edg) "
            , ".edg", ".edg");

    // Output shock folder (if not object folder)
    this->output_edge_link_folder_.set_values(this->param_list_, "io",
            "output_edge_link_folder",
            "output folder to write computed contour map", "",
            "/vision/scratch/firat/weizmann_contour_maps",
            0, // associated to 0th input object
            dborl_parameter_system_info::OUTPUT_FILE,
            "contour_map",
            dborl_parameter_type_info::FILEASSOC);

    //: extension of edge file
    this->pyramid_folder_.
    set_values(this->param_list_, "io",
            "pyramid_folder",
            "-io: folder that contains image pyramid"
            , "/vision/projects/kimia/shockshape/symseg/results/weizmann_horse/weizmann_pyramids",
            "/vision/projects/kimia/shockshape/symseg/results/weizmann_horse/weizmann_pyramids");

    // prune curve fragments ?
    this->prune_contours_.
    set_values(this->param_list_, "io",
            "prune_contours",
            "-io: prune contours based on contrast ?"
            , false, false);

    //: add the parameters of the dbdet_third_order_edge_detector_process
    dbdet_third_order_edge_detector_process pro1;
    vcl_vector<bpro1_param*> pars = pro1.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++)
    {
        param_list_.push_back(
                convert_parameter_from_bpro1(tag_gray_edge_detection_,
                        "[" + tag_gray_edge_detection_+ "] ",
                        pars[i]));
    }

    //: add the parameters for the dbdet_third_order_color_edge_detector_process
    dbdet_third_order_color_edge_detector_process pro2;
    pars = pro2.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++)
    {
        param_list_.push_back(
                convert_parameter_from_bpro1(tag_color_edge_detection_,
                        "[" + tag_color_edge_detection_+ "] ",
                        pars[i]));
    }

    //: add the parameters for edge linking
    dbdet_sel_process pro3;
    pars = pro3.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++)
    {
        param_list_.push_back(
                convert_parameter_from_bpro1(tag_edge_linking_,
                        "[" + tag_edge_linking_ + "] ",
                        pars[i]));
    }

    //: add the parameters for extract regular contours
    dbdet_sel_extract_contours_process pro4;
    pars = pro4.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++)
    {
        param_list_.push_back(
                convert_parameter_from_bpro1(tag_extract_contours_,
                        "[" + tag_extract_contours_+ "] ",
                        pars[i]));
    }


    //: add the parameter for pruning
    dbdet_prune_curves_process pro5;
    pars = pro5.parameters()->get_param_list();
    for (unsigned i = 0; i < pars.size(); i++)
    {
        param_list_.push_back(
                convert_parameter_from_bpro1(tag_prune_contours_,
                        "[" + tag_prune_contours_+ "] ",
                        pars[i]));
    }

}


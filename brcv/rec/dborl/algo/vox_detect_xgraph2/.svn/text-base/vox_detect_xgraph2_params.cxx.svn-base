// This is rec/dborl/algo/vox_detect_xgraph2/vox_detect_xgraph2_params.cxx

//:
// \file


#include "vox_detect_xgraph2_params.h"
#include <dborl/algo/dborl_algo_params.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <dborl/algo/dborl_search_index_file.h>
//#include <dborl/algo/dborl_utilities.h>

// -----------------------------------------------------------------------------
//: Constructor
vox_detect_xgraph2_params::
vox_detect_xgraph2_params(vcl_string algo_name) :
dborl_algo_params(algo_name)
{

    this->index_filename_.set_values(param_list_,
            "io", "index_filename",
            "[INPUT] path of the index file", "", "/vision/scratch/firat/vox_pyramids/index.xml",
            0,
            dborl_parameter_system_info::NOT_DEFINED,
            "flat_image",
            dborl_parameter_type_info::FILEASSOC);
    // Image //////////////////////////////////////////////////

    // extension of the input image
    this->input_image_extension_.set_values(this->param_list_, "io", "input_image_extention",
            "[DATA] input image extension",
            ".jpg",
            ".jpg");

    // Edgemap and edge orientation /////////////////////////////////////////////

    // Folder of input edgemap (if not in object folder)
    this->input_edgemap_folder_.set_values(this->param_list_, "io", "input_edgemap_folder",
            "[DATA] input folder to read edgemap file",
            //"/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_edgemaps-clean_using_kovesi-I_15-len_10",
            "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_pb_edges_pyramid-clean_using_kovesi-I_15-len_4",
            "v:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_pb_edges_pyramid-clean_using_kovesi-I_15-len_4",
            1, // associated to 0th input object
            dborl_parameter_system_info::NOT_DEFINED,
            "edge_map_image_and_oriention_directory",
            dborl_parameter_type_info::FILEASSOC);

    // extension of the input edgemap
    this->input_edgemap_extension_.set_values(this->param_list_, "io", "input_edgemap_extention",
            "[DATA] input edgemap extension",
            "_linked.png",  //"_edges.tif"
            "_linked.png" //"_edges.tif"
    );

    // extension of the input edge orientation
    this->input_edgeorient_extension_.set_values(this->param_list_, "io", "input_edgeorient_extention",
            "[DATA] input edge orientation extension",
            "_linked_orient.txt", //"_orient.txt",
            "_linked_orient.txt"); //"_orient.txt");


    //: Relative scale of edgemap used for detection.
    // value is in log2 of the scale ratio between of the selected edgemap and the selected image in the pyramid
    this->input_edgemap_log2_scale_ratio_.set_values(this->param_list_, "io", "input_edgemap_log2_scale_ratio",
            "[EDGE] Log2 of relative scale of edgemap used for detection",
            0.5f, // use edgemap at the same level as the image
            0.5f);


    //: Base size of prototype xgraph - all detections will be run on this size
    // \todo This will be detetermined by the ccm xgraph model
    this->prototype_xgraph_base_size_.set_values(this->param_list_, "io", "prototype_xgraph_base_size",
            "[MODEL-SIZE] prototype_xgraph_base_size (sqrt of area)",
            64.0f, // use edgemap at the same level as the image
            64.0f);

    //: Min graph size
    this->prototype_xgraph_min_size_.set_values(this->param_list_, "io", "prototype_xgraph_min_size",
            "[MODEL-SIZE] prototype_xgraph_min_size (sqrt of area)",
            64.0f,
            64.0f);

    //: Max graph size
    this->prototype_xgraph_max_size_.set_values(this->param_list_, "io", "prototype_xgraph_max_size",
            "[MODEL-SIZE] prototype_xgraph_max_size (sqrt of area)",
            1000.0f,
            1000.0f);

    //: Increment step of graph size - computed as log2 of scale ratio
    this->prototype_xgraph_log2_increment_step_.set_values(this->param_list_, "io", "prototype_xgraph_log2_increment_step",
            "[MODEL-SIZE] prototype_xgraph_log2_increment_step, e.g. step (50->100) = 1",
            0.5f,
            0.5f);

    //: Max ratio between largest and smallest model size - in log2
    this->prototype_xgraph_ratio_max_size_to_min_size_.set_values(this->param_list_, "io", "prototype_xgraph_ratio_max_size_to_min_size",
            "[MODEL-SIZE] Max ratio between max-size and min-size",
            9.0f,
            9.0f);


    // Category-spectific info ////////////////////////////////////////////////////////

    //: Object categery of input shock graph
    this->input_model_category_.set_values(this->param_list_, "io", "input_model_category",
            "[INPUT] input model category",
            "mugs",
            "mugs");

    // Name of input object
    this->input_object_name_.set_values(this->param_list_, "io", "input_object_name",
            "[INPUT] input object name",
            "mugs_tdnkitchen",
            "mugs_tdnkitchen",
            1, // for 0th input object
            dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);

    // passes the folder of the input object
    this->input_object_dir_.set_values(this->param_list_, "io", "input_object_dir",
            "[INPUT] input object folder",
            "/vision/projects/kimia/categorization/ethz_shape/vox-upload/mugs_tdnkitchen",
            "v:/projects/kimia/categorization/ethz_shape/vox-upload/mugs_tdnkitchen",
            1, // for 0th input object
            dborl_parameter_system_info::INPUT_OBJECT_DIR);


    //: Folder containing the object shock graph
    this->input_gt_xgraph_label_.set_values(this->param_list_, "io", "input_gt_xgraph_label",
            "[XGRAPH] Groundtruth xgraph VOX label",
            "weizmann_gt_xgraph",
            "weizmann_gt_xgraph");

    // XML filename of object
    this->input_xgraph_prototype_object_.set_values(this->param_list_, "io", "input_xgraph_prototype_object",
            "[XGRAPH] input object whose xgraph will be used",
            "horse001",
            "horse001");

    // option 1: degree-3 root node
    //: xgraph geometric model filename (.xml)
    this->input_xgraph_geom_dir_.set_values(this->param_list_, "io", "input_xgraph_geom_dir",
            "[GEOM] Input xgraph geometric model directory",
            "xgraph_geom_model-mugs-v2.xml",
            "xgraph_geom_model-mugs-v2.xml",
            0, // associated to 0th input object
            dborl_parameter_system_info::NOT_DEFINED,
            "xgraph_geom_model_directory",
            dborl_parameter_type_info::FILEASSOC);

    //: xgraph Contour-Chamfer-Matching model filename (.xml)
    this->input_xgraph_ccm_dir_.set_values(this->param_list_, "io", "input_xgraph_ccm_dir",
            "[CCM] Input xgraph Contour-Chamfer-Matching model directory",
            "mugs-xgraph_ccm_model-v2-2009aug26-chamfer_4.xml",
            "mugs-xgraph_ccm_model-v2-2009aug26-chamfer_4.xml",
            0, // associated to 0th input object
            dborl_parameter_system_info::NOT_DEFINED,
            "xgraph_ccm_model_directory",
            dborl_parameter_type_info::FILEASSOC);

    //: List of contour fragments whose cost will be ignored
    this->input_cfrag_list_to_ignore_.set_values(this->param_list_, "io", "input_cfrag_list_to_ignore",
            "[CCM] Input list of bnd contour fragments to ignore cost (separated by commas)",
            "0-L,0-R",
            "0-L,0-R");


    //: Parameter lambda of Oriented Chamfer Matching
    this->input_wcm_weight_unmatched_.set_values(this->param_list_, "io", "input_wcm_unmatched",
            "[WCM] Weight of cost for unmatched edges in WCM cost computation",
            0.0f,
            0.0f);


    //////////////////////////////////////////////////////////////////////////////
    // Output

    // if written to this folder as opposed to object folder
    this->output_object_folder_.set_values(this->param_list_, "io", "output_object_folder",
            "[OUTPUT] output folder to write results",
            "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments",
            "D:/vision/projects/symseg/xshock/vox-output"/*,
            1, // associated to 0th input object
            dborl_parameter_system_info::OUTPUT_FILE,
            "detect_xgraph_output_directory",
            dborl_parameter_type_info::FILEASSOC*/);

    // tag for this algorithm
    this->algo_abbreviation_ = "detect_xgraph";

    //////////////////////////////////////////////////////////////////////////////
    //>> External constraints

    //: Width of detection window
    this->detection_window_width_.set_values(this->param_list_, "io",
            "detection_window_width",
            "[IMPL] Width of detection window", int(256), int(256)); //int(512), int(512));

    //: Height of detection window
    this->detection_window_height_.set_values(this->param_list_, "io",
            "detection_window_height",
            "[IMPL] Height of detection window", int(256), int(256)); //int(512), int(512));


    //////////////////////////////////////////////////////////////////////////////
    //> Post-process
    //: (Absolute) Minimum confidence value to be accepted as a detection
    this->min_accepted_confidence_.set_values(this->param_list_, "io",
            "min_accepted_confidence",
            "[POSTPROC] Minimum accepted confidence level", 10.0f, 10.0f);

    //: Run non-max suppression based on overlapping?
    this->run_nms_based_on_overlap_.set_values(this->param_list_, "io",
            "run_nms_based_on_overlap",
            "[POSTPROC] Run NMS based on overlap?", true, true);

    //: Minimum overlap between two detection before one is rejected
    this->min_overlap_ratio_for_rejection_.set_values(this->param_list_, "io",
            "min_overlap_ratio_for_rejection",
            "[POSTPROC] Min overlap ratio to be considered overlap during NMS", 0.5f, 0.5f);
}


//------------------------------------------------------------------------------
//: List of boundary contour fragment to ignore
bool vox_detect_xgraph2_params::
parse_input_cfrag_list_to_ignore(vcl_vector<vcl_string >& cfrag_list_to_ignore) const
{
    // clean-up first
    cfrag_list_to_ignore.clear();


    // parse the corresponding parameter
    vcl_string buffer = this->input_cfrag_list_to_ignore_();

    // replace "," by space so that we can use standard parser
    for (unsigned i =0; i < buffer.length(); ++i)
    {
        if (buffer[i] == ',')
        {
            buffer[i] =' ';
        }
    }

    // parse the string using stringstream
    vcl_stringstream ss;
    ss << buffer;

    vcl_string cfrag_desc;
    while (ss >> cfrag_desc)
    {
        cfrag_list_to_ignore.push_back(cfrag_desc);
    }
    return true;
}

//------------------------------------------------------------------------------
//:
vcl_string vox_detect_xgraph2_params::
get_image_file() const
{
    vcl_string image_folder = this->input_object_dir_();
    vcl_string image_file = image_folder + "/" + this->input_object_name_() +
            this->input_image_extension_();
    return image_file;
}


//------------------------------------------------------------------------------  
//:
vcl_string vox_detect_xgraph2_params::
get_xgraph_file() const
{
    dborl_index_query index_query;
    index_query.add_object_name(this->input_xgraph_prototype_object_());
    index_query.add_assoc_file_label(this->input_gt_xgraph_label_());
    index_query.add_assoc_file_type("groundtruth_xgraph_directory");
    vcl_vector<vcl_string> query_result = dborl_get_assoc_file_paths(this->index_filename_(), index_query);

    if(query_result.size() == 1)
    {
        vul_file_iterator fit(query_result[0] + "/*.xml");
        return query_result[0] + "/" + fit.filename();
    }
    else
    {
        vcl_cerr << "\nERROR: Could not load prototype xgraph. Quit now. Query result size = " << query_result.size() << "\n";
        return "";
    }
}


//------------------------------------------------------------------------------
//: xgraph geometric model
vcl_string vox_detect_xgraph2_params::
get_xgraph_geom_file() const
{
    vcl_string xgraph_geom_file = this->input_xgraph_geom_dir_() + "/object_geom_model.xml";
    return xgraph_geom_file;
}

//------------------------------------------------------------------------------
//:
vcl_vector<double > vox_detect_xgraph2_params::
get_xgraph_scales() const
{
    vcl_vector<double > xgraph_scales;
    ////
    //if (this->parse_input_model_scales(xgraph_scales))
    //{
    //  return xgraph_scales;
    //}
    //else
    //{
    //  return vcl_vector<double >();
    //}
    return xgraph_scales;
}


//------------------------------------------------------------------------------
//:
vcl_string vox_detect_xgraph2_params::
get_xgraph_ccm_file() const
{
    vcl_string xgraph_ccm_file = this->input_xgraph_ccm_dir_() + "/object_ccm_overall_model.xml";
    return xgraph_ccm_file;
}


//------------------------------------------------------------------------------
//: list of boundary fragments to ignore
vcl_vector<vcl_string > vox_detect_xgraph2_params::
get_cfrag_list_to_ignore() const
{
    vcl_vector<vcl_string > cfrag_list_to_ignore;
    if (this->parse_input_cfrag_list_to_ignore(cfrag_list_to_ignore))
    {
        return cfrag_list_to_ignore;
    }
    else
    {
        return vcl_vector<vcl_string >();
    }
}


// location of all output files
vcl_string vox_detect_xgraph2_params::
get_output_folder() const
{
    return this->output_object_folder_();
}



//: Name of the prototype xgraph
vcl_string vox_detect_xgraph2_params::
get_xgraph_prototype_name() const
{
    return vul_file::strip_extension(vul_file::strip_directory(this->get_xgraph_file()));
}

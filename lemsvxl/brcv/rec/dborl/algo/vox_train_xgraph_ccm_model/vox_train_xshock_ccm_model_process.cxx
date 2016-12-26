// This is /lemsvxl/brcv/rec/dborl/algo/vox_train_xgraph_geometry_and_ccm_models/vox_train_xgraph_ccm_model/dbsks_train_xshock_ccm_model_process.cxx

//:
// \file

#include "vox_train_xshock_ccm_model_process.h"

#include <bpro1/bpro1_parameters.h>
//#include <vidpro1/storage/vidpro1_image_storage.h>


//#include <vil/vil_convert.h>
//#include <vil/vil_new.h>
//#include <vnl/vnl_math.h>

#include "vox_train_ccm_model.h"


//: Constructor
vox_train_xshock_ccm_model_process::
vox_train_xshock_ccm_model_process()
{
    if (
            //// CCM parameters
            //!parameters()->add("Number of orientation bins for [0,pi]:" , "-nbins_0to_pi", int(18)) ||
            //!parameters()->add("Chamfer distance threshold:", "-distance_threshold", 4) ||
            //!parameters()->add("Distance tolerance due to discretization:", "-distance_tol_near_zero", int(2)) ||
            //!parameters()->add("Orientation threshold:", "-orient_threshold",  float(vnl_math::pi_over_4)) ||
            //!parameters()->add("Orientation tolerance due to discretization:", "-orient_tol_near_zero", float((vnl_math::pi / 18) / 2)) ||
            //
            //// CCM Weights
            //!parameters()->add("Weight - chamfer:", "-weight_chamfer", 0.2f) ||
            //!parameters()->add("Weight - edge orient:", "-weight_edge_orient", 0.4f) ||
            //!parameters()->add("Weight - contour orient:", "-weight_contour_orient", 0.4f) ||
            //!parameters()->add("Local window width:", "-local_window_width", 3.0f) ||

            //// number of data point for groundtruth xgraph
            //!parameters()->add("Number of samples per xgraph:", "-num_samples_per_xgraph", int(100))||
            //!parameters()->add("Random perturbation in x-direction (pixels):", "-perturb_delta_x", float(2)) ||
            //!parameters()->add("Random perturbation in y-direction (pixels):", "-perturb_delta_y", float(2)) ||
            //!parameters()->add("Base size for xgraph (square root of area, in pixels):", "-base_xgraph_size", float(64)) ||

            !parameters()->add("Index file path", "-index_file", bpro1_filepath("")) ||
            !parameters()->add("Input image extension", "-image_ext", vcl_string(".jpg")) ||
            // edge folder

            !parameters()->add("edgemap extension (to add to image name):", "-edgemap_ext", vcl_string("_linked.png")) ||
            !parameters()->add("edgeorient extension (to add to image name):", "-edgeorient_ext", vcl_string("_linked_orient.txt")) ||
            !parameters()->add("Edgemap/orient VOX label", "-edge_vox", vcl_string("weizmann_pyramid_gray_linked_edge_img_orient")) ||
            // Category-specific info
            //!parameters()->add("Object category:", "-category", vcl_string("applelogos")) ||
            !parameters()->add("File containing list of training positive object names", "-list_pos_images", bpro1_filepath("/vision/scratch/firat/weizmann_xshock/gt-objects.txt")) ||
            !parameters()->add("File containing list of training negative object names", "-list_neg_images", bpro1_filepath("/vision/scratch/firat/weizmann_xshock/gt-objects.txt")) ||
            !parameters()->add("File containing list of objects with groundtruth xgraphs:", "-list_gt_xgraph", bpro1_filepath("/vision/scratch/firat/weizmann_xshock/gt-objects.txt")) ||
            !parameters()->add("Groundtruth xgraph VOX label", "-xgraph_vox", vcl_string("weizmann_gt_xgraph")) ||
            !parameters()->add("Prototype xgraph object", "-prototype_xgraph_file", vcl_string("horse001")) ||
            !parameters()->add("Root vertex id:", "-root_vid", unsigned(1)) ||
            !parameters()->add("Positive output data file:", "-pos_output_file", bpro1_filepath("")) ||
            !parameters()->add("Negative output data file:", "-neg_output_file", bpro1_filepath(""))
            //
    )
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
}


//: Destructor
vox_train_xshock_ccm_model_process::
~vox_train_xshock_ccm_model_process()
{
}


//: Clone the process
bpro1_process* vox_train_xshock_ccm_model_process::
clone() const
{
    return new vox_train_xshock_ccm_model_process(*this);
}

//: Returns the name of this process
vcl_string vox_train_xshock_ccm_model_process::
name()
{ 
    return "Train xshock ccm model (vox)";
}

//: Provide a vector of required input types
vcl_vector< vcl_string > vox_train_xshock_ccm_model_process::
get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.clear();
    return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > vox_train_xshock_ccm_model_process::
get_output_type()
{
    vcl_vector<vcl_string > to_return;
    to_return.clear();
    return to_return;
}

//: Return the number of input frames for this process
int vox_train_xshock_ccm_model_process::
input_frames()
{
    return 1;
}


//: Return the number of output frames for this process
int vox_train_xshock_ccm_model_process::
output_frames()
{
    return 1;
}



//------------------------------------------------------------------------------
//: Execute this process
bool vox_train_xshock_ccm_model_process::
execute()
{
    // USER PARAMS --------------------------------------------------------------

    vox_train_ccm_model_params params = vox_train_ccm_model_default_params();

    // image folder
    bpro1_filepath temp_path;

    this->parameters()->get_value("-index_file", temp_path);
    params.index_file_name = temp_path.path;

    this->parameters()->get_value("-image_ext", params.image_ext);

    // edgemap extension
    this->parameters()->get_value("-edgemap_ext", params.edgemap_ext);
    this->parameters()->get_value("-edgeorient_ext", params.edgeorient_ext);
    this->parameters()->get_value("-edge_vox", params.edgemap_orient_vox_label);

    // Category-specific info
    this->parameters()->get_value("-list_pos_images", temp_path);
    params.list_pos_objs = temp_path.path;

    this->parameters()->get_value("-list_neg_images", temp_path);
    params.list_neg_objs = temp_path.path;

    this->parameters()->get_value("-list_gt_xgraph", temp_path);
    params.list_gt_xgraph_objs = temp_path.path;

    this->parameters()->get_value("-xgraph_vox", params.gt_xgraph_vox_label);

    this->parameters()->get_value("-prototype_xgraph_file", params.prototype_xgraph_file_obj);

    this->parameters()->get_value("-root_vid", params.root_vid);

    // Output files
    this->parameters()->get_value("-pos_output_file", temp_path);
    params.pos_output_file = temp_path.path;

    this->parameters()->get_value("-neg_output_file", temp_path);
    params.neg_output_file = temp_path.path;


    // STORAGE CLASSES ----------------------------------------------------------


    // PROCESS DATA -------------------------------------------------------------


    vox_train_ccm_model trainer;
    trainer.set_training_data_info(params);
    trainer.collect_positive_data();
    trainer.collect_negative_data();

    // OUTPUT DATA ---------------------------------------------------------------

    return true;
}


// ----------------------------------------------------------------------------
bool vox_train_xshock_ccm_model_process::
finish()
{
    return true;
}








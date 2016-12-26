//:
// \file
// \brief The parameter class for dborl_categorization inherits from dborl_algo_params
//        handles all the io related to input.xml, status.xml, perf.xml, params.xml thanks to inheritance
//        all other parameters needed for dborl_categorization is defined here and added to param_list_ of base class in the constructor
//        
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 01/14/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_categorization_params_h)
#define _dborl_categorization_params_h

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks to the parameter list
class dborl_categorization_params : public dborl_algo_params
{
public:

  dborl_parameter<bool> use_shock_matching; // if false uses curve matching

  //: from dbskr_tree_edit_params // unfortunately needs to be repeated here to define each of them as a dborl_parameter: OZGE todo: automize this somehow when such a parameter class is available
  dborl_parameter<float> scurve_sample_ds;  // Sampling ds to reconstruct the scurve, def: 1.0
  dborl_parameter<float> scurve_interpolate_ds;  // Interpolation ds to get densely interpolated versions of the scurves, def: 1.0 --> meaningful if localized_edit option is ON
  dborl_parameter<bool> localized_edit; // improve elastic matching cost of scurves using the densely interpolated versions, def: false
  dborl_parameter<bool> elastic_splice_cost; // use the elastic splice cost computation, def: false
  //: the defualt for this R parameter is 6.0f in original Sebastian implementation of shock matching
  dborl_parameter<double> shock_matching_R; // curve matching R that weighs bending over stretching, default 6.0f
  //: the following option is only false when shock fragments are being matched, 
  //  to match shock graphs of simpled closed contours, i.e. shock trees, this option should always be TRUE
  dborl_parameter<bool> circular_ends; // use circular completions at leaf branches during shock matching, def: true 
  dborl_parameter<bool> combined_edit; // use combined edit cost during shock matching, def: false
  dborl_parameter<bool> coarse_edit; // run coarse edit distance algorithm, def: false

  dborl_parameter<bool> use_normalized_cost;
  dborl_parameter<bool> arclength_normalization; // if false uses total splice cost for normalization
  dborl_parameter<bool> save_matching;

  dborl_parameter<bool> check_for_same_object; // if there is a possibility that the input object is in the input index file, make this check not to report the object itself as the nearest neighbor

  //: from dbcvr_clsd_curvematch parameters:
  dborl_parameter<int> template_size;
  dborl_parameter<float> curve_matching_R;   // this R's default is different so repeated for ease of use
  dborl_parameter<float> rest_curvematch_ratio;
  dborl_parameter<float> rms;
  dborl_parameter<bool> normalized_stretch_cost;

  dborl_parameter<vcl_string> input_object_path;
  dborl_parameter<vcl_string> input_object_name;
  dborl_parameter<vcl_string> index_filename;
  dborl_parameter<vcl_string> output_directory;
  dborl_parameter<vcl_string> input_extension;       // extensions of images to load, assumption: <obj name><.ext> is found in the object directory
  
  dborl_categorization_params(vcl_string algo_name) : dborl_algo_params(algo_name) { 
    use_shock_matching.set_values(param_list_, "data", "use_shock_matching", "if false use curve matching to find similarity of the input object to the objects in the index", true, true);
    
    scurve_sample_ds.set_values(param_list_, "tree_edit_params", "scurve_sample_ds", "Sampling ds to reconstruct the scurve", 1.0f, 1.0f); 
    scurve_interpolate_ds.set_values(param_list_, "tree_edit_params", "scurve_interpolate_ds", "Interpolation ds to get densely interpolated versions of the scurves, meaningful if localized_edit option is ON", 1.0f, 1.0f); 
    localized_edit.set_values(param_list_, "tree_edit_params", "localized_edit", "improve elastic matching cost of scurves using the densely interpolated versions", false, false); 
    elastic_splice_cost.set_values(param_list_, "tree_edit_params", "elastic_splice_cost", "use the elastic splice cost computation", false, false); 
    shock_matching_R.set_values(param_list_, "tree_edit_params", "shock_matching_R", "curve matching R that weighs bending over stretching during shock matching", 6.0f, 6.0f); 
    circular_ends.set_values(param_list_, "tree_edit_params", "circular_ends", "use circular completions at leaf branches during shock matching", true, true); 
    combined_edit.set_values(param_list_, "tree_edit_params", "combined_edit", "use combined edit cost during shock matching", false, false); 
    coarse_edit.set_values(param_list_, "tree_edit_params", "coarse_edit", "run coarse edit distance algorithm", false, false); 

    template_size.set_values(param_list_, "curve_match_params", "template_size", "template size in DP table", 3, 3);
    curve_matching_R.set_values(param_list_, "curve_match_params", "curve_matching_R", "curve matching R that weighs bending over stretching", 10.0f, 10.0f);
    rest_curvematch_ratio.set_values(param_list_, "curve_match_params", "rest_curvematch_ratio", "restricted closed matching ratio, if 0.25f checkes 1/4 of total length as start point", 0.25f, 0.25f);
    rms.set_values(param_list_, "curve_match_params", "rms", "rms for line fitting, pass 0 if no line fitting", 0.05f, 0.05f);
    normalized_stretch_cost.set_values(param_list_, "curve_match_params", "normalized_stretch_cost", "normalize stretch cost during curve matching?", false, false);

    check_for_same_object.set_values(param_list_, "data", "check_for_same_object", "if there is a possibility that the input object is in the input index file, make this check not to report the object itself as the nearest neighbor", true, true);
    use_normalized_cost.set_values(param_list_, "data", "use_normalized_cost", "normalized the final cost", true, true);
    arclength_normalization.set_values(param_list_, "data", "arclength_normalization", "if true use total arclength of constructed scurves for boundary, otherwise use total splice cost", true, true);
    save_matching.set_values(param_list_, "data", "save_matching", "save shgms/curve match files?", true, true);

    input_extension.set_values(param_list_, "data", "input_extension", "string to append to object names to load esfs/cons", "", ".esf");  // can be anything to append to the object name e.g. _mask.jpg
    input_object_path.set_values(param_list_, "io_data", "input_object_path", "the path to the input object, assumes esf is saved here", "", "", 0, dborl_parameter_system_info::INPUT_OBJECT_DIR);
    input_object_name.set_values(param_list_, "io_data", "input_object_name", "the string identifier of the input object", "", "", 0, dborl_parameter_system_info::INPUT_OBJECT_STRING_ID);
    index_filename.set_values(param_list_, "io_data", "index_filename", "path of index file", "", "", 1, dborl_parameter_system_info::NOT_DEFINED, "flat_image", dborl_parameter_type_info::FILEASSOC); 
    output_directory.set_values(param_list_, "io_data", "output_directory", "output directory", "", "", -1, dborl_parameter_system_info::OUTPUT_FILE, "");
  }
};

#endif // _dborl_categorization_params_h)

//:
// \file
// \brief A base class that defines the parameters along with all of their attributes for the algorithms
//        all the parameter files are constructed by this base class automatically
//        as long as the derived classes add their additional parameters to the parameter vector in this class properly in their constructors
//        The design is similar to vul_arg_list
//
//        The base class has 6 parameters which are required for any algorithm
//        so they come automatically as long as the algorithm has a parameter class deriving from this base
//
//        This class is intended to support parameter pass via xml formatted parameter files 
//        only "primitive" parameter types are supported like integer, short, unsigned, float, double, vcl_string, bool (whose type string is "flag"), etc.
//        other types like vcl_vector<int> are not supported by the dborl_parameter class since they cannot be listed in the parameter files.
//        if one needs to pass a vector of a primitive type to an algorithm then one needs to write those parameter values to another file
//        and pass the name of that file as a parameter and read from that file in the algorithm
//        [see the example algorithm in dborl/algo/examples which needs a list of image names which is passed via an intermediate file].
//        If the algorithm is to be run on ORL website though, only special files can be passed via the system and those files' names are parameters with a system_info attribute
//        All the other parameters should be of primitive type and not lists
//
// \author Ozge C Ozcanli (Brown)
// \date   December 20, 2007
//
// \verbatim
//  Modifications
// \endverbatim
//

//  Usage: 
//  any algorithm should have a parameter class deriving from this base class
//  then a command line call to the algorithm:
//  algo [-options]
//  can be processed by passing the [-options] to the algorithm's parameter class for parsing
//  if the call is like:
//  algo -x input.xml
//  then the parameter class parses the input.xml parameter file and fills in all the parameter values automatically
//  if the call is like:
//  algo -help
//  then the corresponding flag parameters are turned on to signal the algorithm to exit by printing the usage message 
//  etc.

//
//5 types of files are important with the following name conventions:
// 1) input.xml : parameters are inputted to the algorithms via these files whose path is passed as a command line argument application/algorithm, and the algorithm parses this file to extract parameter values 
// 2) params.xml : ORL web application uses this file to generate the input form for the users to set parameter values while running algorithms through the website. Once the user submits the form for the parameters, ORL generates input.xml automatically and passes it as a command line argument to the algorithm
// 3) status.xml : algorithms are supposed to create and update this file in their output folders for ORL to track their execution status, e.g. how many images have been processed so far, etc.
//                 the parameters should be created with the group name: status and added to the param_list
//                 if that is the case they're automatically added to the status.xml file and excempt from input.xml and params.xml
// 4) perf.xml : algorithms are supposed to create these files on exit for ORL to create performance plots.
//               the parameters should be created with the group name: performance and added to the param_list
//               if that is the case they're automatically added to the perf.xml file and excempt from input.xml and params.xml
// 5) evaluation.xml : algorithms are supposed to create these files on exit for ORL to create plots of type ROC, RPC, etc. or recall bar charts
//                     there is a method to create this file in a predetermined mode given a map of category names and category statistics in the form of 
//                     TPs, FPs, TNs and FNs.
//
//   the group names: status, performance, web_directive are special


#if !defined(_dborl_algo_params_h)
#define _dborl_algo_params_h

#include <dborl/dborl_algo_params_base.h>

#include <vbl/vbl_ref_count.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <vgl/vgl_point_2d.h>
#include <dborl/dborl_evaluation.h>

//: base class that the algorithm params should be inherited from
//  for inheritence from this class, it is enough to add the extra parameters needed for the algorithm 
//  and write a constructor which initializes the attributes of these parameters 
//  and adds them to the parameter vector in this base class using the set_values method of the dborl_parameter class
//  (see the constructor of this base class which does this for the 6 parameters in the base class,
//   or the example: dborl_example_algo in dborl/algo/examples/ )
//  no need to overwrite any of the methods in this class
class dborl_algo_params : public dborl_algo_params_base
{
public:

  //: parameters in the web_directive group, used in creation of params.xml file
  dborl_parameter<vcl_string> print_params_file; // name of the params file, passed in the input.xml in the web_directive group
  dborl_parameter<bool> print_params_only; // ("web_directive", "print_params_only", "print the xml parameter file only", false, false);
  dborl_parameter<vcl_string> status_file; // ("web_directive", "status_block", "Status file path", "", "", dborl_parameter::STATUS_BLOCK, "");
  dborl_parameter<vcl_string> perf_file; // ("web_directive", "perf_block", "Performance file path", "", "", dborl_parameter::PERFORMANCE_OUTPUT, "");
  dborl_parameter<vcl_string> evaluation_file; //  in "web_directive" group
  dborl_parameter<vcl_string> categorization_file; //  in "web_directive" group

  //: parameters in the basic group of the status block, used in creation of status.xml file
  dborl_parameter<float> percent_completed;  
  dborl_parameter<int> exit_code;
  dborl_parameter<vcl_string> exit_message;

  //: parameters for command line processing, not included in the parameter list
  dborl_parameter<bool> exit_with_no_processing;

  //: constructor
  dborl_algo_params(vcl_string algo_name);

  virtual ~dborl_algo_params() { perf_param_map_.clear(); }

  //: makes no check as to whether the same parameter is being added or not
  void add_params(dborl_algo_params_base& other);

  //: set the parameters with the same group names and names in the other's list with the values of the parameters in this list
  void set_params(dborl_algo_params_base& other);
    
  bool print_params_xml(vcl_string filename);
  bool print_status_xml();
  
  bool parse_from_data(bxml_data_sptr root);
  bxml_element *create_default_document_data();
  bxml_element *create_document_data();

  //: parse the input parameter file to fill in parameter values
  //  input_param_filename_ should be set prior to calling this method
  //  parse_command_line_args() sets input_param_filename_ if called prior to this method
  bool parse_input_xml();
  //: print a parameter file with the current values of the parameters
  void print_input_xml(vcl_string param_file);
  //: print a parameter file with the default values of the parameters
  void print_default_input_xml(vcl_string param_file);

  //: the command line args can be passed directly to the params class 
  //  the following method extracts the name of the parameter file if passed properly with -x option
  //  parse_input_xml() method should be called to actually parse the parameter file 
  //  also supports printing an input parameter file with the default values, 
  bool parse_command_line_args(int argc, char* argv[]);
  //: for algos using dborl_cluster for parallel processing, when command line parameters are broadcasted to all processes
  bool parse_command_line_args(vcl_vector<vcl_string>& argv);  

  vcl_map<vcl_string, vgl_point_2d<double> >& get_perf_param_map() { return perf_param_map_; }
  
  //: return false if an entry with key "name" does not exist
  bool perf_map_update(vcl_string name, double x, double y);
  //: return false if an entry with key "name" already exists
  bool perf_map_insert(vcl_string name, double x, double y);
  void perf_plot_set_type(short type) { plot_type_ = type; }
  short perf_plot_get_type() { return plot_type_; }

  //: print perf.xml with the current values in the perf_param_map
  void print_perf_xml(vcl_string description);

  //: print evaluation.xml with the statistics passed
  void print_evaluation_xml(vcl_map<vcl_string, dborl_exp_stat_sptr>& category_statistics, bool print_FN);

  //: the name of the algo is used as root node in input.xml 
  vcl_string input_param_filename_;
protected:
  
 
  //: this is a special map that is used to keep track of performance parameters that will be outputted to perf.xml file
  //  the string part is the legend attribute and vgl_point is the x, y attributes of a point tag in perf.xml
  vcl_map<vcl_string, vgl_point_2d<double> > perf_param_map_;
  short plot_type_;  

  void print_params_open_root(vcl_ostream& of);
  void print_params_close_root(vcl_ostream& of);

  void print_status_open(vcl_ostream& of);
  void print_status_basic(vcl_ostream& of);
  void print_status_close(vcl_ostream& of);
  void print_status_optionals(vcl_ostream& of);
  
};

#endif  //_dborl_algo_params_h

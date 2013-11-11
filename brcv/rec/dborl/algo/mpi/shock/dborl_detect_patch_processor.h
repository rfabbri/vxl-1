//:
// \file
// \brief The object detection processor inherits from dborl_dataset_processor base
//
//        This algorithm uses shock patch fragment sets of query image and the models to detect an instance
//        from the class of the model in the query image
//
//        This particular program takes a set of test images with ground truths and
//        a set of model images from the same category and evaluates the detections
//        The test images collection should be in orl-style and will be loaded into a dborl_dataset
//        so that all the groundtruths are available (each object should contain a groundtruth file named as <object name>.xml
//        in its folder under the main dataset folder)
//
//        parses an input file
//        input: a model patch 
//               a test dataset given by a flat index
//               assumes patch storage files are saved in train dir and database dir
//
//        output: # of correct and wrong detections for each threshold is inserted into an output file
//
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_detect_patch_processor_h)
#define _dborl_detect_patch_processor_h

#include <dborl/algo/mpi/dborl_dataset_processor.h>
#include <dborl/dborl_image_description_sptr.h>
#include <bxml/bxml_document.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/algo/io/dbskr_detect_patch_params.h>

#include <vcl_utility.h> // --> for vcl_pair class
#include <vul/vul_timer.h>

class dborl_detect_patch_processor_input
{ public:
    dborl_detect_patch_processor_input(dbskr_shock_patch_storage_sptr model_s, 
                                      dbskr_shock_patch_storage_sptr query_s, 
                                      vcl_string model_n, vcl_string query_n, float t, dborl_image_description_sptr query_d) :
                                      model_st(model_s), query_st(query_s), model_name(model_n), query_name(query_n), 
                                      threshold(t), query_desc(query_d) {}
    dbskr_shock_patch_storage_sptr model_st;  // model storage
    dbskr_shock_patch_storage_sptr query_st;  // query storage
    vcl_string model_name;  // model's name
    vcl_string query_name;  // query's name
    float threshold;
    dborl_image_description_sptr query_desc;
};

//: turn the detection boxes into fixed sized arrays for message passing
//  the size of the array is the number of thresholds for this experiment, size_t_ variable of the class
class dborl_detect_patch_processor_output
{
public:
  dborl_detect_patch_processor_output() : TP_(0), FP_(0), TN_(0), FN_(0) {}
  
  inline void set_values(int tp, int fp, int tn, int fn) { TP_ = tp, FP_ = fp, TN_ = tn, FN_ = fn; }
  
  int TP_;
  int FP_;
  int TN_;
  int FN_;
};


//: result class is a float point number which is the matching cost
//  to normalize this matching cost, finalize method will be used
class dborl_detect_patch_processor : public dborl_dataset_processor<dborl_detect_patch_processor_input, dborl_detect_patch_processor_output>
{
public:
  dborl_detect_patch_processor() {}

  //: this method is run on each processor after lead processor broadcasts its command
  //  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
  virtual bool parse_command_line(vcl_vector<vcl_string>& argv);

  //: this method is run on each processor
  //  parse the input file into a bxml document and extract each parameter
  virtual bool parse(const char* param_file);

  //: parse the index file
  virtual bool parse_index(vcl_string index_file);

  //: this method prints an xml input file setting all the parameters to defaults
  //  run the algorithm to generate this file, then modify it
  void print_default_file(const char* default_file);

  //: this method is run on each processor
  virtual bool initialize(vcl_vector<dborl_detect_patch_processor_input>& t);

  //: this method is run in a distributed mode on each processor on the cluster
  virtual bool process(dborl_detect_patch_processor_input i, dborl_detect_patch_processor_output& f);

  //: this method is run on the lead processor once after results are collected from each processor
  virtual bool finalize(vcl_vector<dborl_detect_patch_processor_output>& results);

  void print_time();

#ifdef MPI_CPP_BINDING
  virtual MPI::Datatype create_datatype_for_R();
#else
  virtual MPI_Datatype create_datatype_for_R();
#endif

protected:
  protected:
  bxml_document param_doc_;  // parse the input file into a document
  dbskr_detect_patch_params params_;
  unsigned size_t_;  // number of thresholds for this experiment
  unsigned query_set_size_;
  vcl_string model_category_;
  int positive_cnt_, negative_cnt_;

  vul_timer t_;
  
};

#endif  //_dborl_detect_patch_processor_h

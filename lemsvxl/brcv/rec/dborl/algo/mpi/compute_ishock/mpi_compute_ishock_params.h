// This is rec/dborl/algo/mpi/compute_ishock/mpi_compute_ishock_params.h
#ifndef mpi_compute_ishock_params_h
#define mpi_compute_ishock_params_h


//:
// \file
// \brief The parameter class for computing intrinsic shocks
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date May 24, 2008
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#include <dborl/algo/dborl_algo_params.h>

//: 
class mpi_compute_ishock_params : public dborl_algo_params
{ 
public:  
  //: Constructor
  mpi_compute_ishock_params(std::string algo_name);
  virtual ~mpi_compute_ishock_params();

  bool parse_from_data(bxml_data_sptr root);
  static bxml_element *create_default_document_data();
  bxml_element *create_document_data();

  std::string file_list() 
  { return this->get_param("io", "file_list")->value_str(); }

  std::string file_dir()
  { return this->get_param("io", "file_dir")->value_str(); }

  std::string out_dir()
  { return this->get_param("io", "out_dir")->value_str(); }

  std::string extension()
  { return this->get_param("io", "extension")->value_str(); }

  // Parameters
  dborl_parameter_base* get_param(const std::string& group, const std::string& name);



  


protected:
  dborl_parameter<std::string> file_list_;
  dborl_parameter<std::string> file_dir_;
  dborl_parameter<std::string> out_dir_;
  dborl_parameter<std::string> extension_;


  // handle of the parameters to clear at the end
  std::vector<dborl_parameter_base*> process_params_;
};

#endif  //_mpi_compute_ishock_params_h

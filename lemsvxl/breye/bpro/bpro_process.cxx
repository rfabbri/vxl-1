// This is brl/bpro/bpro_process.cxx

//:
// \file

#include <bpro/bpro_process.h>

#include <vcl_iostream.h>
#include <vcl_cassert.h>

#include <bpro/bpro_parameters.h>
#include <bpro/bpro_storage.h>


bpro_process::bpro_process()
{
  parameters_ = new bpro_parameters;
}


bpro_process::~bpro_process()
{
}


//: Copy Constructor
bpro_process::bpro_process(const bpro_process& other)
 : input_data_(other.input_data_), output_data_(other.output_data_),
   parameters_(new bpro_parameters(other.parameters_)), // deep copy of parameters
   input_names_(other.input_names_), output_names_(other.output_names_)
{
  ///this->ref_count = 0;
}


void bpro_process::clear_input(int size) 
{
  input_data_.clear();
  if(size < 0)
    input_data_.resize(this->input_frames());
  else
    input_data_.resize(size);
}


void bpro_process::clear_output(int size)
{
  output_data_.clear();
  if(size < 0)
    output_data_.resize(this->output_frames());
  else
    output_data_.resize(size);
}


void bpro_process::set_input( const vcl_vector< bpro_storage_sptr >& i , unsigned frame)
{
  if (input_data_.size()<frame+1)
    input_data_.resize(frame+1);
  input_data_[frame] = i;
}


void bpro_process::add_input(const bpro_storage_sptr& i, unsigned frame)
{
  if (input_data_.size()<frame+1)
    input_data_.resize(frame+1);
  input_data_[frame].push_back(i);
}


vcl_vector< bpro_storage_sptr > const & bpro_process::get_output(unsigned frame)
{
  assert(output_data_.size()>frame);
  return output_data_[frame];
}


vcl_vector< bpro_storage_sptr > const & bpro_process::get_input(unsigned frame)
{
  assert(input_data_.size()>frame);
  return input_data_[frame];
}


bpro_parameters_sptr bpro_process::parameters()
{
  return parameters_;
}


void bpro_process::set_parameters(const bpro_parameters_sptr& params)
{
  parameters_ = params;
}

//: Set the vector of boolean flags to denote global storages
bool bpro_process::set_global(unsigned outer_index,
                               unsigned inner_index)
{  
  if ((int)outer_index <  output_frames() && 
           outer_index >= 0               &&
           inner_index >= 0)   {
    // If empty fill it with false (default)
    if (is_output_global_.empty())     {
      is_output_global_.assign(output_frames(),vcl_vector<bool>(output_frames(),false));
    }
    if (inner_index >=  is_output_global_[outer_index].size())
      is_output_global_[outer_index].resize(inner_index+1,false);

    is_output_global_[outer_index][inner_index] = true; // set flag to denote global (true)
    return true;
  } else {
    vcl_cerr << "\nbpro2_process error: set_global(...) incorrect index range!\n";
    return false;
  }
}

//: Get the vector of boolean flags to denote global storages
vcl_vector<vcl_vector<bool> > bpro_process::get_global()
{
  return is_output_global_;
}

//: Returns a vector of strings with suggested names for output classes
vcl_vector< vcl_string > bpro_process::suggest_output_names()
{
  return vcl_vector< vcl_string >();
}

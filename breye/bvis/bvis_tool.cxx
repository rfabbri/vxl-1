// This is breye/bvis/bvis_tool.cxx
//:
// \file

#include <vgui/vgui_dialog.h>
#include <vcl_cassert.h>
#include "bvis_tool.h"

//: Constructor
bvis_tool::bvis_tool()
{
}


//: Destructor
bvis_tool::~bvis_tool()
{
}


//: Allow the tool to add to the popup menu as a tableau would
void
bvis_tool::get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
}


//: This is called when the tool is activated
void 
bvis_tool::activate()
{
}


//: This is called when the tool is activated
void 
bvis_tool::deactivate()
{
}

void bvis_tool::clear_input(int size) 
{
  input_data_.clear();
  if(size < 0)
    input_data_.resize(1);
  else
    input_data_.resize(n_input_datasets());
}


void bvis_tool::clear_output(int size)
{
  output_data_.clear();
  if(size < 0)
    output_data_.resize(1);
  else
    output_data_.resize(n_output_datasets());
}


void bvis_tool::set_input( const vcl_vector< bpro_storage_sptr >& input,
                            const unsigned index)
{
  input_data_[index] = input;
}


void bvis_tool::add_input(const bpro_storage_sptr& input,
                            const unsigned index)
{
  if (input_data_.size()<=0)
    input_data_.resize(n_input_datasets());
  input_data_[index].push_back(input);
}

//: Set the vector of outputs
void bvis_tool::set_output( const vcl_vector< bpro_storage_sptr >& output,
                            const unsigned index)
{
  if (output_data_.size()<=0)
    output_data_.resize(n_output_datasets());
  output_data_[index] = output;

}
//: Append the storage class to the output vector
void bvis_tool::add_output( const bpro_storage_sptr& output,
                            const unsigned index)
{
  if (output_data_.size()<=0)
    output_data_.resize(n_output_datasets());
  output_data_[index].push_back(output);
}

vcl_vector< bpro_storage_sptr > const & 
bvis_tool::get_output(const unsigned index)
{
  assert(output_data_.size()>index);
  return output_data_[index];
}


vcl_vector< bpro_storage_sptr > const &
bvis_tool::get_input(const unsigned index)
{
  assert(input_data_.size()>index);
  return input_data_[index];
}

//: Use in menus to toggle a parameter
void bvis_tool_toggle(const void* boolref)
{
  bool* bref = (bool*) boolref;
  *bref = !(*bref);
}

//: Use in menus to toggle a parameter
void bvis_tool_inc(const void* intref)
{
  int* iref = (int*) intref;
  *iref = (*iref)++;
}

//: Use in menus to toggle a parameter
void bvis_tool_dec(const void* intref)
{
  int* iref = (int*) intref;
  if ((*iref)>1)
    *iref = (*iref)--;
}

//: Use in menus to set the value of a double parameter
void bvis_tool_set_param(const void* doubleref)
{
  double* dref = (double*)doubleref;
  double param_val = *dref;
  vgui_dialog param_dlg("Set Tool Param");
  param_dlg.field("Parameter Value", param_val);
  if(!param_dlg.ask())
    return;

  *dref = param_val;
}

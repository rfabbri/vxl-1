// This is bpro1/xio/bpro1_xio_parameters.cxx


#include "bpro1_xio_parameters.h"

#include <bpro1/bpro1_parameters.h>

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <vcl_fstream.h>


// ============================================================================
// IMPLEMENTATION OF X_WRITE and X_READ
// ============================================================================

// ----------------------------------------------------------------------------
//: write parameters of a bpro1_process to an xml file
bool x_write(const vcl_string& filepath, 
             const bpro1_parameters_sptr& parameters,
             const vcl_string& name)
{
  vcl_ofstream file(filepath.c_str());
  if (x_write(file, parameters, name))
  {
    file.close();
    return true;
  }
  else
  {
    file.close();
    return false;
  }
}


// ----------------------------------------------------------------------------
//: write parameters of a bpro1_process to a stream
bool x_write(vcl_ostream& os, 
             const bpro1_parameters_sptr& parameters,
             const vcl_string& name)
{
  bxml_document doc;
  bxml_element *root = new bxml_element(name);
  doc.set_root_element(root);

  vcl_vector<bpro1_param* > param_list = parameters->get_param_list();
  for (unsigned i =0; i < param_list.size(); ++i)
  {
    bpro1_param* param = param_list[i];
    root->set_attribute(param->name(), param->value_str());
  }

  bxml_write(os, doc);
  
  return true;
}








// -----------------------------------------------------------------------------
//: append parameters as attributes to an element
bool x_write(bxml_element* elm, const bpro1_parameters_sptr& parameters)
{
  // only write to existing element
  if (!elm)
    return false;

  vcl_vector<bpro1_param* > param_list = parameters->get_param_list();
  for (unsigned i =0; i < param_list.size(); ++i)
  {
    bpro1_param* param = param_list[i];
    elm->set_attribute(param->name(), param->value_str());
  }
  return true;
}




// ============================================================================
// READ
// ============================================================================


//: load parameters of a bpro1_process from an xml file
// Only existing parameters will be loaded. Others are ignored.
bool x_read(const vcl_string& filepath, bpro1_parameters_sptr& parameters,
            const vcl_string& name)
{
  bxml_document doc = bxml_read(filepath);
  
  bxml_element* data = static_cast<bxml_element*>(doc.root_element().as_pointer());

  vcl_vector<bpro1_param* > param_list = parameters->get_param_list();
  for (unsigned i =0; i < param_list.size(); ++i)
  {
    bpro1_param* param = param_list[i];
    vcl_string param_value;
    if (data->get_attribute(param->name(), param_value))
    {
      if (!param->parse_value_str(param_value))
      {
        vcl_cout << "ERROR: could not parse parameter " << param->name() << "\n";
        return false;
      }
    }
    else
    {
      vcl_cout << "ERROR: couldn't set value of param " << param->name() << "\n";
      return false;
    }
  }

  return true;
}




// -----------------------------------------------------------------------------
//: Load parameters from an XML element
// Only existing parameters will be loaded. Others are ignored.
bool x_read(const bxml_element* elm, bpro1_parameters_sptr& parameters)
{
  vcl_vector<bpro1_param* > param_list = parameters->get_param_list();
  for (unsigned i =0; i < param_list.size(); ++i)
  {
    bpro1_param* param = param_list[i];
    vcl_string param_value;
    if (elm->get_attribute(param->name(), param_value))
    {
      if (!param->parse_value_str(param_value))
      {
        vcl_cout << "ERROR: could not parse parameter " << param->name() << "\n";
        return false;
      }
    }
    else
    {
      vcl_cout << "ERROR: couldn't set value of param " << param->name() << "\n";
      return false;
    }
  }

  return true;
}








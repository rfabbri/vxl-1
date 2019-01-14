// This is dbsks/xio/dbsks_xio_xshock.cxx


#include "dbsks_xio_xshock_det.h"

#include <bpro1/bpro1_parameters.h>

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <fstream>




//==============================================================================
// Functions Declaration
//==============================================================================


dbsks_xshock_det_record_sptr dbsks_xshock_det_record_new_v1();
dbsks_xshock_det_record_sptr dbsks_xshock_det_record_new_v2();
dbsks_xshock_det_record_sptr dbsks_xshock_det_record_new_v3();


//: Write a list of xshock detection to a stream
bool x_write_v3(std::ostream& os, const std::vector<dbsks_xshock_det_record_sptr >& xshock_det_list);

//: Write a list of xshock detection to a stream
bool x_write_v2(std::ostream& os, const std::vector<dbsks_xshock_det_record_sptr >& xshock_det_list);

//: Write a list of xshock detection to a stream
bool x_write_v1(std::ostream& os, const std::vector<dbsks_xshock_det_record_sptr >& xshock_det_list);



//==============================================================================
// List of tags
//==============================================================================


const std::string det_list_tag = "xgraph_det_list";
const std::string xshock_det_tag = "xshock_det";


//==============================================================================
// Function Implementation
//==============================================================================


//: latest version of xshock_det_record
dbsks_xshock_det_record_sptr dbsks_xshock_det_record_new(const std::string& version)
{
  if (version == "1")
    return dbsks_xshock_det_record_new_v1();
  else if (version == "2")
    return dbsks_xshock_det_record_new_v2();
  else if (version == "3")
    return dbsks_xshock_det_record_new_v3();
  else
    return 0;
}


// -------------------------------------------------------------------------
//: A template for a xshock detection record with all necessary fields - version 1
dbsks_xshock_det_record_sptr dbsks_xshock_det_record_new_v1()
{
  dbsks_xshock_det_record_sptr params = new bpro1_parameters();
  params->add("object_name", "object_name", std::string(""));
  params->add("model_category", "model_category", std::string(""));
  params->add("screenshot", "screenshot", std::string(""));
  params->add("xgraph_xml", "xgraph_xml", std::string(""));
  params->add("confidence", "confidence", double(0));
  params->add("xgraph_scale", "xgraph_scale", double(1));
  params->add("bbox_xmin", "bbox_xmin", double(0));
  params->add("bbox_ymin", "bbox_ymin", double(0));
  params->add("bbox_xmax", "bbox_xmax", double(0));
  params->add("bbox_ymax", "bbox_ymax", double(0));
  return params;
}


// -------------------------------------------------------------------------
//: xshock_det_record - version 2
dbsks_xshock_det_record_sptr dbsks_xshock_det_record_new_v2()
{
  dbsks_xshock_det_record_sptr params = dbsks_xshock_det_record_new_v1();
  params->add("unmatched_weight", "unmatched_weight", std::string("[]"));
  params->add("wcm_confidence", "wcm_confidence", std::string("[]"));
  return params;
}


// -------------------------------------------------------------------------
//: xshock_det_record - version 3
dbsks_xshock_det_record_sptr dbsks_xshock_det_record_new_v3()
{
  dbsks_xshock_det_record_sptr params = dbsks_xshock_det_record_new_v2();
  params->add("bnd_screenshot", "bnd_screenshot", std::string(""));
  return params;
}






// -----------------------------------------------------------------------------
//: Write a list of xshock detection to a stream
bool x_write(std::string filepath, const std::vector<dbsks_xshock_det_record_sptr >& xshock_det_list)
{
  std::ofstream file(filepath.c_str(), std::ios::out);
  if (x_write(file, xshock_det_list))
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



// -----------------------------------------------------------------------------
//: Write a list of xshock detection to a stream
bool x_write(std::ostream& os, const std::vector<dbsks_xshock_det_record_sptr >& xshock_det_list)
{
  return x_write_v3(os, xshock_det_list);
}


//: Write a list of xshock detection to a stream
bool x_write_v3(std::ostream& os, const std::vector<dbsks_xshock_det_record_sptr >& xshock_det_list)
{
  bxml_document doc;
  bxml_element *root = new bxml_element(det_list_tag);
  doc.set_root_element(root);

  // Version
  root->set_attribute("version", "3");

  // Extract a list of parameter name in a detection record
  dbsks_xshock_det_record_sptr det_record_template = dbsks_xshock_det_record_new_v3();
  std::vector<bpro1_param* > template_params = det_record_template->get_param_list();

  std::vector<std::string > param_names;
  for (unsigned i =0; i < template_params.size(); ++i)
  {
    param_names.push_back(template_params[i]->name());
  }


  // write a list of detection record out
  for (unsigned m =0; m < xshock_det_list.size(); ++m)
  {
    dbsks_xshock_det_record_sptr xshock_det_record = xshock_det_list[m];
    
    // create a new xml item for this detection
    bxml_element* xshock_det_elm = new bxml_element(xshock_det_tag);
    root->append_data(xshock_det_elm);

    // append each parameter as an element
    for (unsigned i =0; i < param_names.size(); ++i)
    {
      std::string param_name = param_names[i];
      std::string param_value = "";

      // locate the pointer to this parameter in the record
      std::map<std::string, bpro1_param* >::iterator iter = 
        xshock_det_record->get_param_map().find(param_name);
      if (iter != xshock_det_record->get_param_map().end())
      {
        param_value = iter->second->value_str();
      }

      // create xml element for this parameter
      bxml_element* param_elm = new bxml_element(param_name);
      xshock_det_elm->append_data(param_elm);
      param_elm->append_text(param_value);
    }
  }

  bxml_write(os, doc);
  return true;
}



// -----------------------------------------------------------------------------
//: Write a list of xshock detection to a stream
bool x_write_v2(std::ostream& os, const std::vector<dbsks_xshock_det_record_sptr >& xshock_det_list)
{
  bxml_document doc;
  bxml_element *root = new bxml_element(det_list_tag);
  doc.set_root_element(root);

  // Version
  root->set_attribute("version", "2");

  // Extract a list of parameter name in a detection record
  dbsks_xshock_det_record_sptr det_record_template = dbsks_xshock_det_record_new();
  std::vector<bpro1_param* > template_params = det_record_template->get_param_list();

  std::vector<std::string > param_names;
  for (unsigned i =0; i < template_params.size(); ++i)
  {
    param_names.push_back(template_params[i]->name());
  }


  // write a list of detection record out
  for (unsigned m =0; m < xshock_det_list.size(); ++m)
  {
    dbsks_xshock_det_record_sptr xshock_det_record = xshock_det_list[m];
    
    // create a new xml item for this detection
    bxml_element* xshock_det_elm = new bxml_element(xshock_det_tag);
    root->append_data(xshock_det_elm);

    // append each parameter as an element
    for (unsigned i =0; i < param_names.size(); ++i)
    {
      std::string param_name = param_names[i];

      // locate the pointer to this parameter in the record
      std::map<std::string, bpro1_param* >::iterator iter = 
        xshock_det_record->get_param_map().find(param_name);
      if (iter == xshock_det_record->get_param_map().end())
        continue;

      // get paramter as a string
      std::string param_value = iter->second->value_str();

      // create xml element for this parameter
      bxml_element* param_elm = new bxml_element(param_name);
      xshock_det_elm->append_data(param_elm);
      param_elm->append_text(param_value);
    }
  }

  bxml_write(os, doc);
  return true;
}

// -----------------------------------------------------------------------------
//: Write a list of xshock detection to a stream - version 1
bool x_write_v1(std::ostream& os, const std::vector<dbsks_xshock_det_record_sptr >& xshock_det_list)
{
  bxml_document doc;
  bxml_element *root = new bxml_element(det_list_tag);
  doc.set_root_element(root);

  // Version
  root->set_attribute("version", "1");

  // Extract a list of parameter name in a detection record
  dbsks_xshock_det_record_sptr det_record_template = dbsks_xshock_det_record_new_v1();
  std::vector<bpro1_param* > template_params = det_record_template->get_param_list();

  std::vector<std::string > param_names;
  for (unsigned i =0; i < template_params.size(); ++i)
  {
    param_names.push_back(template_params[i]->name());
  }


  // write a list of detection record out
  for (unsigned m =0; m < xshock_det_list.size(); ++m)
  {
    dbsks_xshock_det_record_sptr xshock_det_record = xshock_det_list[m];
    
    // create a new xml item for this detection
    bxml_element* xshock_det_elm = new bxml_element(xshock_det_tag);
    root->append_data(xshock_det_elm);

    // append each parameter as an element
    for (unsigned i =0; i < param_names.size(); ++i)
    {
      std::string param_name = param_names[i];

      // locate the pointer to this parameter in the record
      std::map<std::string, bpro1_param* >::iterator iter = 
        xshock_det_record->get_param_map().find(param_name);
      if (iter == xshock_det_record->get_param_map().end())
        continue;

      // get paramter as a string
      std::string param_value = iter->second->value_str();

      // create xml element for this parameter
      bxml_element* param_elm = new bxml_element(param_name);
      xshock_det_elm->append_data(param_elm);
      param_elm->append_text(param_value);
    }
  }

  bxml_write(os, doc);
  return true;
}



//: load a list of xshock detection
bool x_read(const std::string& filepath, std::vector<dbsks_xshock_det_record_sptr >& xshock_det_list)
{
  xshock_det_list.clear();

  // load the whole XML file into memory
  bxml_document doc = bxml_read(filepath);
  if (!doc.root_element())
  {
    std::cout << "\nERROR: couldn't read XML file: " << filepath << std::endl;
    return false;
  }

  // locate the root node
  bxml_data_sptr result = bxml_find_by_name(doc.root_element(), 
    bxml_element(det_list_tag));

  if (!result)
  {
    std::cout << "ERROR: could not find node 'xgraph_det_list'.\n";
    return false;
  }

  bxml_element* root = static_cast<bxml_element* >(result.ptr());
  std::string version = "";
  if (!root->get_attribute("version", version))
  {
    version = "1"; // initial version, no version in header
  };



  // the root node should contain a list of detections. Iterate to retrieve them
  for (bxml_element::const_data_iterator it = root->data_begin(); 
    it != root->data_end(); ++it)
  {
    bxml_data_sptr data = *it;
    if (data->type() != bxml_data::ELEMENT)
    {
      continue;
    }

    bxml_element* elm = static_cast<bxml_element* >(data.ptr());
    if (elm->name() == xshock_det_tag)
    {
      bxml_element* xshock_det_elm = elm;

      // create a xshock detection record
      dbsks_xshock_det_record_sptr xml_record = dbsks_xshock_det_record_new(version);

      // retrieve values for each parameter of the record
      std::map<std::string, bpro1_param* >& param_map = xml_record->get_param_map();
      for (std::map<std::string, bpro1_param* >::iterator iter = param_map.begin();
        iter != param_map.end(); ++iter)
      {
        std::string param_name = iter->first;
        


        bxml_data_sptr temp = bxml_find_by_name(xshock_det_elm, bxml_element(param_name));
        if (!temp || temp->type() != bxml_data::ELEMENT)
        {
          std::cout << "\nWarning: xml record in file " << filepath 
            << " does not contain value for parameter: " 
            << param_name << std::endl;
          continue;
        }
        bxml_element* param_elm = static_cast<bxml_element* >(temp.ptr());

        // there should be only one data items in this element and that is the
        // value of the parameter (in text format)
        std::string param_value = "";
        if (param_elm->num_data() > 1)
        {
          std::cout << "\nERROR: #data items in parameter-value field is not 1.\n";
          continue;
        }
        if (param_elm->num_data() == 1)
        { 
          bxml_data_sptr data = *param_elm->data_begin();
          if (data->type() != bxml_data::TEXT)
            continue;
          bxml_text* text_data = static_cast<bxml_text*>(data.ptr());

          // assign value for the parameter
          param_value = text_data->data();
        }

        // special handling for string parameters
        if (xml_record->valid_parameter_type(param_name, std::string("")))
        {
          // special handling for "screenshot" because the automatic parse cut the string short when 
          // contains space and it does not preserve ">" and "<"
          if (param_name == "screenshot")
          {
            // replace "<" with "&lt;" and ">" with "&gt;"
            std::string::size_type found;
            while ( (found = param_value.find("<")) != std::string::npos)
            {
              param_value = param_value.replace(found, 1, "&lt;");
            }

            while ( (found = param_value.find(">")) != std::string::npos)
            {
              param_value = param_value.replace(found, 1, "&gt;");
            }
          }

          // remove " at the begining
          if (param_value.length() > 1 &&
            param_value.at(0) == '\"' && 
            param_value.at(param_value.length()-1) == '\"')
          {
            param_value = param_value.substr(1, param_value.length()-2);
          }

          xml_record->set_value(param_name, param_value);
        }
        else
        {
          if (!iter->second->parse_value_str(param_value))
          {
            std::cout << "\nERROR: parameter \"" << param_name 
              << "\" has wrong type in xml file.\n";
            continue;
          }
        }
      } // for each parameter
      xshock_det_list.push_back(xml_record);
    } 
  }// for each detection record
  return true;
}


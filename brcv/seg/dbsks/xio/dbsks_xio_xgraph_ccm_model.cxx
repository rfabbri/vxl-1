// This is dbsksp/xio/dbsks_xio_xgraph_ccm_model.cxx


#include "dbsks_xio_xgraph_ccm_model.h"

#include <dbsks/dbsks_xgraph_ccm_model.h>

#include <dbxml/dbxml_xio.h>
#include <dbxml/dbxml_algos.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>

#include <vcl_utility.h>



//==============================================================================
// Declaring Supporting Functions
//==============================================================================

//: Read version 1 from an XML root node
bool x_read_v1(const bxml_element* root, dbsks_xgraph_ccm_model_sptr& xgraph_ccm);

//: Read version 2 from an XML root node
bool x_read_v2(bxml_element* root, dbsks_xgraph_ccm_model_sptr& xgraph_ccm);

//: Read version 2 from an XML root node
bool x_read_v2_1(bxml_element* root, bxml_element* param_root, dbsks_xgraph_ccm_model_sptr& xgraph_ccm);

//: Parse a list of CCM parameters from an XML element
bool x_read(bxml_element* ccm_params_element, dbsks_ccm_params& params);

//: Parse a bfrag_ccm_model
bool x_read(bxml_element* elm, dbsks_bfrag_cost_model& bfrag_model,
            unsigned& edge_id, int& side_idx);

//: Parse a bsta_histogram
bool x_read(bxml_element* elm, bsta_histogram<double >& histogram);

//==============================================================================
// List of tags
//==============================================================================


const vcl_string tag_xgraph_ccm_model = "dbsks_xgraph_ccm_model";
const vcl_string tag_ccm_params = "ccm_params";
const vcl_string tag_xgraph_info = "xgraph_info";
const vcl_string tag_list_bfrag_cost_models = "list_bfrag_cost_models";
const vcl_string tag_bfrag_cost_model = "bfrag_cost_model";
const vcl_string tag_user_override = "user_override";

//==============================================================================
// WRITE
//==============================================================================


///// ----------------------------------------------------------------------------
////:  Write xgraph contour ocm model to a stream
//bool x_write(vcl_ostream& os, const dbsks_xgraph_ccm_model_sptr& xgraph_ocm);



//==============================================================================
// READ
//==============================================================================

//: Read xgraph contour ocm model from a file
bool x_read(const vcl_string& filepath, dbsks_xgraph_ccm_model_sptr& xgraph_ccm)
{
  // House-cleaning
  xgraph_ccm = 0;

  // load the whole XML file into memory
  bxml_document doc = bxml_read(filepath);

  if (!doc.root_element())
  {
    vcl_cout << "\nERROR: couldn't read XML file: " << filepath << vcl_endl;
    return false;
  }

  // locate the root node
  bxml_element* root = dbxml_algos::find_by_name(doc.root_element(), tag_xgraph_ccm_model);

  if (!root)
  {
    vcl_cout << "ERROR: could not find node '" << tag_xgraph_ccm_model << "'.\n";
    return false;
  }

  unsigned version = 0;
  root->get_attribute("version", version);
  if (version == 1)
  {
    return x_read_v1(root, xgraph_ccm);
  }
  else if (version == 2)
  {
    return x_read_v2(root, xgraph_ccm);
  }
  else
  {
    vcl_cout << "ERROR: Unknown dbsks_xgraph_ccm_model XML version. Quit now.\n";
    return false;
  }
}






//: Read xgraph contour ocm model from a file
bool x_read(const vcl_string& filepath, const vcl_string& param_filepath, dbsks_xgraph_ccm_model_sptr& xgraph_ccm)
{
  // House-cleaning
  xgraph_ccm = 0;

  // load the whole XML file into memory
  bxml_document doc = bxml_read(filepath);

  if (!doc.root_element())
  {
    vcl_cout << "\nERROR: couldn't read XML file: " << filepath << vcl_endl;
    return false;
  }
  
  // locate the root node
  bxml_element* root = dbxml_algos::find_by_name(doc.root_element(), tag_xgraph_ccm_model);

  if (!root)
  {
    vcl_cout << "ERROR: could not find node '" << tag_xgraph_ccm_model << "'.\n";
    return false;
  }

  unsigned version = 0;
  root->get_attribute("version", version);
  if (version == 1)
  {
    return x_read_v1(root, xgraph_ccm);  
  }
  else if (version == 2)
  {
    //return x_read_v2(root, xgraph_ccm);
    bxml_document param_doc = bxml_read(param_filepath);
    bxml_element* param_root = dbxml_algos::find_by_name(param_doc.root_element(), tag_ccm_params);

    if (!param_root)
    {
      vcl_cout << "ERROR: could not find node '" << tag_ccm_params << "'.\n";
      return false;
    }
    return x_read_v2_1(root, param_root, xgraph_ccm);
  }
  else
  {
    vcl_cout << "ERROR: Unknown dbsks_xgraph_ccm_model XML version. Quit now.\n";
    return false;  
  }
}









//------------------------------------------------------------------------------
//: Read version 1 from an XML root node
bool x_read_v1(const bxml_element* root, dbsks_xgraph_ccm_model_sptr& xgraph_ccm)
{
  // Prepare space to store the xgraph geometry model
  xgraph_ccm = new dbsks_xgraph_ccm_model();
  vcl_map<unsigned, dbsks_xfrag_ccm_model_sptr > map_edge2ocm;


  // Read graph tree info: root node
  unsigned root_vid = 0;
  root->get_attribute("root_vid", root_vid);
  xgraph_ccm->set_root_vid(root_vid);

  // Read parameters necessary to compute CCM cost
  float tol_near_zero = 4.0f;
  root->get_attribute("tol_near_zero", tol_near_zero);

  float edge_threshold = 15.0f;
  root->get_attribute("edge_threshold", edge_threshold);

  float distance_threshold = 8.0f; 
  root->get_attribute("distance_threshold", distance_threshold);

  float ccm_gamma = 0.3f;
  root->get_attribute("ccm_gamma", ccm_gamma);

  float ccm_lambda = 0.4f;
  root->get_attribute("ccm_lambda", ccm_lambda);

  int nbins_0topi = 18;
  root->get_attribute("nbins_0topi", nbins_0topi);

  xgraph_ccm->set_ccm_params(edge_threshold, tol_near_zero, distance_threshold, ccm_gamma, ccm_lambda, nbins_0topi);

  
  // Read statistical distribution of the fragments
  for (bxml_element::const_data_iterator it = root->data_begin();
    it != root->data_end(); ++it)
  {
    bxml_data_sptr data = *it;
    if (data->type() != bxml_data::ELEMENT)
    {
      continue;
    }

    bxml_element* elm = static_cast<bxml_element* >(data.ptr());
    if (elm->name() == "dbsks_xfrag_ccm_model")
    {
      bxml_element* frag_elm = elm;

      // retrieve edge id of the fragment
      unsigned int edge_id = 0;
      frag_elm->get_attribute("edge_id", edge_id);

      // create a place holder for the xfrag's data
      dbsks_xfrag_ccm_model_sptr xfrag_ccm = new dbsks_xfrag_ccm_model();
      xgraph_ccm->map_edge2ccm().insert(vcl_make_pair(edge_id, xfrag_ccm));

      for (bxml_element::const_data_iterator it2 = frag_elm->data_begin(); it2 != frag_elm->data_end(); ++it2)
      {
        bxml_data_sptr data2 = *it2;
        if (data2->type() != bxml_data::ELEMENT)
        {
          continue;
        }

        bxml_element* elm2 = static_cast<bxml_element* >(data2.ptr());
        if (elm2->name() == "fg_dist")
        {
          bxml_element* contour_elm = elm2;
          unsigned int side_id = 0;
          contour_elm->get_attribute("side_id", side_id);

          if (side_id != 0 && side_id != 1)
          {
            vcl_cout << "\nERROR: Unknown side id, skipping this fg_dist.\n";
            continue;
          }

          for (bxml_element::const_data_iterator it3 = contour_elm->data_begin(); it3 != contour_elm->data_end(); ++it3)
          {
            bxml_data_sptr data3 = *it3;
            if (data3->type() != bxml_data::ELEMENT)
            {
              continue;
            }

            bxml_element* elm3 = static_cast<bxml_element* >(data3.ptr());
            if (elm3->name() == "weibull")
            {
              bxml_element* weibull = elm3;

              double weibull_lambda, weibull_k, weibull_avg_logl;
              weibull->get_attribute("lambda", weibull_lambda);
              weibull->get_attribute("k", weibull_k);
              weibull->get_attribute("avg_logl", weibull_avg_logl);

              // construct a distribution
              bsta_weibull<double > weibull_dist(weibull_lambda, weibull_k);
              xfrag_ccm->set_bnd_weibull_distribution(side_id, weibull_dist);
              //xfrag_ccm->set_avg_logl_fit_weibull(side_id, weibull_avg_logl);
              xfrag_ccm->set_active_bnd_distribution(side_id, WEIBULL);

            } // if
          } // for it3
        }
      }// for it2
    }
  }// for it

  return true; 
}




//------------------------------------------------------------------------------
//: Read version 2 from an XML root node
bool x_read_v2(bxml_element* root, dbsks_xgraph_ccm_model_sptr& xgraph_ccm)
{
  // Prepare space to store the xgraph geometry model
  xgraph_ccm = new dbsks_xgraph_ccm_model();
  
  //> Parse ccm params
  bxml_element* elm_ccm_params = dbxml_algos::find_by_name(root, tag_ccm_params);
  if (!elm_ccm_params)
  {
    vcl_cout << "\nERROR: Missing '" << tag_ccm_params << "' element in XML file.\n";
    return false;
  }
  else
  {
    dbsks_ccm_params params;
    x_read(elm_ccm_params, params);
    xgraph_ccm->set_ccm_params(params);
  }

  //> Parse xgraph info
  bxml_element* elm_xgraph_info = dbxml_algos::find_by_name(root, tag_xgraph_info);
  int num_bfrags = 0;
  if (!elm_xgraph_info)
  {
    vcl_cout << "\nERROR: Missing '" << tag_xgraph_info << "' element in XML file.\n";
    return false;
  }
  else
  {
    bxml_element* temp = 0;
    
    unsigned root_vid = xgraph_ccm->root_vid();
    temp = dbxml_algos::find_by_name(elm_xgraph_info, "root_vid");
    xml_parse(temp, root_vid);
    xgraph_ccm->set_root_vid(root_vid);

    double base_xgraph_size = xgraph_ccm->base_xgraph_size();
    temp = dbxml_algos::find_by_name(elm_xgraph_info, "base_xgraph_size");
    xml_parse(temp, base_xgraph_size);
    xgraph_ccm->set_base_xgraph_size(base_xgraph_size);

    temp = dbxml_algos::find_by_name(elm_xgraph_info, "num_bfrags");
    xml_parse(temp, num_bfrags);
  }

  //> Parse list of bfrag ccm models
  bxml_element* elm_list_bfrags = dbxml_algos::find_by_name(root, tag_list_bfrag_cost_models);
  if (!elm_list_bfrags)
  {
    vcl_cout << "\nERROR: Missing '" << tag_list_bfrag_cost_models << "' element in XML file.\n";
    return false;
  }
  else
  {
    // Iterate thru the list of bfrag models and parse one by one
    for (bxml_element::const_data_iterator iter = elm_list_bfrags->data_begin();
      iter != elm_list_bfrags->data_end(); ++iter)
    {
      // only pay attention to element of type "bfrag_ccm_model"
      bxml_element* elm = dbxml_algos::cast_to_element(*iter, tag_bfrag_cost_model);
      if (!elm)
        continue;

      dbsks_bfrag_cost_model bfrag_model;
      unsigned edge_id;
      int side_id;
      x_read(elm, bfrag_model, edge_id, side_id);

      // save to the xgraph model
      dbsks_xfrag_ccm_model_sptr xfrag_model = xgraph_ccm->xfrag_model(edge_id);
      // create one if there isn't one yet
      if (!xfrag_model)
      {
        xfrag_model = new dbsks_xfrag_ccm_model();
        xgraph_ccm->set_xfrag_model(edge_id, xfrag_model);
      }

      // save it!
      xfrag_model->set_bfrag_model(side_id, bfrag_model);
    }
  }

  //> Parse list of bfrag ccm models
  bxml_element* elm_user_override = dbxml_algos::find_by_name(root, tag_user_override);
  if (!elm_user_override)
  {
    vcl_cout << "\nERROR: Missing '" << tag_user_override << "' element in XML file.\n";
    return false;
  }
  else
  {
    // various things to check. None may exist
    bxml_element* elm_list_bfrags_to_ignore = 
      dbxml_algos::find_by_name(root, "list_bfrags_to_ignore");
    if (elm_list_bfrags_to_ignore)
    {
      vcl_string list_bfrags_str;
      if (xml_parse(elm_list_bfrags_to_ignore, list_bfrags_str))
      {
        xgraph_ccm->override_cfrag_with_constant_distribution(list_bfrags_str, ',');
      }
    }
  }


  return true;
}




//------------------------------------------------------------------------------
//: Read version 2 from an XML root node
bool x_read_v2_1(bxml_element* root, bxml_element* param_root, dbsks_xgraph_ccm_model_sptr& xgraph_ccm)
{
  // Prepare space to store the xgraph geometry model
  xgraph_ccm = new dbsks_xgraph_ccm_model();

  //> Parse ccm params
  //bxml_element* elm_ccm_params = param_root;
  /*if (!elm_ccm_params)
  {
    vcl_cout << "\nERROR: Missing '" << tag_ccm_params << "' element in XML file.\n";
    return false;
  }
  else*/
  {
    dbsks_ccm_params params;
    x_read(param_root, params);
    xgraph_ccm->set_ccm_params(params);
  }

  //> Parse xgraph info
  bxml_element* elm_xgraph_info = dbxml_algos::find_by_name(root, tag_xgraph_info);
  int num_bfrags = 0;
  if (!elm_xgraph_info)
  {
    vcl_cout << "\nERROR: Missing '" << tag_xgraph_info << "' element in XML file.\n";
    return false;
  }
  else
  {
    bxml_element* temp = 0;

    unsigned root_vid = xgraph_ccm->root_vid();
    temp = dbxml_algos::find_by_name(elm_xgraph_info, "root_vid");
    xml_parse(temp, root_vid);
    xgraph_ccm->set_root_vid(root_vid);

    double base_xgraph_size = xgraph_ccm->base_xgraph_size();
    temp = dbxml_algos::find_by_name(elm_xgraph_info, "base_xgraph_size");
    xml_parse(temp, base_xgraph_size);
    xgraph_ccm->set_base_xgraph_size(base_xgraph_size);

    temp = dbxml_algos::find_by_name(elm_xgraph_info, "num_bfrags");
    xml_parse(temp, num_bfrags);
  }

  //> Parse list of bfrag ccm models
  bxml_element* elm_list_bfrags = dbxml_algos::find_by_name(root, tag_list_bfrag_cost_models);
  if (!elm_list_bfrags)
  {
    vcl_cout << "\nERROR: Missing '" << tag_list_bfrag_cost_models << "' element in XML file.\n";
    return false;
  }
  else
  {
    // Iterate thru the list of bfrag models and parse one by one
    for (bxml_element::const_data_iterator iter = elm_list_bfrags->data_begin();
      iter != elm_list_bfrags->data_end(); ++iter)
    {
      // only pay attention to element of type "bfrag_ccm_model"
      bxml_element* elm = dbxml_algos::cast_to_element(*iter, tag_bfrag_cost_model);
      if (!elm)
        continue;

      dbsks_bfrag_cost_model bfrag_model;
      unsigned edge_id;
      int side_id;
      x_read(elm, bfrag_model, edge_id, side_id);

      // save to the xgraph model
      dbsks_xfrag_ccm_model_sptr xfrag_model = xgraph_ccm->xfrag_model(edge_id);
      // create one if there isn't one yet
      if (!xfrag_model)
      {
        xfrag_model = new dbsks_xfrag_ccm_model();
        xgraph_ccm->set_xfrag_model(edge_id, xfrag_model);
      }

      // save it!
      xfrag_model->set_bfrag_model(side_id, bfrag_model);
    }
  }

  //> Parse list of bfrag ccm models
  bxml_element* elm_user_override = dbxml_algos::find_by_name(root, tag_user_override);
  if (!elm_user_override)
  {
    vcl_cout << "\nERROR: Missing '" << tag_user_override << "' element in XML file.\n";
    return false;
  }
  else
  {
    // various things to check. None may exist
    bxml_element* elm_list_bfrags_to_ignore =
      dbxml_algos::find_by_name(root, "list_bfrags_to_ignore");
    if (elm_list_bfrags_to_ignore)
    {
      vcl_string list_bfrags_str;
      if (xml_parse(elm_list_bfrags_to_ignore, list_bfrags_str))
      {
        xgraph_ccm->override_cfrag_with_constant_distribution(list_bfrags_str, ',');
      }
    }
  }


  return true;
}







//------------------------------------------------------------------------------
//: Read a list of CCM parameters from an XML element
bool x_read(bxml_element* element, dbsks_ccm_params& ccm_params)
{
  if (!element)
    return false;

  if (element->name() != tag_ccm_params)
    return false;

  vcl_cout << vcl_endl << "CCM Model Parameters:" << vcl_endl;
  vcl_cout << "-------------------------" << vcl_endl;

  bxml_element* temp = 0;
  temp = dbxml_algos::find_by_name(element, "distance_threshold");
  xml_parse(temp, ccm_params.distance_threshold_);
  vcl_cout << "CCM Distance Threshold: " << ccm_params.distance_threshold_ << vcl_endl;

  temp = dbxml_algos::find_by_name(element, "distance_tol_near_zero");
  xml_parse(temp, ccm_params.distance_tol_near_zero_);
  vcl_cout << "CCM Distance Tolerance Near Zero: " << ccm_params.distance_tol_near_zero_ << vcl_endl;

  temp = dbxml_algos::find_by_name(element, "orient_threshold");
  xml_parse(temp, ccm_params.orient_threshold_);
  vcl_cout << "CCM Orientation Threshold: " << ccm_params.orient_threshold_ << vcl_endl;

  temp = dbxml_algos::find_by_name(element, "orient_tol_near_zero");
  xml_parse(temp, ccm_params.orient_tol_near_zero_);
  vcl_cout << "CCM Orientation Tolerance Near Zero: " << ccm_params.orient_tol_near_zero_ << vcl_endl;

  temp = dbxml_algos::find_by_name(element, "weight_chamfer"); 
  xml_parse(temp, ccm_params.weight_chamfer_);
  vcl_cout << "CCM Weight Chamfer: " << ccm_params.weight_chamfer_ << vcl_endl;

  temp = dbxml_algos::find_by_name(element, "weight_edge_orient"); 
  xml_parse(temp, ccm_params.weight_edge_orient_);
  vcl_cout << "CCM Weight Edge Orientation: " << ccm_params.weight_edge_orient_ << vcl_endl;

  temp = dbxml_algos::find_by_name(element, "weight_contour_orient"); 
  xml_parse(temp, ccm_params.weight_contour_orient_);
  vcl_cout << "CCM Weight Contour Orientation: " << ccm_params.weight_contour_orient_ << vcl_endl;

  temp = dbxml_algos::find_by_name(element, "nbins_0topi");
  xml_parse(temp, ccm_params.nbins_0topi_);
  vcl_cout << "CCM Number of Bins 0 - pi: " << ccm_params.nbins_0topi_ << vcl_endl;

  temp = dbxml_algos::find_by_name(element, "local_window_width");
  xml_parse(temp, ccm_params.local_window_width_);
  vcl_cout << "CCM Local Window Width: " << ccm_params.local_window_width_ << vcl_endl;

  return true;
}


//------------------------------------------------------------------------------
//: Parse a bfrag_ccm_model
bool x_read(bxml_element* elm, dbsks_bfrag_cost_model& bfrag_model,
            unsigned& edge_id, int& side_idx)
{
  if (!elm)
    return false;

  if (elm->name() != tag_bfrag_cost_model)
    return false;

  // get edge_id
  elm->get_attribute("edge_id", edge_id);

  // get side_id
  vcl_string side_id_str;
  elm->get_attribute("side_id", side_id_str);
  side_idx = (side_id_str == "L") ? 0 : 1;

  // parse foreground distribution
  bxml_element* elm_dist = 0;
  {
    bxml_element* elm_fg = dbxml_algos::find_by_name(elm, "foreground");
    if (!elm_fg)
      return false;
    elm_dist = elm_fg;
  }

  // parse the distribution
  {
    vcl_string active_dist_type = "";
    bxml_element* temp = 0;
    temp = dbxml_algos::find_by_name(elm_dist, "active_dist_type");
    xml_parse(temp, active_dist_type);
    if (active_dist_type == "NONPARAM")
    {
      bfrag_model.active_dist_type_[FOREGROUND] = NONPARAM;

      // parse histogram
      bxml_element* elm_hist = dbxml_algos::find_by_name(elm_dist, "bsta_histogram");
      if (!elm_hist)
      {
        vcl_cout << "\nERROR: could not find element of type 'bsta_histogram'.\n";
        return false;
      }

      bsta_histogram<double > histogram;
      x_read(elm_hist, histogram);
      bfrag_model.nonparam_dist_[FOREGROUND] = histogram; 
    }
    else
    {
      vcl_cout << "\nERROR: Unknown distribution type [ " << active_dist_type << " ]\n";
      return false;
    }
  }

  // parse background distribution
  {
    bxml_element* elm_bg = dbxml_algos::find_by_name(elm, "background");
    if (!elm_bg)
      return false;
    elm_dist = elm_bg;
  }

  // parse the active distribution
  {
    vcl_string active_dist_type = "";
    bxml_element* temp = 0;
    temp = dbxml_algos::find_by_name(elm_dist, "active_dist_type");
    xml_parse(temp, active_dist_type);
    if (active_dist_type == "NONPARAM")
    {
      bfrag_model.active_dist_type_[BACKGROUND] = NONPARAM;

      // parse histogram
      bxml_element* elm_hist = dbxml_algos::find_by_name(elm_dist, "bsta_histogram");
      if (!elm_hist)
      {
        vcl_cout << "\nERROR: could not find element of type 'bsta_histogram'.\n";
        return false;
      }

      bsta_histogram<double > histogram;
      x_read(elm_hist, histogram);
      bfrag_model.nonparam_dist_[BACKGROUND] = histogram;     
    }
    else
    {
      vcl_cout << "\nERROR: Unknown distribution type [ " << active_dist_type << " ]\n";
      return false;
    }
  }
  return true;
}


//------------------------------------------------------------------------------
//: Parse a bsta_histogram
bool x_read(bxml_element* elm, bsta_histogram<double >& histogram)
{
  if (!elm)
    return false;

  if (elm->name() != "bsta_histogram")
    return false;

  int nbins;
  double min, max, min_prob;
  
  bxml_element* temp = 0;
  temp = dbxml_algos::find_by_name(elm, "nbins");
  xml_parse(temp, nbins);

  temp = dbxml_algos::find_by_name(elm, "min");
  xml_parse(temp, min);

  temp = dbxml_algos::find_by_name(elm, "max");
  xml_parse(temp, max);

  temp = dbxml_algos::find_by_name(elm, "min_prob");
  xml_parse(temp, min_prob);

  // vector of values
  // example vector: [0.2 0.3 10]
  temp = dbxml_algos::find_by_name(elm, "values");
  vcl_string values_str;
  xml_parse(temp, values_str);
  vcl_string::size_type start = values_str.find_first_of('[', 0);
  vcl_string::size_type end = values_str.find_first_of(']', start);
  if (start == values_str.npos || end == values_str.npos)
  {
    return false;
  }
  
  values_str = values_str.substr(start+1, end-start-1);
  vcl_istringstream values_istream(values_str);
  vnl_vector<double > values(nbins, 0);
  if (!values.read_ascii(values_istream))
    return false;

  temp = dbxml_algos::find_by_name(elm, "counts");
  vcl_string counts_str;
  xml_parse(temp, counts_str);
  start = counts_str.find_first_of('[', 0);
  end = counts_str.find_first_of(']', start);
  if (end == counts_str.npos)
    return false;
  counts_str = counts_str.substr(start+1, end-start-1);
  vcl_istringstream counts_istream(counts_str);
  vnl_vector<double > counts(nbins, 0);
  if (!counts.read_ascii(counts_istream))
    return false;

  bsta_histogram<double > h(min, max, nbins, min_prob);
  for (int i =0; i < nbins; ++i)
  {
    h.upcount(values[i], counts[i]);
  }
  histogram = h;

  return true;
}



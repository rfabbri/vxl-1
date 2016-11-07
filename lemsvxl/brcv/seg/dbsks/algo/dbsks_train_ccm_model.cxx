// This is file seg/dbsks/dbsks_train_ccm_model.cxx

//:
// \file

#include "dbsks_train_ccm_model.h"

#include <dbsks/algo/dbsks_load.h>
#include <dbsks/dbsks_subpix_ccm.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsks/dbsks_xgraph_ccm_model.h>
#include <dbul/dbul_parse_simple_file.h>
#include <bpro1/bpro1_parameters.h>


#include <dbxml/dbxml_vector_xio.h>
#include <dbxml/dbxml_xio.h>
//#include <dbul/dbul_parse_simple_file.h>


#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>



#include <vsol/vsol_box_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_intersection.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>
#include <vul/vul_timer.h>
#include <vil/vil_load.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>


//==============================================================================
// dbsks_train_ccm_model_params
//==============================================================================

//------------------------------------------------------------------------------
//: Set ccm parameters from a list of key-value pairs
// both key and value are in string format
void dbsks_train_ccm_model_params::
parse_ccm_params(const vcl_map<vcl_string, vcl_string >& ccm_params)
{
  // a template of current parameters
  bpro1_parameters_sptr p = new bpro1_parameters();
  p->add("", "nbins_0topi", this->nbins_0topi);
  p->add("", "distance_threshold", this->distance_threshold);
  p->add("", "distance_tol_near_zero", this->distance_tol_near_zero);
  p->add("", "orient_threshold", this->orient_threshold);
  p->add("", "orient_tol_near_zero", this->orient_tol_near_zero);
  p->add("", "weight_chamfer", this->weight_chamfer);
  p->add("", "weight_edge_orient", this->weight_edge_orient);
  p->add("", "weight_contour_orient", this->weight_contour_orient);
  p->add("", "local_window_width", this->local_window_width);

  // adjust these parameters using input data
  for (vcl_map<vcl_string, vcl_string >::const_iterator iter = ccm_params.begin();
    iter != ccm_params.end(); ++iter)
  {
    bpro1_parameters_set_value_from_str(*p, iter->first, iter->second);
  }

  // save back the parameter values
  p->get_value("nbins_0topi", this->nbins_0topi);
  p->get_value("distance_threshold", this->distance_threshold);
  p->get_value("distance_tol_near_zero", this->distance_tol_near_zero);
  p->get_value("orient_threshold", this->orient_threshold);
  p->get_value("orient_tol_near_zero", this->orient_tol_near_zero);
  p->get_value("weight_chamfer", this->weight_chamfer);
  p->get_value("weight_edge_orient", this->weight_edge_orient);
  p->get_value("weight_contour_orient", this->weight_contour_orient);
  p->get_value("local_window_width", this->local_window_width);
  
  return;
}


//------------------------------------------------------------------------------
//: Set data parameters from a list of key-value pairs
// both key and value are in string format
void dbsks_train_ccm_model_params::
parse_data_params(const vcl_map<vcl_string, vcl_string >& data_params)
{
  // a template of current parameters
  bpro1_parameters_sptr p = new bpro1_parameters();
  p->add("", "base_xgraph_size", this->base_xgraph_size);
  p->add("", "num_samples_per_xgraph", this->num_samples_per_xgraph);
  p->add("", "perturb_delta_x", this->perturb_delta_x);
  p->add("", "perturb_delta_y", this->perturb_delta_y);

  // adjust these parameters using input data
  for (vcl_map<vcl_string, vcl_string >::const_iterator iter = data_params.begin();
    iter != data_params.end(); ++iter)
  {
    bpro1_parameters_set_value_from_str(*p, iter->first, iter->second);
  }

  // save back the parameter values
  p->get_value("base_xgraph_size", this->base_xgraph_size);
  p->get_value("num_samples_per_xgraph", this->num_samples_per_xgraph);
  p->get_value("perturb_delta_x", this->perturb_delta_x);
  p->get_value("perturb_delta_y", this->perturb_delta_y);
  return;
}



//=============================================================================
//  dbsks_train_ccm_model_params
//=============================================================================


//: Default training parameters for various categories
dbsks_train_ccm_model_params dbsks_train_ccm_model_default_params(const vcl_string& category)
{
  dbsks_train_ccm_model_params p;

  // CCM parameters
  p.nbins_0topi                = 18;
  p.distance_threshold         = 4;
  p.distance_tol_near_zero     = 1; //2;
  p.orient_threshold           = float(vnl_math::pi_over_4);
  p.orient_tol_near_zero       = float(vnl_math::pi / p.nbins_0topi) / 2;
  p.weight_chamfer             = 0.2f;
  p.weight_edge_orient         = 0.4f;
  p.weight_contour_orient      = 0.4f;
  p.local_window_width         = 3.0f;

  // number of data point for groundtruth xgraph
  p.num_samples_per_xgraph     = 100;
  p.perturb_delta_x            = 2;
  p.perturb_delta_y            = 2;
  p.base_xgraph_size          = 64;

  // image folder
  p.image_folder               = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals";

  // specifying positive and negative images
  p.edgemap_folder             = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_pb_edges_pyramid-clean_using_kovesi-I_15-len_4";
  p.edgemap_ext                = "_pb_edges.png";
  p.edgeorient_ext             = "_pb_orient.txt";

  // Category-specific info
  if (category == "applelogos")
  {
    p.list_pos_images          = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/list_applelogos_prototype1_pos_images.txt";
    p.list_neg_images          = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/list_applelogos_prototype1_neg_images.txt";
    p.xgraph_folder            = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/applelogos-xgraph";
    p.list_gt_xgraph           = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/list_applelogos_prototype1_xml.txt";
    p.prototype_xgraph_file    = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/applelogos-xgraph/applelogos_another.xgraph.0.prototype1.xml";
    p.root_vid                 = 4;
    p.pos_output_file          = "D:/vision/projects/symseg/xshock/applelogos_prototype1-pos_ccm_cost-2009jul15-long-range.xml";
    p.neg_output_file          = "D:/vision/projects/symseg/xshock/applelogos_prototype1-neg_ccm_cost-2009jul15-long-range.xml";
  }
  else if (category == "mugs")
  {
    p.list_pos_images          = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/list_mugs_pos_images.txt"; // done
    p.list_neg_images          = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/list_mugs_neg_images.txt"; // done
    p.xgraph_folder            = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/mugs-xgraph"; //done
    p.list_gt_xgraph           = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/list_mugs_xml.txt"; //done
    p.prototype_xgraph_file    = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/mugs-xgraph/mugs_apple.xgraph.0.xml"; //done
    p.root_vid                 = 1; // done
    p.pos_output_file          = "D:/vision/projects/symseg/xshock/mugs-pos_ccm_cost-2009jul23-chamfer_4.xml"; // done
    p.neg_output_file          = "D:/vision/projects/symseg/xshock/mugs-neg_ccm_cost-2009jul23-chamfer_4.xml"; // done
  }
  else if (category == "giraffes")
  {
    p.list_pos_images          = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/giraffes/list_giraffes_pos_images.txt"; // done
    p.list_neg_images          = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/giraffes/list_giraffes_neg_images.txt"; // done
    p.xgraph_folder            = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/giraffes-xgraph"; // done
    p.list_gt_xgraph           = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/giraffes/list_giraffes_xml.txt"; //done
    p.prototype_xgraph_file    = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/giraffes/giraffes_prototype.0.xml"; //done
    p.root_vid                 = 1; // done
    p.pos_output_file          = "D:/vision/projects/symseg/xshock/giraffes-pos_ccm_cost-2009aug31-chamfer_4.xml"; // done
    p.neg_output_file          = "D:/vision/projects/symseg/xshock/giraffes-neg_ccm_cost-2009aug31-chamfer_4.xml"; // done
  }
  else if (category == "bottles")
  {
    p.list_pos_images          = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/bottles/list_bottles_pos_images.txt"; // done
    p.list_neg_images          = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/bottles/list_bottles_neg_images.txt"; // done
    p.xgraph_folder            = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/bottles-xgraph"; // done
    p.list_gt_xgraph           = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/bottles/list_bottles_xml.txt"; //done
    p.prototype_xgraph_file    = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-train/bottles/bottles_prototype.0.xml"; //done
    p.root_vid                 = 4; // done
    p.pos_output_file          = "D:/vision/projects/symseg/xshock/bottles-pos_ccm_cost-2009sep14-chamfer_4.xml"; // done
    p.neg_output_file          = "D:/vision/projects/symseg/xshock/bottles-neg_ccm_cost-2009sep14-chamfer_4.xml"; // done
  };

  return p;
}

//=============================================================================
//  dbsks_train_ccm_model
//=============================================================================

//: Set info training info
bool dbsks_train_ccm_model::
set_training_data_info(const dbsks_train_ccm_model_params& p)
{
  this->params = p;

  //> Parse the list of positive images
  if (!dbul_parse_string_list(this->params.list_pos_images, pos_image_fnames))
  {
    vcl_cout << "\nERROR: couldn't load list of positive images:" << this->params.list_pos_images << vcl_endl;
    return false;
  }


  //> Parse the list of negative images
  if (!dbul_parse_string_list(this->params.list_neg_images, neg_image_fnames))
  {
    vcl_cout << "\nERROR: couldn't load list of negative images:" << this->params.list_neg_images << vcl_endl;
    return false;
  }

  return true;
}



//------------------------------------------------------------------------------
//: Form a list of boundary fragment labels from the prototype graph
bool dbsks_train_ccm_model::
form_prototype_bnd_frag_labels(vcl_vector<vcl_string >& prototype_bnd_frag_labels,
                               vcl_vector<unsigned >& list_prototype_eid)
{
  prototype_bnd_frag_labels.clear();
  list_prototype_eid.clear();

  //> Load the prototype shock graph
  dbsksp_xshock_graph_sptr prototype_xgraph = 0;
  if (!dbsks_load_xgraph(this->params.prototype_xgraph_file, prototype_xgraph))
  {
    vcl_cout << "\nERROR: Could not load prototype xgraph. Quit now.\n";
    return false;
  }
  
  //> Form a list of labels for the contour segments
  
  for (dbsksp_xshock_graph::edge_iterator eit = prototype_xgraph->edges_begin(); 
    eit != prototype_xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    unsigned eid = xe->id();

    prototype_bnd_frag_labels.push_back(vul_sprintf("%d-L", eid)); 
    prototype_bnd_frag_labels.push_back(vul_sprintf("%d-R", eid)); 
    list_prototype_eid.push_back(eid);
  }
  return true;
}


//: Collect positive training data
bool dbsks_train_ccm_model::
collect_positive_data()
{
  // start the timer
  vul_timer timer;
  timer.mark();

  //--------------------------------------------------------------------------//

  //> Get a list of boundary fragment labels;
  vcl_vector<vcl_string > prototype_bnd_frag_labels;
  vcl_vector<unsigned > list_prototype_eid;
  this->form_prototype_bnd_frag_labels(prototype_bnd_frag_labels, list_prototype_eid);

  //> Group the xgraph filenames by the names of the original images
  vcl_map<vcl_string, vcl_vector<vcl_string > > map_image_name_to_xgraph_fname;
  this->build_grouping_of_gt_xgraph_fnames(map_image_name_to_xgraph_fname);

  
  //> Construct a biarc sampler
  dbsks_biarc_sampler biarc_sampler;
  this->build_biarc_sampler(biarc_sampler);


  //// start a new XML document
  //bxml_document doc;

  //bxml_element *root = new bxml_element("xgraph_ccm_cost");
  //root->set_attribute("version", "1.0");
  //root->set_attribute("cost_type", "positive");
  //doc.set_root_element(root);
  


  //> Write header to output file
  vcl_ofstream os_pos(this->params.pos_output_file.c_str(), vcl_ios::out);

  os_pos << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
  os_pos << "<xgraph_ccm_cost version=\"1.0\" cost_type=\"positive\">\n";

  
  this->print_xml_header(os_pos);
  
  
  // 3. Generate ccm cost for postive samples...................................

  os_pos << "<data>\n";
  
  //> Iterate thru the positive images and compute ccm cost for each ground truth xgraph
  for (unsigned i_pos =0; i_pos < pos_image_fnames.size(); ++i_pos)
  {
    // name of image and the groundtruth xgraph in it
    vcl_string image_name = vul_file::strip_extension(pos_image_fnames[i_pos]);
    vcl_vector<vcl_string > xgraph_fnames = map_image_name_to_xgraph_fname[image_name];

    // load the image to get the original size
    vcl_string image_file = this->params.image_folder + "/" + pos_image_fnames[i_pos];
    vil_image_resource_sptr image_rsc = vil_load_image_resource(image_file.c_str(), true);
    if (!image_rsc)
    {
      vcl_cout << "\nERROR: could not load image file: " << image_file << vcl_endl;
      continue;
    }
    int image_width = image_rsc->ni();
    int image_height = image_rsc->nj();
    
    // keep track of the edgemap name by their width
    // \todo need a better way
    vcl_vector<vcl_string > list_edgemap_names;
    vcl_vector<vgl_box_2d<int > > list_edgemap_bboxes;
    this->gather_edgemap_pyramid_info(image_name, list_edgemap_names, list_edgemap_bboxes);

    // compute the scale of for each edgemap level
    vnl_vector<double > list_edgemap_scales(list_edgemap_names.size(), 1.0);
    for (unsigned kk =0; kk < list_edgemap_names.size(); ++kk)
    {
      // note that vgl_box_2d<int >::width() is 1 less the actual width
      list_edgemap_scales[kk] = double(list_edgemap_bboxes[kk].width()+1) / image_width;
    }

    // for each xshock graph, load the appropriate edgemap in the pyramid
    // and compute ccm cost
    for (unsigned i_xgraph =0; i_xgraph < xgraph_fnames.size(); ++i_xgraph)
    {
      // path to the xgraph file
      vcl_string xgraph_fname = xgraph_fnames[i_xgraph];
      vcl_string xgraph_file = this->params.xgraph_folder + "/" + xgraph_fname;

      // load the xgraph
      dbsksp_xshock_graph_sptr xgraph = 0;

      if (!dbsks_load_xgraph(xgraph_file, xgraph))
      {
        vcl_cout << "\nERROR: could not load xgraph file: " << xgraph_fname << vcl_endl;
        continue;
      }

      // Compute scale of the xgraph
      double cur_xgraph_scale = vcl_sqrt(xgraph->area());

      //> Find the edgemap in the pyramid so that we can keep the xgraph scale at
      // the base level while keeping the ratio intact

      // target scale for edgemap in pyramd
      double target_edgemap_scale = this->params.base_xgraph_size / cur_xgraph_scale;

      // find the edgemap with the clostest scale
      // scale_difference = |log(scale1 / scale2)|
      vnl_vector<double > scale_diff(list_edgemap_scales.size(), vnl_numeric_traits<double >::maxval);
      for (unsigned i =0; i < scale_diff.size(); ++i)
      {
        scale_diff[i] = vnl_math::abs(vcl_log(list_edgemap_scales[i] / target_edgemap_scale));
      }
      unsigned min_idx = scale_diff.arg_min();
      double actual_edgemap_scale = list_edgemap_scales[min_idx];
      vgl_box_2d<int > edgemap_bbox = list_edgemap_bboxes[min_idx];
      vcl_string edgemap_name = list_edgemap_names[min_idx];
      
      //> Adjust the xgraph to cope with the actual scale of the pyramid edgemap
      if(this->params.use_edgemap_pyramid)
      	xgraph->scale_up(0, 0, actual_edgemap_scale);


      //> compute ccm cost for an area twice as big as the xgraph
      vsol_box_2d_sptr bbox = xgraph->bounding_box();
      int cx = vnl_math::rnd( (bbox->get_min_x() + bbox->get_max_x()) / 2);
      int cy = vnl_math::rnd( (bbox->get_min_y() + bbox->get_max_y()) / 2);
      vgl_point_2d<int > centroid(cx, cy);

      vgl_box_2d<int > roi(vgl_point_2d<int >(cx, cy), 
        2*vnl_math::ceil(bbox->width()), // double the width
        2*vnl_math::ceil(bbox->height()), // double the height
        vgl_box_2d<int >::centre);
      roi = vgl_intersection<int >(roi, edgemap_bbox);


      //> Prepare a CCM cost function
      dbsks_subpix_ccm ccm;
      this->build_subpix_ccm(image_name, edgemap_name, ccm, &roi);
      
     


      // a clone of the xgraph used to perturbed and compute cost
      dbsksp_xshock_graph_sptr xgraph1 = new dbsksp_xshock_graph(*xgraph);
      xgraph1->compute_vertex_depths(this->params.root_vid);


      //> Form a list of labels for the contour segments
      vcl_vector<vcl_string > cfrag_labels;
      for (dbsksp_xshock_graph::edge_iterator eit = xgraph1->edges_begin(); eit !=
        xgraph1->edges_end(); ++eit)
      {
        dbsksp_xshock_edge_sptr xe = *eit;
        unsigned eid = xe->id();

        cfrag_labels.push_back(vul_sprintf("%d-L", eid)); 
        cfrag_labels.push_back(vul_sprintf("%d-R", eid)); 
      }

      //> Perturb the groundtruth xgraph around its current state and compute ccm cost
      vnl_matrix<float > ccm_cost(this->params.num_samples_per_xgraph, 2*xgraph->number_of_edges());
      ccm_cost.fill(0);
      vnl_random randomizer(timer.system());
      for (int i_trial =0; i_trial < this->params.num_samples_per_xgraph; ++i_trial)
      {
        vcl_cout << " " << i_trial;
        vcl_cout.flush();

        //> perturbe each node indenpendently
        for (dbsksp_xshock_graph::vertex_iterator vit = xgraph1->vertices_begin();
          vit != xgraph1->vertices_end(); ++vit)
        {
          dbsksp_xshock_node_sptr xv = *vit;
          
          // skip terminal nodes
          if (xv->degree() == 1)
            continue;

          double dx = randomizer.drand32(-this->params.perturb_delta_x, this->params.perturb_delta_x);
          double dy = randomizer.drand32(-this->params.perturb_delta_y, this->params.perturb_delta_y);

          xv->set_pt(xv->pt() + vgl_vector_2d<double >(dx, dy));
        }
        xgraph1->update_all_degree_1_nodes();

        //> Loop thru the edges / xfrags and compute cost
        vnl_vector<float > ccm_cost_vec;
        if (this->compute_xgraph_ccm_cost(ccm, biarc_sampler, xgraph1, list_prototype_eid, ccm_cost_vec))
        {
          ccm_cost.set_row(i_trial, ccm_cost_vec);
        }

        //> reset xgraph1 back to its original state
        for (dbsksp_xshock_graph::vertex_iterator vit = xgraph1->vertices_begin();
          vit != xgraph1->vertices_end(); ++vit)
        {
          dbsksp_xshock_node_sptr xv1 = *vit;
          
          // skip terminal nodes
          if (xv1->degree() == 1)
            continue;

          // corresponding xnode in xgraph
          dbsksp_xshock_node_sptr xv = xgraph->node_from_id(xv1->id());
          xv1->set_pt(xv->pt());
        }
        xgraph1->update_all_degree_1_nodes();
      } // trial
      vcl_cout << vcl_endl; 

      // save result to xml file
      for (unsigned r =0; r < ccm_cost.rows(); ++r)
      {
        for (unsigned c =0; c < ccm_cost.cols(); ++c)
        {
          os_pos << " " << ccm_cost(r, c);
        }
        os_pos << "\n";
      }
    } // i_xgraph
  } // image
  os_pos << "</data>\n";

  os_pos << "</xgraph_ccm_cost>\n";



  //// write out xml file
  //vcl_ofstream os(this->params.pos_output_file.c_str(), vcl_ios::out);
  //bxml_write(os, doc);
  return true;

}






//------------------------------------------------------------------------------
//: Gather list of groundtruth xgraph file names
bool dbsks_train_ccm_model::
build_grouping_of_gt_xgraph_fnames(vcl_map<vcl_string, vcl_vector<vcl_string > >& 
                                   map_image_name_to_xgraph_fname)
{
  //> parse input file containing list of xshock graphs
  vcl_vector<vcl_string > xgraph_filenames;
  if (!dbul_parse_string_list(this->params.list_gt_xgraph, xgraph_filenames))
  {
    vcl_cout << "ERROR: couldn't load xgraph list file:" << this->params.list_gt_xgraph << vcl_endl;
    return false;
  }

  // group the xgraph filenames by the names of the original images
  for (unsigned i =0; i < xgraph_filenames.size(); ++i)
  {
    vcl_string xgraph_filename = xgraph_filenames[i];
    
    // get the image name associated with this xgraph
    xgraph_filename = vul_file::strip_directory(xgraph_filename); //applelogos_another.xgraph.0.prototype1.xml

    // keep removing the "extension" till only one component is left
    vcl_string image_name = xgraph_filename;
    while (!vul_file::extension(image_name).empty())
    {
      image_name = vul_file::strip_extension(image_name);
    }
    map_image_name_to_xgraph_fname[image_name].push_back(xgraph_filename);
  }

  return true;
}



//------------------------------------------------------------------------------
//: Build a biarc sampler to sample fragment boundaries
bool dbsks_train_ccm_model::
build_biarc_sampler(dbsks_biarc_sampler& biarc_sampler)
{
  

  //> Construct a biarc sampler ................................................
  vcl_cout << "\nConstructing a biarc sampler ...";

  biarc_sampler.clear();

  // Construct a ccm model using the available parameters
  // and then use it to construct a biarc sampler, to maintain consistency
  dbsks_xgraph_ccm_model_sptr ccm_model = new dbsks_xgraph_ccm_model();
  ccm_model->set_base_xgraph_size(this->params.base_xgraph_size);

  dbsks_ccm_params ccm_params;
  ccm_params.distance_threshold_     = this->params.distance_threshold; 
  ccm_params.distance_tol_near_zero_ = this->params.distance_tol_near_zero;
  ccm_params.orient_threshold_       = this->params.orient_threshold;
  ccm_params.orient_tol_near_zero_   = this->params.orient_tol_near_zero;
  ccm_params.weight_chamfer_         = this->params.weight_chamfer; 
  ccm_params.weight_edge_orient_     = this->params.weight_edge_orient;
  ccm_params.weight_contour_orient_  = this->params.weight_contour_orient; 
  ccm_params.nbins_0topi_            = this->params.nbins_0topi;
  ccm_params.local_window_width_     = this->params.local_window_width;
  

  ccm_model->set_ccm_params(ccm_params);
  ccm_model->set_root_vid(this->params.root_vid);
  ccm_model->build_biarc_sampler(biarc_sampler);

  return true;
}

//------------------------------------------------------------------------------
//: Print header info in XML format
vcl_ostream& dbsks_train_ccm_model::
print_xml_header(vcl_ostream& os)
{
  vcl_vector<vcl_string > prototype_bnd_frag_labels;
  vcl_vector<unsigned > list_prototype_eid;
  this->form_prototype_bnd_frag_labels(prototype_bnd_frag_labels, list_prototype_eid);
  os << "<header>\n";

  // print list of positive images
  os << "<list_pos_image>\n";
  for (unsigned i =0; i < this->pos_image_fnames.size(); ++i)
  {
    os << vul_sprintf("<file_name>%s</file_name>\n", this->pos_image_fnames[i].c_str());
  }
  os << "</list_pos_image>\n";

  // print list of negative images
  os << "<list_neg_image>\n";
  for (unsigned i =0; i < this->neg_image_fnames.size(); ++i)
  {
    os << vul_sprintf("<file_name>%s</file_name>\n", this->neg_image_fnames[i].c_str());
  }
  os << "</list_neg_image>\n";


  os << vul_sprintf("<distance_threshold> %f </distance_threshold>\n", this->params.distance_threshold)
    << vul_sprintf("<distance_tol_near_zero> %f </distance_tol_near_zero>\n", this->params.distance_tol_near_zero)
    << vul_sprintf("<orient_threshold> %f </orient_threshold>\n", this->params.orient_threshold)
    << vul_sprintf("<orient_tol_near_zero> %f </orient_tol_near_zero>\n", this->params.orient_tol_near_zero)
    << vul_sprintf("<weight_chamfer> %f </weight_chamfer>\n", this->params.weight_chamfer)
    << vul_sprintf("<weight_edge_orient> %f </weight_edge_orient>\n", this->params.weight_edge_orient)
    << vul_sprintf("<weight_contour_orient> %f </weight_contour_orient>\n", this->params.weight_contour_orient)
    << vul_sprintf("<local_window_width> %f </local_window_width>\n", this->params.local_window_width)
    << vul_sprintf("<root_vid> %d </root_vid>\n", this->params.root_vid)
    << vul_sprintf("<base_xgraph_size> %f </base_xgraph_size>\n", this->params.base_xgraph_size)
    << vul_sprintf("<nbins_0topi> %d </nbins_0topi>\n", this->params.nbins_0topi);

  os  << vul_sprintf("<num_samples_per_xgraph> %d </num_samples_per_xgraph>\n", this->params.num_samples_per_xgraph)
    << vul_sprintf("<perturb_delta_x> %f </perturb_delta_x>\n", this->params.perturb_delta_x)
    << vul_sprintf("<perturb_delta_y> %f </perturb_delta_y>\n", this->params.perturb_delta_y);

  os << vul_sprintf("<num_bnd_frags> %d </num_bnd_frags>\n", prototype_bnd_frag_labels.size());
  os << vul_sprintf("<bnd_frag_labels>\n");
  for (unsigned i_label =0; i_label < prototype_bnd_frag_labels.size(); ++i_label)
  {
    os << vul_sprintf("<label> %s </label>\n", prototype_bnd_frag_labels[i_label].c_str());
  }
  os << vul_sprintf("</bnd_frag_labels>\n");
  os << "</header>\n";
  return os;
}










//------------------------------------------------------------------------------
//: Collect info about an edgemap pyramid (name and width)
bool dbsks_train_ccm_model::
gather_edgemap_pyramid_info(const vcl_string& image_name, 
                            vcl_vector<vcl_string >& list_edgemap_names,
                            vcl_vector<vgl_box_2d<int > >& list_edgemap_bboxes)
{
  // clean up;
  list_edgemap_names.clear();
  list_edgemap_bboxes.clear();

  // load the names of edgemap pyramid file
  // edgmap     : ./applelogos_another/applelogos_another_00_pb_edges.png
  // edgeorient : ./applelogos_another/applelogos_another_00_pb_orient.txt
  vcl_string edgemap_regexp = this->params.edgemap_folder + "/" + image_name +  "/" + image_name + "*" + this->params.edgemap_ext;

  // sort the edgemap by their area, in decreasing order
  vcl_map<int, vcl_string > map_area2name;
  vcl_map<int, vgl_box_2d<int > > map_area2bbox;
  for (vul_file_iterator fn = edgemap_regexp; fn; ++fn)
  {
    vil_image_resource_sptr img = vil_load_image_resource(fn());
    if (!img)
    {
      continue;
    }
    vcl_string fname = vul_file::strip_directory(fn());
    fname = fname.substr(0, fname.size()-this->params.edgemap_ext.size());
    
    vgl_point_2d<int > min_point(0, 0);
    vgl_point_2d<int > max_point(img->ni()-1, img->nj()-1);
    vgl_box_2d<int > bbox(min_point, max_point);
    int area = bbox.area();

    // insert to the sorted map
    map_area2name.insert(vcl_make_pair(-area, fname));
    map_area2bbox.insert(vcl_make_pair(-area, bbox));
  }

  // Save the sorted list of names and bbox'es to vectors
  for (vcl_map<int, vcl_string >::iterator iter = map_area2name.begin(); iter !=
    map_area2name.end(); ++iter)
  {
    list_edgemap_names.push_back(iter->second);
  }

  for (vcl_map<int, vgl_box_2d<int > >::iterator iter = map_area2bbox.begin();
    iter != map_area2bbox.end(); ++iter)
  {
    list_edgemap_bboxes.push_back(iter->second);
  }

  return true;
}








//------------------------------------------------------------------------------
//: Construct a CCM cost calculator for a given edgemap
// if "Region of Interest" roi = 0, the whole edge map is computed
bool dbsks_train_ccm_model::
build_subpix_ccm(const vcl_string& image_name, const vcl_string& edgemap_name,
                 dbsks_subpix_ccm& ccm, const vgl_box_2d<int >* roi)
{
  //> Load the edgemap and edge orient files to construct an edgemap
  vcl_string edgemap_file = this->params.edgemap_folder + "/" + image_name + "/" + 
    edgemap_name + this->params.edgemap_ext;
  vcl_string edgeorient_file = this->params.edgemap_folder + "/" + image_name + "/" + 
    edgemap_name + this->params.edgeorient_ext;
  dbdet_edgemap_sptr edgemap = dbsks_load_subpix_edgemap(edgemap_file, edgeorient_file, 15.0f, 255.0f);

  if (!edgemap)
  {
    return false;
  }

  // ccm params
  ccm.set_distance_params(this->params.distance_threshold, this->params.distance_tol_near_zero);
  ccm.set_orient_params(this->params.orient_threshold, this->params.orient_tol_near_zero);
  ccm.set_local_window_width(this->params.local_window_width);
  ccm.set_nbins_0topi(this->params.nbins_0topi);
  ccm.set_weights(this->params.weight_chamfer, this->params.weight_edge_orient, this->params.weight_contour_orient);

  // data source
  ccm.set_edgemap(edgemap);

  // Determine the region to pre-compute CCM cost for
  vgl_box_2d<int > ccm_roi;
  if (roi) // bounding box was clearly specified
  {
    ccm_roi = *roi;
  }
  else // otherwise, compute for the whole image
  {
    ccm_roi.add(vgl_point_2d<int >(0, 0));
    ccm_roi.add(vgl_point_2d<int >(edgemap->ncols()-1, edgemap->nrows()-1));
  }

  // pre-compute the ccm cost
  ccm.compute(ccm_roi);
  return true;
}



//------------------------------------------------------------------------------
//: Compute ccm cost for each boundary fragment of an xgraph
bool dbsks_train_ccm_model::
compute_xgraph_ccm_cost(const dbsks_subpix_ccm& ccm, 
                        const dbsks_biarc_sampler& biarc_sampler,
                        const dbsksp_xshock_graph_sptr& xgraph1,
                        const vcl_vector<unsigned >& list_prototype_eid,
                        vnl_vector<float >& bnd_frag_ccm_cost)
{
  bnd_frag_ccm_cost.set_size(xgraph1->number_of_edges()*2);

  // Container for samples computed by biarc_sampler
  static vcl_vector<int > x_vec, y_vec, angle_vec;
  double angle_step;

  int col_count = 0;
  for (unsigned i =0; i < list_prototype_eid.size(); ++i)
  {
    unsigned eid = list_prototype_eid[i];
    dbsksp_xshock_edge_sptr xe = xgraph1->edge_from_id(eid);
    
    // That an edge does not exist implies incorrect graph topology. Ignore computation
    if (!xe)
      return false;

    dbsksp_xshock_node_sptr xv_p = xe->parent_node();
    dbsksp_xshock_node_sptr xv_c = xe->child_node();
    assert(xv_p->depth() < xv_c->depth());

    dbsksp_xshock_node_descriptor start = *(xv_p->descriptor(xe));
    dbsksp_xshock_node_descriptor end = xv_c->descriptor(xe)->opposite_xnode();

    // left boundary curve
    biarc_sampler.compute_samples(start.bnd_pt_left(), start.bnd_tangent_left(), 
      end.bnd_pt_left(), end.bnd_tangent_left(), 
      x_vec, y_vec, angle_vec, angle_step);  

    float cost_left = ccm.f(x_vec, y_vec, angle_vec);

    // right boundary curve
    biarc_sampler.compute_samples(start.bnd_pt_right(), start.bnd_tangent_right(), 
      end.bnd_pt_right(), end.bnd_tangent_right(),
      x_vec, y_vec, angle_vec, angle_step);  
    float cost_right = ccm.f(x_vec, y_vec, angle_vec);

    bnd_frag_ccm_cost(col_count++) = cost_left;
    bnd_frag_ccm_cost(col_count++) = cost_right;
  }
  return true;
}




//------------------------------------------------------------------------------
//: Collect negative training data
bool dbsks_train_ccm_model::
collect_negative_data()
{
  // start the timer
  vul_timer timer;
  timer.mark();

  //--------------------------------------------------------------------------//

  //> Get a list of boundary fragment labels;
  vcl_vector<vcl_string > prototype_bnd_frag_labels;
  vcl_vector<unsigned > list_prototype_eid;
  this->form_prototype_bnd_frag_labels(prototype_bnd_frag_labels, list_prototype_eid);

  //> Group the xgraph filenames by the names of the original images
  vcl_map<vcl_string, vcl_vector<vcl_string > > map_image_name_to_xgraph_fname;
  this->build_grouping_of_gt_xgraph_fnames(map_image_name_to_xgraph_fname);

  
  //> Construct a biarc sampler
  dbsks_biarc_sampler biarc_sampler;
  this->build_biarc_sampler(biarc_sampler);


  //> Write header to output file
  vcl_ofstream os(this->params.neg_output_file.c_str(), vcl_ios::out);

  os << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
  os << "<xgraph_ccm_cost version=\"1.0\" cost_type=\"negative\">\n";

  
  this->print_xml_header(os);
  
  
  // 3. Generate ccm cost for postive samples...................................

  os << "<data>\n";
  
  //> Iterate thru the positive images and compute ccm cost for each ground truth xgraph
  for (unsigned i_pos =0; i_pos < pos_image_fnames.size(); ++i_pos)
  {
    // name of image and the groundtruth xgraph in it
    vcl_string image_name = vul_file::strip_extension(pos_image_fnames[i_pos]);
    vcl_vector<vcl_string > xgraph_fnames = map_image_name_to_xgraph_fname[image_name];

    // load the image to get the original size
    vcl_string image_file = this->params.image_folder + "/" + pos_image_fnames[i_pos];
    vil_image_resource_sptr image_rsc = vil_load_image_resource(image_file.c_str(), true);
    if (!image_rsc)
    {
      vcl_cout << "\nERROR: could not load image file: " << image_file << vcl_endl;
      continue;
    }
    int image_width = image_rsc->ni();
    int image_height = image_rsc->nj();
    
    // keep track of the edgemap name by their width
    // \todo need a better way
    vcl_vector<vcl_string > list_edgemap_names;
    vcl_vector<vgl_box_2d<int > > list_edgemap_bboxes;
    this->gather_edgemap_pyramid_info(image_name, list_edgemap_names, list_edgemap_bboxes);

    // compute the scale of for each edgemap level
    vnl_vector<double > list_edgemap_scales(list_edgemap_names.size(), 1.0);
    for (unsigned kk =0; kk < list_edgemap_names.size(); ++kk)
    {
      // note that vgl_box_2d<int >::width() is 1 less the actual width
      list_edgemap_scales[kk] = double(list_edgemap_bboxes[kk].width()+1) / image_width;
    }

    // for each xshock graph, load the appropriate edgemap in the pyramid
    // and compute ccm cost
    for (unsigned i_xgraph =0; i_xgraph < xgraph_fnames.size(); ++i_xgraph)
    {
      // path to the xgraph file
      vcl_string xgraph_fname = xgraph_fnames[i_xgraph];
      vcl_string xgraph_file = this->params.xgraph_folder + "/" + xgraph_fname;

      // load the xgraph
      dbsksp_xshock_graph_sptr xgraph = 0;

      if (!dbsks_load_xgraph(xgraph_file, xgraph))
      {
        vcl_cout << "\nERROR: could not load xgraph file: " << xgraph_fname << vcl_endl;
        continue;
      }

      // Compute scale of the xgraph
      double cur_xgraph_size = vcl_sqrt(xgraph->area());

      //> Find the edgemap in the pyramid so that we can keep the xgraph scale at
      // the base level while keeping the ratio intact

      // target scale for edgemap in pyramd: twice as big as the groundtruth
      double target_edgemap_scale = 2*this->params.base_xgraph_size / cur_xgraph_size; //> note *2

      // find the edgemap with the clostest scale
      // scale_difference = |log(scale1 / scale2)|
      vnl_vector<double > scale_diff(list_edgemap_scales.size(), vnl_numeric_traits<double >::maxval);
      for (unsigned i =0; i < scale_diff.size(); ++i)
      {
        scale_diff[i] = vnl_math::abs(vcl_log(list_edgemap_scales[i] / target_edgemap_scale));
      }
      unsigned min_idx = scale_diff.arg_min();
      double actual_edgemap_scale = list_edgemap_scales[min_idx];
      vgl_box_2d<int > edgemap_bbox = list_edgemap_bboxes[min_idx];
      vcl_string edgemap_name = list_edgemap_names[min_idx];
      
      //> Adjust the xgraph so that it is roughly twice as small as the ground truth xgraph to generate negative data
      if(this->params.use_edgemap_pyramid)
      	xgraph->scale_up(0, 0, target_edgemap_scale/2); //> note /2
      vsol_box_2d_sptr xgraph_bbox = xgraph->bounding_box();

      //> Compute CCM cost for any "big enough" edgemap in the edgemap pyramid

      //>> Define "big-enough": expansion + some padding
      double padding = 8;
      double expansion_ratio = 2;
      int required_width = vnl_math::ceil(xgraph_bbox->width()*expansion_ratio + padding*2);
      int required_height = vnl_math::ceil(xgraph_bbox->height()*expansion_ratio + padding*2);

      //> ignore small edgemaps
      if (edgemap_bbox.width() < required_width || edgemap_bbox.height() < required_height)
      {
        continue;
      }


      //> compute ccm cost for the whole image 
      dbsks_subpix_ccm ccm;
      this->build_subpix_ccm(image_name, edgemap_name, ccm, 0);

      // a clone of the xgraph used to perturbed and compute cost
      dbsksp_xshock_graph_sptr xgraph1 = new dbsksp_xshock_graph(*xgraph);
      xgraph1->compute_vertex_depths(this->params.root_vid);


      //> Compute CCM cost by translating the xgraph around
      xgraph1->update_bounding_box();
      vsol_box_2d_sptr xgraph1_bbox = xgraph1->bounding_box();

      //> Allowed range of variation for the groundtruth xgraph
      double min_dx = padding - xgraph1_bbox->get_min_x();
      double max_dx = edgemap_bbox.width() - padding - xgraph1_bbox->get_max_x();
      double min_dy = padding - xgraph1_bbox->get_min_y();
      double max_dy = edgemap_bbox.width() - padding - xgraph1_bbox->get_max_y();


      ////> Form a list of labels for the contour segments
      //vcl_vector<vcl_string > cfrag_labels;
      //for (dbsksp_xshock_graph::edge_iterator eit = xgraph1->edges_begin(); eit !=
      //  xgraph1->edges_end(); ++eit)
      //{
      //  dbsksp_xshock_edge_sptr xe = *eit;
      //  unsigned eid = xe->id();

      //  cfrag_labels.push_back(vul_sprintf("%d-L", eid)); 
      //  cfrag_labels.push_back(vul_sprintf("%d-R", eid)); 
      //}

      
      //> Translate the gt xgraph randomly in the allowed range and compute ccm cost
      vnl_matrix<float > ccm_cost(this->params.num_samples_per_xgraph, 2*xgraph->number_of_edges());
      vnl_random randomizer(timer.system());
      for (int i_trial =0; i_trial < this->params.num_samples_per_xgraph; ++i_trial)
      {
        vcl_cout << " " << i_trial;
        vcl_cout.flush();

        double dx = randomizer.drand32(min_dx, max_dx);
        double dy = randomizer.drand32(min_dy, max_dy);
        xgraph1->translate(dx, dy);
       
        //> Compute cost vector of the graph
        vnl_vector<float > ccm_cost_vec;
        if (this->compute_xgraph_ccm_cost(ccm, biarc_sampler, xgraph1, list_prototype_eid, ccm_cost_vec))
        {
          ccm_cost.set_row(i_trial, ccm_cost_vec);
        }

        // translate back
        xgraph1->translate(-dx, -dy);
      } // trial
      vcl_cout << vcl_endl; 

      // save result to xml file
      for (unsigned r =0; r < ccm_cost.rows(); ++r)
      {
        for (unsigned c =0; c < ccm_cost.cols(); ++c)
        {
          os << " " << ccm_cost(r, c);
        }
        os << "\n";
      }
    } // i_xgraph
  } // image
  os << "</data>\n";

  os << "</xgraph_ccm_cost>\n";
  return true;
}






//------------------------------------------------------------------------------
//: Old way to collect negative data
bool dbsks_train_ccm_model::
collect_negative_data_old()
{
  //> start the timer
  vul_timer timer;
  timer.mark();

  vcl_vector<vcl_string > prototype_bnd_frag_labels;
  vcl_vector<unsigned > list_prototype_eid;
  this->form_prototype_bnd_frag_labels(prototype_bnd_frag_labels, list_prototype_eid);

  //> Define a biarc sampler
  dbsks_biarc_sampler biarc_sampler;
  this->build_biarc_sampler(biarc_sampler);

  
  //> Write header to output file
  vcl_ofstream os_neg(this->params.neg_output_file.c_str(), vcl_ios::out);
  os_neg << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
  os_neg << "<xgraph_ccm_cost version=\"1.0\" cost_type=\"negative\">\n";
  this->print_xml_header(os_neg);

  
  
  //> Generate ccm cost for postive samples........................................

  //> Extract a list of usable groundtruth xgraphs, given the list of positive images
  vcl_vector<vcl_string > list_gt_xgraph_fnames;

  //>> Group the gt xgraph fnames
  vcl_map<vcl_string, vcl_vector<vcl_string > > map_image_name_to_xgraph_fname;
  this->build_grouping_of_gt_xgraph_fnames(map_image_name_to_xgraph_fname);

  //>> Combine the lists
  for (unsigned i_pos =0; i_pos < pos_image_fnames.size(); ++i_pos)
  {
    vcl_string image_name = vul_file::strip_extension(pos_image_fnames[i_pos]);
    vcl_vector<vcl_string > fnames = map_image_name_to_xgraph_fname[image_name];
    for (unsigned i =0; i < fnames.size(); ++i)
    {
      list_gt_xgraph_fnames.push_back(fnames[i]);
    }
  }


  //> Iterate thru the positive images and compute ccm cost for each ground truth xgraph
  os_neg << "<data>\n"; // header for data block in xml file
  for (unsigned i_neg =0; i_neg < neg_image_fnames.size(); ++i_neg)
  {
    vnl_random randomizer(timer.system());

    // name of image and the groundtruth xgraph in it
    vcl_string image_fname = neg_image_fnames[i_neg];
    vcl_string image_name = vul_file::strip_extension(image_fname);
    vcl_cout << "\n> Image name = " << image_name << "\n";

    // keep track of the edgemap name by their width
    // \todo need a better way
    vcl_vector<vcl_string > list_edgemap_names;
    vcl_vector<vgl_box_2d<int > > list_edgemap_bboxes;
    this->gather_edgemap_pyramid_info(image_name, list_edgemap_names, list_edgemap_bboxes);

    // Computing costs for all levels in the pyramid takes too much time.
    // we only want to compute costs for 3 or 4 levels per negative image pyramid
    unsigned step_level = list_edgemap_bboxes.size() / 3;

    for (unsigned i_edgemap =0; i_edgemap < list_edgemap_bboxes.size(); i_edgemap += step_level)
    {
      // Name of the edgemap
      vcl_string edgemap_name = list_edgemap_names[i_edgemap];

      vcl_cout << "\n> edgemap_name = " << edgemap_name << "\n";

      //> Prepare a CCM cost calculator
      dbsks_subpix_ccm ccm;
      this->build_subpix_ccm(image_name, edgemap_name, ccm, 0);

      // for each xshock graph, load the appropriate edgemap in the pyramid
      // and compute ccm cost
      for (unsigned i_xgraph =0; i_xgraph < list_gt_xgraph_fnames.size(); ++i_xgraph)
      {
        // path to the xgraph file
        vcl_string xgraph_fname = list_gt_xgraph_fnames[i_xgraph];
        vcl_string xgraph_file = this->params.xgraph_folder + "/" + xgraph_fname;

        vcl_cout << "\n> xgraph_fname = " << xgraph_fname << "\n";

        //> load the xgraph the scale it to the base scale
        dbsksp_xshock_graph_sptr xgraph = 0;

        if (!dbsks_load_xgraph(xgraph_file, xgraph))
        {
          vcl_cout << "\nERROR: could not load xgraph file: " << xgraph_fname << vcl_endl;
          continue;
        }

        // Compute scale of the xgraph
        double cur_xgraph_scale = vcl_sqrt(xgraph->area());

        // Scale the xgraph so that its scale is the same as the base-scale
        xgraph->scale_up(0, 0, this->params.base_xgraph_size/cur_xgraph_scale);

        //> Bounding box of the "normalized xgraph"
        xgraph->update_bounding_box();
        vsol_box_2d_sptr xgraph_bbox = xgraph->bounding_box();

        //> Compute CCM cost for any "big enough" edgemap in the edgemap pyramid

        //>> Define "big-enough": expansion + some padding
        double padding = 8;
        double expansion_ratio = 2;
        int required_width = vnl_math::ceil(xgraph_bbox->width()*expansion_ratio + padding*2);
        int required_height = vnl_math::ceil(xgraph_bbox->height()*expansion_ratio + padding*2);

        //> ignore small edgemaps
        vgl_box_2d<int > edgemap_bbox = list_edgemap_bboxes[i_edgemap];
        if (edgemap_bbox.width() < required_width || edgemap_bbox.height() < required_height)
        {
          continue;
        }
    
        // a clone of the xgraph used to perturbed and compute cost
        dbsksp_xshock_graph_sptr xgraph1 = new dbsksp_xshock_graph(*xgraph);
        xgraph1->compute_vertex_depths(this->params.root_vid);

        
        //> Form a list of labels for the contour segments
        vcl_vector<vcl_string > cfrag_labels;
        for (dbsksp_xshock_graph::edge_iterator eit = xgraph1->edges_begin(); eit !=
          xgraph1->edges_end(); ++eit)
        {
          dbsksp_xshock_edge_sptr xe = *eit;
          unsigned eid = xe->id();

          cfrag_labels.push_back(vul_sprintf("%d-L", eid)); 
          cfrag_labels.push_back(vul_sprintf("%d-R", eid)); 
        }

        //> Compare this list with the protype list - skip xgraph if the two
        // lists are not the same

        //> Compute CCM cost by translating the xgraph around
        xgraph1->update_bounding_box();
        vsol_box_2d_sptr xgraph1_bbox = xgraph1->bounding_box();

        //> Allowed range of variation for the groundtruth xgraph
        double min_dx = padding - xgraph1_bbox->get_min_x();
        double max_dx = edgemap_bbox.width() - padding - xgraph1_bbox->get_max_x();
        double min_dy = padding - xgraph1_bbox->get_min_y();
        double max_dy = edgemap_bbox.width() - padding - xgraph1_bbox->get_max_y();

        //> Translate the gt xgraph randomly in the allowed range and compute ccm cost
        vnl_matrix<float > ccm_cost(this->params.num_samples_per_xgraph, 2*xgraph1->number_of_edges());

        // generate random samples within this box
        vcl_cout << "\n> Trial = ";
        for (int i_trial =0; i_trial < this->params.num_samples_per_xgraph; ++i_trial)
        {
          vcl_cout << " " << i_trial;
          vcl_cout.flush();

          double dx = randomizer.drand32(min_dx, max_dx);
          double dy = randomizer.drand32(min_dy, max_dy);
          xgraph1->translate(dx, dy);

          //> Loop thru the edges / xfrags and compute cost
          int col_count = 0;

          vnl_vector<float > ccm_cost_vec;
          if (this->compute_xgraph_ccm_cost(ccm, biarc_sampler, xgraph1, list_prototype_eid, ccm_cost_vec))
          {
            ccm_cost.set_row(i_trial, ccm_cost_vec);
          }

          // translate back
          xgraph1->translate(-dx, -dy);
        } // trial
        vcl_cout << vcl_endl;
        // save result to xml file
        vcl_cout << "\n> Saving ccm_cost .... ";
        for (unsigned r =0; r < ccm_cost.rows(); ++r)
        {
          for (unsigned c =0; c < ccm_cost.cols(); ++c)
          {
            os_neg << " " << ccm_cost(r, c);
          }
          os_neg << "\n";
        }
        vcl_cout << "[ OK ]\n";
      } // i_edgemap
    } // i_xgraph
  } // image
  os_neg << "</data>\n";

  os_neg << "</xgraph_ccm_cost>\n";

  return true;
}










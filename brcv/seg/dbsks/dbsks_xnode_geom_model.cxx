// This is file shp/dbsksp/dbsks_xnode_geom_model.cxx

//:
// \file

#include "dbsks_xnode_geom_model.h"

#include <dbsks/dbsks_utils.h>
#include <dbsks/dbsks_xshock_utils.h>

#include <mbl/mbl_data_array_wrapper.h>
#include <pdf1d/pdf1d_gaussian_builder.h>
#include <pdf1d/pdf1d_pdf.h>


#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <container/vcl_vector_to_vnl_vector.h>


// =============================================================================
// dbsks_xnode_geom_model
// =============================================================================

//------------------------------------------------------------------------------
//: Set parameter range
void dbsks_xnode_geom_model::
set_param_range(double min_psi, double max_psi,
                double min_radius, double max_radius,
                double min_phi, double max_phi,
                double min_phi_diff, double max_phi_diff,
                double graph_size)
{  
  //> psi pdf
  
  // make sure the range is less than 2*Pi
  max_psi = vnl_math_min(max_psi, min_psi + 2*vnl_math::pi);
  //
  dbsks_regularize_min_max_values(min_psi, max_psi);
  this->psi_model_.set(min_psi, max_psi);

  //> radius pdf
  
  // make sure radius is always positive
  min_radius = vnl_math_max(1.0, min_radius);
  max_radius = vnl_math_max(1.0, max_radius);
  //
  dbsks_regularize_min_max_values(min_radius, max_radius);
  this->radius_model_.set(min_radius, max_radius);

  //> phi pdf

  // make sure phi is > 0 and less then pi
  min_phi = vnl_math_max(min_phi, vnl_math::pi / 36);
  max_phi = vnl_math_min(max_phi, vnl_math::pi - vnl_math::pi/36);
  min_phi = vnl_math_min(min_phi, max_phi);
  //
  dbsks_regularize_min_max_values(min_phi, max_phi);
  this->phi_model_.set(min_phi, max_phi);

  //> phi_diff pdf
  dbsks_regularize_min_max_values(min_phi_diff, max_phi_diff);
  this->phi_diff_model_.set(min_phi_diff, max_phi_diff);

  this->graph_size_ = graph_size;
  return;
}


//------------------------------------------------------------------------------
//: Get the parameter range
void dbsks_xnode_geom_model::
get_param_range(double& min_psi, double& max_psi,
                double& min_radius, double& max_radius,
                double& min_phi, double& max_phi,
                double& min_phi_diff, double& max_phi_diff,
                double& graph_size)
{
  // retrieve range from prob distribution
  min_psi = this->psi_model_.lo();
  max_psi = this->psi_model_.hi();

  min_radius = this->radius_model_.lo();
  max_radius = this->radius_model_.hi();
  
  min_phi = this->phi_model_.lo();
  max_phi = this->phi_model_.hi();
  
  min_phi_diff = this->phi_diff_model_.lo();
  max_phi_diff = this->phi_diff_model_.hi();

  graph_size = this->graph_size_;

  return;
}





//------------------------------------------------------------------------------
//: Get data values for a geometric attribute
// Return false if the attribute does not exist
bool dbsks_xnode_geom_model::
get_attr_data(const vcl_string& attr_name, vcl_vector<double >& attr_values) const
{
  // clean up first
  attr_values.clear();

  vcl_map<vcl_string, vcl_vector<double > >::const_iterator iter = 
    this->attr_data_.find(attr_name);

  // return false if the specified attribute does not exist
  if (iter == this->attr_data_.end())
    return false;

  // copy to the returned variable
  attr_values = iter->second;
  return true;  
}




//------------------------------------------------------------------------------
//: Build model from computed attribute data
bool dbsks_xnode_geom_model::
build_from_attr_data()
{
  // \experiment different methods to construct grid
  enum compute_method
  {
    MINMAX = 0,
    GAUSSIAN_3STD = 1,
  };
  compute_method method = GAUSSIAN_3STD;


  // raw data
  vcl_vector<double > vcl_psi;
  vcl_vector<double > vcl_phi;
  vcl_vector<double > vcl_radius;
  vcl_vector<double > vcl_phi_diff;

  if (!this->get_attr_data("list_psi", vcl_psi) ||
    !this->get_attr_data("list_phi", vcl_phi) ||
    !this->get_attr_data("list_radius", vcl_radius) ||
    !this->get_attr_data("list_phi_diff", vcl_phi_diff))
  {
    return false;
  }

  
  

  if (method == MINMAX)
  {
    // convert to vnl_vector
    vnl_vector<double > psi = vcl_vector_to_vnl_vector(vcl_psi); 
    vnl_vector<double > phi = vcl_vector_to_vnl_vector(vcl_phi);
    vnl_vector<double > radius = vcl_vector_to_vnl_vector(vcl_radius);
    vnl_vector<double > phi_diff = vcl_vector_to_vnl_vector(vcl_phi_diff);

    // min and max for angle requires a special treatment because they are circular
    double min_psi, max_psi;
    dbsks_compute_angle_minmax(psi, min_psi, max_psi);

    // construct a xfrag geomtric model
    this->set_param_range(min_psi, max_psi, 
      radius.min_value(), radius.max_value(), 
      phi.min_value(), phi.max_value(), 
      phi_diff.min_value(), phi_diff.max_value(), 
      this->graph_size_for_attr_data());
  }
  else if (method == GAUSSIAN_3STD)
  {
    int num_std = 3;

    // build a new one
    pdf1d_gaussian_builder builder;
    pdf1d_pdf* model = builder.new_model();

    // psi - min and max
    vnl_vector<double > psi = vcl_vector_to_vnl_vector(vcl_psi);
    double min_psi, max_psi;
    psi = dbsks_compute_angle_minmax(psi, min_psi, max_psi);
    
    builder.build_from_array(*model, psi.data_block(), psi.size()); 
    min_psi = model->mean() - num_std * vcl_sqrt(model->variance());
    max_psi = model->mean() + num_std * vcl_sqrt(model->variance());
        
    // radius
    vnl_vector<double > radius = vcl_vector_to_vnl_vector(vcl_radius);
    builder.build_from_array(*model, radius.data_block(), radius.size());
    double min_radius = model->mean() - num_std * vcl_sqrt(model->variance());
    double max_radius = model->mean() + num_std * vcl_sqrt(model->variance());

    // phi
    vnl_vector<double > phi = vcl_vector_to_vnl_vector(vcl_phi);
    builder.build_from_array(*model, phi.data_block(), phi.size());
    double min_phi = model->mean() - num_std * vcl_sqrt(model->variance());
    double max_phi = model->mean() + num_std * vcl_sqrt(model->variance());

    // phi_diff
    vnl_vector<double >phi_diff = vcl_vector_to_vnl_vector(vcl_phi_diff);
    builder.build_from_array(*model, phi_diff.data_block(), phi_diff.size());
    double min_phi_diff = model->mean() - num_std * vcl_sqrt(model->variance());
    double max_phi_diff = model->mean() + num_std * vcl_sqrt(model->variance());

    this->set_param_range(min_psi, max_psi, 
      min_radius, max_radius, 
      min_phi, max_phi, 
      min_phi_diff, max_phi_diff, 
      this->graph_size_for_attr_data());

    // deallocate model;
    delete model;
  }
  else
  {
    return false;
  }
  return true;
}


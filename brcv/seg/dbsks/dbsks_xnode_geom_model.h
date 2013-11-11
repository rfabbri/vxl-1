// This is shp/dbsksp/dbsks_xnode_geom_model.h
#ifndef dbsks_xnode_geom_model_h_
#define dbsks_xnode_geom_model_h_

//:
// \file
// \brief A generative model for generating xshock_node
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Nov 11, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_map.h>

#include <pdf1d/pdf1d_flat.h>

// =============================================================================
// dbsks_xnode_geom_model
// =============================================================================


//: A generative model for generating xshock_fragment
class dbsks_xnode_geom_model: public vbl_ref_count
{
public:
  // CONSTRUCTORS/DESTRUCTORS/INITIALIZATION -----------------------------------
  
  //: constructor
  dbsks_xnode_geom_model(){};

  //: destructor
  virtual ~dbsks_xnode_geom_model(){};
  
  // DATA ACCESS ---------------------------------------------------------------

  //: Set parameter range
  void set_param_range(double min_psi, double max_psi,
    double min_radius, double max_radius,
    double min_phi, double max_phi,
    double min_phi_diff, double max_phi_diff,
    double graph_size);

  //: Get the parameter range
  void get_param_range(double& min_psi, double& max_psi,
    double& min_radius, double& max_radius,
    double& min_phi, double& max_phi,
    double& min_phi_diff, double& max_phi_diff,
    double& graph_size);


  //: Set data for a geometric attribute. If existing, old data will be overwritten
  void set_attr_data(const vcl_string& attr_name, const vcl_vector<double >& attr_values)
  {
    this->attr_data_[attr_name] = attr_values;
  }

  //: Get data values for a geometric attribute
  // Return false if the attribute does not exist
  bool get_attr_data(const vcl_string& attr_name, vcl_vector<double >& attr_values) const; 

  //: Return const reference to the attribute data map
  const vcl_map<vcl_string, vcl_vector<double > >& attr_data() const 
  {return this->attr_data_; };

  //: Clear the list of attribute data
  void clear_attr_data()
  {
    this->attr_data_.clear();
  }

  //: Size of graph in attribute data
  double graph_size_for_attr_data() const {return this->graph_size_for_attr_data_; }
  void set_graph_size_for_attr_data(double graph_size)
  {
    this->graph_size_for_attr_data_ = graph_size;
  }
  
  // UTILITY -------------------------------------------------------------------

  //: Build model from computed attribute data
  bool build_from_attr_data();

  
  // I/O =======================================================================

  //: write info of the dbskbranch to an output stream
  virtual void print(vcl_ostream & os){};

protected:
  // min and max of each parameter

  //double psi_[2]; // to be depreciated
  //double radius_[2];
  //double phi_[2];
  //double phi_diff_[2];

  pdf1d_flat psi_model_;
  pdf1d_flat radius_model_;
  pdf1d_flat phi_model_; 
  pdf1d_flat phi_diff_model_; 

  double graph_size_;


  // list of data related to the fragments
  vcl_map<vcl_string, vcl_vector<double > > attr_data_;
  double graph_size_for_attr_data_;

};

#endif // shp/dbsksp/dbsks_xnode_geom_model.h



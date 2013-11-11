// This is shp/dbsksp/dbsks_xfrag_geom_model.h
#ifndef dbsks_xfrag_geom_model_h_
#define dbsks_xfrag_geom_model_h_

//:
// \file
// \brief A generative model for generating xshock_fragment
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Oct 13, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsksp/dbsksp_xshock_node_descriptor.h>

#include <dbsksp/dbsksp_xshock_fragment.h>

#include <pdf1d/pdf1d_flat.h>
#include <pdf1d/pdf1d_gaussian.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <dbsks/dbsks_biarc_sampler.h>

// =============================================================================
// dbsks_xfrag_geom_model
// =============================================================================


class pdf1d_pdf;
class pdf1d_sampler;



//: A generative model for generating xshock_fragment
class dbsks_xfrag_geom_model: public vbl_ref_count
{
public:
  static vnl_random random_engine;

  // CONSTRUCTORS/DESTRUCTORS/INITIALIZATION -----------------------------------
  
  //: constructor
  dbsks_xfrag_geom_model();

  //: destructor
  virtual ~dbsks_xfrag_geom_model();
  
  // DATA ACCESS ---------------------------------------------------------------

  //: Set start and end node descriptor
  void set(const dbsksp_xshock_node_descriptor& ref_start,
    const dbsksp_xshock_node_descriptor& ref_end)
  { 
    this->ref_start_ = ref_start;
    this->ref_end_ = ref_end;
  }

  //: Set parameter range
  void set_param_range(double min_psi_start, double max_psi_start,
    double min_r_start, double max_r_start,
    double min_phi_start, double max_phi_start,
    double min_alpha_start, double max_alpha_start,
    double min_chord, double max_chord,
    double min_dpsi, double max_dpsi,
    double min_r_end, double max_r_end,
    double min_phi_end, double max_phi_end,
    double graph_size);

  //: Get the parameter range
  void get_param_range(double& min_psi_start, double& max_psi_start,
    double& min_r_start, double& max_r_start,
    double& min_phi_start, double& max_phi_start,
    double& min_alpha_start, double& max_alpha_start,
    double& min_chord, double& max_chord,
    double& min_dpsi, double& max_dpsi,
    double& min_r_end, double& max_r_end,
    double& min_phi_end, double& max_phi_end,
    double& graph_size);


  //: Get range of psi start
  void get_range_psi_start(double& min_psi_start, double& max_psi_start) const;

  //: Get range of phi start
  void get_range_phi_start(double& min_phi_start, double& max_phi_start) const;

  //: Get range of chord length
  void get_range_chord(double& min_chord, double& max_chord) const;

  //: Get range of alpha - angle between shock chord and shock tangent
  void get_range_alpha_start(double& min_alpha, double& max_alpha) const;


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
  { this->attr_data_.clear(); }

  //: Size of graph in attribute data
  double graph_size_for_attr_data() const {return this->graph_size_for_attr_data_; }
  void set_graph_size_for_attr_data(double graph_size)
  { this->graph_size_for_attr_data_ = graph_size; }

  //: biarc sampler
  dbsks_biarc_sampler* biarc_sampler() const { return this->biarc_sampler_; }
  void set_biarc_sampler(dbsks_biarc_sampler* new_sampler)
  {
    this->biarc_sampler_ = new_sampler;
  }
  
  void set_nkdiff_threshold(double t)
  {
      this->nkdiff_threshold_ = t;
  }

  // UTILITY -------------------------------------------------------------------

  //: Generate samples for the ending node descriptor, given the the source descriptor
  bool sample_end_given_start(const dbsksp_xshock_node_descriptor& start, int num_samples,
    vcl_vector<dbsksp_xshock_node_descriptor >& xdesc_list, double graph_size)
  {
    //// sampling method 2 - min-max in model
    //if (graph_size == 0)
    //  graph_size = this->graph_size_;
    //return this->sample_end_given_start_using_model_minmax_range(start, graph_size, 
    //  num_samples, xdesc_list);

    // sampling method 3 - use min-max in model and guarantees all samples correspond to legal fragments
    return this->sample_legal_end_given_start_using_model_minmax_range(start, graph_size, 
      num_samples, xdesc_list);  
  }

  //: Generate samples for the ending node descriptor, hard-coded variation range
  bool sample_end_given_start_using_fixed_range(const dbsksp_xshock_node_descriptor& start, int num_samples,
    vcl_vector<dbsksp_xshock_node_descriptor >& xdesc_list);

  //: Generate samples for the ending node descriptor, using minmax-range from model
  bool sample_end_given_start_using_model_minmax_range(const dbsksp_xshock_node_descriptor& start, 
    double graph_size, int num_samples, vcl_vector<dbsksp_xshock_node_descriptor >& xdesc_list);

  bool sample_legal_end_given_start_using_model_minmax_range(const dbsksp_xshock_node_descriptor& start, 
                                                double graph_size, int num_samples, 
                                                vcl_vector<dbsksp_xshock_node_descriptor >& xdesc_list);

  //: Uniform sampling of the starting location of the fragment given the allowed
  // deviation from the initial location
  bool sample_start_position(int num_samples, double delta_x, double delta_y,
    vcl_vector<double >& x, vcl_vector<double >& y);

  //: Generate samples of the position of the starting node which form a grid surrounding
  // its current position
  bool sample_start_position(double step_x, int num_x_backward, int num_x_forward,
    double step_y, int num_y_backward, int num_y_forward, 
    vcl_vector<double >& xs, vcl_vector<double >& ys);

  //: Uniform sampling of the intrinsic properties of the start descriptor, given the allowed range
  bool sample_start_idesc(int num_samples, double delta_psi, double delta_phi, double delta_log2r,
    vcl_vector<double >& psi, vcl_vector<double >& phi, vcl_vector<double >& r);


  // Utilities------------------------------------------------------------------
  
  //: Build model from computed attribute data
  bool build_from_attr_data();

  // Geometric constaints-------------------------------------------------------
  
  //: Compute geometric constraints based on collected exemplar data
  void compute_constraints_from_attr_data();

  //: Check whether a fragment satisfy the geometric constaints
  bool check_constraints(const dbsksp_xshock_fragment& xfrag) const
  {
    if (this->biarc_sampler() && this->biarc_sampler()->has_cache_nkdiff())
    {
      return this->check_nkdiff_constraint_using_biarc_sampler_cache(xfrag);
    }
    else
    {
      return this->check_nkdiff_constraint_no_biarc_sampler(xfrag);
    }
  }


  //: Check "normalized curvature difference" constraint using cache values in biarc
  bool check_nkdiff_constraint_using_biarc_sampler_cache(const dbsksp_xshock_fragment& xfrag) const;

  //: Check "normalized curvature difference" constraint - compute from scratch
  bool check_nkdiff_constraint_no_biarc_sampler(const dbsksp_xshock_fragment& xfrag) const;

  //: Check constraint on start width (euclidean distance between two boundary points)
  bool check_start_width_constraint(const dbsksp_xshock_node_descriptor& start, double graph_size) const;

protected:
  //: Compute normalized curvature difference constraint from exemplar data
  void compute_nkdiff_constraint_from_attr_data();

  //: Compute start-width constraint
  void compute_start_width_constraint_from_attr_data();

  //: Check constraint on normalized curvature difference
  bool pass_norm_curvature_diff_constraint(dbsksp_xshock_fragment::bnd_side side,
    double norm_curvature_diff) const;

  
  // I/O------------------------------------------------------------------------


  //: write info of the dbskbranch to an output stream
  virtual vcl_ostream& print(vcl_ostream & os){return os;};

protected:
  dbsksp_xshock_node_descriptor ref_start_;
  dbsksp_xshock_node_descriptor ref_end_;  
  double graph_size_;

  // parameter range
  pdf1d_flat psi_start_model_;
  pdf1d_flat r_start_model_;
  pdf1d_flat phi_start_model_;
  pdf1d_flat alpha_start_model_;
  pdf1d_flat chord_model_;
  pdf1d_flat dpsi_model_;
  pdf1d_flat r_end_model_;
  pdf1d_flat phi_end_model_;

  // Generator
  pdf1d_sampler* alpha_start_sampler_;
  pdf1d_sampler* chord_sampler_;
  pdf1d_sampler* dpsi_sampler_;
  pdf1d_sampler* r_end_sampler_;
  pdf1d_sampler* phi_end_sampler_;

  //> Constraints

  //: Normalized curvature difference
  pdf1d_gaussian nkdiff_model_[2];
  double nkdiff_threshold_;
  dbsks_biarc_sampler* biarc_sampler_;

  //: Start width
  pdf1d_gaussian start_width_model_;
  double start_width_num_std_;

  //> Attribute data
  
  //: list of data related to the fragments
  vcl_map<vcl_string, vcl_vector<double > > attr_data_;
  double graph_size_for_attr_data_;
};

#endif // shp/dbsksp/dbsks_xfrag_geom_model.h



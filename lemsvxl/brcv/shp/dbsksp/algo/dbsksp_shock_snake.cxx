// This is file shp/dbsksp/dbsksp_shock_snake.cxx

//:
// \file

#include "dbsksp_shock_snake.h"

#include <vnl/vnl_math.h>
//#include <vnl/vnl_numeric_traits.h>
#include <vgl/vgl_distance.h>
//
//
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_fragment.h>
#include <dbsksp/dbsksp_shock_edge.h>
//
#include <vpdfl/vpdfl_gaussian.h>
#include <vpdfl/vpdfl_gaussian_builder.h>
#include <vpdfl/vpdfl_sampler_base.h>
#include <dbsksp/algo/dbsksp_twoshapelet_cost_function.h>

#include <vnl/vnl_random.h>
#include <vul/vul_get_timestamp.h>
#include <vnl/algo/vnl_amoeba.h>

// ============================================================================
//  dbsksp_shock_snake
// ============================================================================



//: Optimize a two shapelet to fit to the image
// Return ERROR code
int dbsksp_shock_snake::
optimize_twoshapelet_method_1(const dbsksp_twoshapelet_sptr& init_ss,
                             dbsksp_twoshapelet_sptr& final_ss)
{


  // shock snake cost function
  double lambda = 0;
  dbsksp_twoshapelet_cost_function twoshapelet_cost(init_ss, 
    this->ext_cost_integrator(), lambda);

  // initial parameter
  vnl_vector<double > x_init = twoshapelet_cost.get_free_params(init_ss);

  vcl_cout << "Starting cost = " << twoshapelet_cost.f(x_init) << vcl_endl;

  // structure of free variables 'x'
  //double m0_;
  //double phi1_;
  //double m1_;
  //double phi2_;

  // do stochastic sampling of the space to find roughly where the the minimum is
  // the perform nonlinear minimization

  vpdfl_gaussian_builder builder;

  vpdfl_gaussian pdf;

  int n = 4;
  vnl_vector<double > mean(n);
  vnl_matrix<double > covar(n, n);

  mean = x_init;
  mean(0) = 0;
  //mean(1) = vnl_math::pi_over_2;
  mean(2) = 0;
  //mean(3) = vnl_math::pi_over_2;
  

  covar.set_identity();
  // set the variance of the variables
  covar(0,0) = 0.15*0.15;
  covar(1,1) = 0.25;
  covar(2,2) = 0.15*0.15;
  covar(3,3) = 0.25;
  
  pdf.set(mean, covar);


  // sample the space =======================================
  int n_samples = 100000;

  vpdfl_sampler_base* p_sampler = pdf.new_sampler();

  // Generate lots of samples
  vcl_vector<vnl_vector<double> > data(n_samples);
  data[0] = x_init;
  for (int i=1; i<n_samples; ++i)
    p_sampler->sample(data[i]);

  // now iterate thru the samples to find the mean
  vnl_vector<double > x_min = x_init;
  double min_cost = vnl_numeric_traits<double >::maxval;
  for (int i=0; i<n_samples; ++i)
  {
    double cost = twoshapelet_cost.f(data[i]);
    if (min_cost > cost)
    {
      min_cost = cost;
      x_min = data[i];
    }
  }

  vnl_vector<double > x = x_min;
  vcl_cout << "Min cost after stochastic sampling = " 
    << twoshapelet_cost.f(x) << vcl_endl;


  // update the shape with the new twoshapelet params
  vcl_cout << "Final min cost = " << twoshapelet_cost.f(x) << vcl_endl;

  final_ss = twoshapelet_cost.get_twoshapelet(x);

  return 1;
}


//: Optimize a two shapelet to fit to the image - method 2
// variables are the position, radius, and phi of the end point
// Return ERROR code
int dbsksp_shock_snake::
optimize_twoshapelet_method_2(const dbsksp_twoshapelet_sptr& init_ss,
                              dbsksp_twoshapelet_sptr& final_ss)
{
  // shock snake cost function

  dbsksp_potential_integrator pot_integrator;
  pot_integrator.set_sampling_length(1);
  pot_integrator.set_potential_field(this->potential_image_);

  dbsksp_twoshapelet_type2_cost_fnt ss_cost(init_ss, 
    &pot_integrator, this->lambda());

  
  // fixed parameters
  double x0, y0, psi0, r0, phi0, len;
  ss_cost.get_fixed_params(x0, y0, psi0, r0, phi0, len);



  
  // structure of free variables 'x'
  //double m0_;
  //double phi1_;
  //double m1_;
  //double phi2_;
  //double len_diff
  
  

  

  // do stochastic sampling of the space to find roughly where the the minimum is
  // the perform nonlinear minimization

  // sample the space =======================================
  int n_samples = 100000;
  vcl_vector<vnl_vector<double> > samples;
  samples.reserve(n_samples);

  // get current time to use as seed for random generator
  int secs, msecs;
  vul_get_timestamp(secs, msecs);

  // random generator
  vnl_random random_generator(secs);


  for (int i=0; i< n_samples; ++i)
  {
    // len_diff, which is in range [-1, 1]
    double len_diff = random_generator.drand32(-1, 1);
    
    // phi1 and phi2
    double phi1 = random_generator.drand32(0.3*vnl_math::pi, 0.7*vnl_math::pi);
    double phi2 = random_generator.drand32(0.3*vnl_math::pi, 0.7*vnl_math::pi);

    // m1 and m2 are constraint by phi1 and phi2
    double max_m0 = vnl_math::min( 
      vnl_math::abs(1/vcl_sin(phi0)), vnl_math::abs(1/vcl_sin(phi1)));
    double max_m1 = vnl_math::min( 
      vnl_math::abs(1/vcl_sin(phi1)), vnl_math::abs(1/vcl_sin(phi2)));

    double m0 = random_generator.drand32(-0.7*max_m0, 0.7*max_m0);
    double m1 = random_generator.drand32(-0.7*max_m1, 0.7*max_m1);

    // we're ready to form a sample of the parameter space
    vnl_vector<double > sample(5);
    sample(0) = m0;
    sample(1) = phi1;
    sample(2) = m1;
    sample(3) = phi2;
    sample(4) = len_diff;

    samples.push_back(sample);
  }

  





  // initial free parameter
  vnl_vector<double > x_init = ss_cost.get_free_params(init_ss);

  // now iterate thru the samples to find the min
  vnl_vector<double > x_min = x_init;
  
  double min_cost = ss_cost.f(x_init);
  vcl_cout << "Starting cost = " << ss_cost.f(x_init) << vcl_endl;
  
  for (int i=0; i<n_samples; ++i)
  {
    vnl_vector<double > x = samples[i];
    double cost = ss_cost.f(x);
    
    if (min_cost > cost)
    {
      // enforce hard constraints
      dbsksp_twoshapelet_sptr ss0 = ss_cost.get_twoshapelet(x);
      double width = 2*ss0->shapelet_end()->radius_end() * vcl_sin(ss0->phi2());

      // if <0 then there is no constraints
      if (this->max_width_ > 0 && width > this->max_width_)
        continue;
      if (this->min_width_ > 0 && width < this->min_width_)
        continue;
      
      min_cost = cost;
      x_min = x;
      
    }
  }






  //// non-linear minimizer
  //vnl_amoeba amoeba_minimizer(twoshapelet_cost);

  //// minimizer configuration
  //amoeba_minimizer.set_max_iterations(max_evals); //set_max_function_evals(max_evals);
  //amoeba_minimizer.verbose = true; //set_verbose(true);
  //amoeba_minimizer.set_relative_diameter(init_step); //set_initial_step(init_step);
  //amoeba_minimizer.set_f_tolerance (linmin_xtol); //set_linmin_xtol(linmin_xtol);


  /////////////////////////////////////////////////////
  //// amoeba_minimizer.minimize(x);
  /////////////////////////////////////////////////////

  //// update the shape with the new twoshapelet params
  //vcl_cout << "Min cost = " << twoshapelet_cost.f(x) << vcl_endl;




  vcl_cout << "Min cost after stochastic sampling = " <<  min_cost << vcl_endl;
  final_ss = ss_cost.get_twoshapelet(x_min);

  return 1;
}



//: Optimize a ``terminal twoshapelet'' to fit to the image
// Return ERROR code
int dbsksp_shock_snake::
optimize_terminal_twoshapelet(const dbsksp_shapelet_sptr& init_ss,
                              dbsksp_shapelet_sptr& final_ss)
{
  // shock snake cost function

  dbsksp_potential_integrator pot_integrator;
  pot_integrator.set_sampling_length(1);
  pot_integrator.set_potential_field(this->potential_image_);

  //dbsksp_twoshapelet_type2_cost_fnt ss_cost(init_ss, 
  //  &pot_integrator, this->lambda());

  //
  //// fixed parameters
  //double x0, y0, psi0, r0, phi0, len;
  //ss_cost.get_fixed_params(x0, y0, psi0, r0, phi0, len);



  //
  //// structure of free variables 'x'
  ////double m0_;
  ////double phi1_;
  ////double m1_;
  ////double phi2_;
  ////double len_diff
  //
  //

  //

  //// do stochastic sampling of the space to find roughly where the the minimum is
  //// the perform nonlinear minimization

  //// sample the space =======================================
  //int n_samples = 100000;
  //vcl_vector<vnl_vector<double> > samples;
  //samples.reserve(n_samples);

  //// get current time to use as seed for random generator
  //int secs, msecs;
  //vul_get_timestamp(secs, msecs);

  //// random generator
  //vnl_random random_generator(secs);


  //for (int i=0; i< n_samples; ++i)
  //{
  //  // len_diff, which is in range [-1, 1]
  //  double len_diff = random_generator.drand32(-1, 1);
  //  
  //  // phi1 and phi2
  //  double phi1 = random_generator.drand32(0.2*vnl_math::pi, 0.8*vnl_math::pi);
  //  double phi2 = random_generator.drand32(0.2*vnl_math::pi, 0.8*vnl_math::pi);

  //  // m1 and m2 are constraint by phi1 and phi2
  //  double max_m0 = vnl_math::min( 
  //    vnl_math::abs(1/vcl_sin(phi0)), vnl_math::abs(1/vcl_sin(phi1)));
  //  double max_m1 = vnl_math::min( 
  //    vnl_math::abs(1/vcl_sin(phi1)), vnl_math::abs(1/vcl_sin(phi2)));

  //  double m0 = random_generator.drand32(-max_m0, max_m0);
  //  double m1 = random_generator.drand32(-max_m1, max_m1);

  //  // we're ready to form a sample of the parameter space
  //  vnl_vector<double > sample(5);
  //  sample(0) = m0;
  //  sample(1) = phi1;
  //  sample(2) = m1;
  //  sample(3) = phi2;
  //  sample(4) = len_diff;

  //  samples.push_back(sample);
  //}

  //





  //// initial free parameter
  //vnl_vector<double > x_init = ss_cost.get_free_params(init_ss);

  //// now iterate thru the samples to find the min
  //vnl_vector<double > x_min = x_init;
  //
  //double min_cost = ss_cost.f(x_init);
  //vcl_cout << "Starting cost = " << ss_cost.f(x_init) << vcl_endl;
  //
  //for (int i=0; i<n_samples; ++i)
  //{
  //  vnl_vector<double > x = samples[i];
  //  double cost = ss_cost.f(x);
  //  if (min_cost > cost)
  //  {
  //    min_cost = cost;
  //    x_min = x;
  //  }
  //}






  ////// non-linear minimizer
  ////vnl_amoeba amoeba_minimizer(twoshapelet_cost);

  ////// minimizer configuration
  ////amoeba_minimizer.set_max_iterations(max_evals); //set_max_function_evals(max_evals);
  ////amoeba_minimizer.verbose = true; //set_verbose(true);
  ////amoeba_minimizer.set_relative_diameter(init_step); //set_initial_step(init_step);
  ////amoeba_minimizer.set_f_tolerance (linmin_xtol); //set_linmin_xtol(linmin_xtol);


  ///////////////////////////////////////////////////////
  ////// amoeba_minimizer.minimize(x);
  ///////////////////////////////////////////////////////

  ////// update the shape with the new twoshapelet params
  ////vcl_cout << "Min cost = " << twoshapelet_cost.f(x) << vcl_endl;




  //vcl_cout << "Min cost after stochastic sampling = " <<  min_cost << vcl_endl;
  //final_ss = ss_cost.get_twoshapelet(x_min);

  return 1;
}




// ----------------------------------------------------------------------------
//: Optimize the two active edges to fit the boundary
int dbsksp_shock_snake::
optimize_two_active_edges(const dbsksp_shock_node_sptr& source_node,
                          const dbsksp_shock_edge_sptr& source_edge,
                          dbsksp_twoshapelet_sptr& final_twoshapelet)
{
  // >> The evolving object: a twoshapelet
  
  dbsksp_twoshapelet_sptr twoshapelet = this->build_twoshapelet(source_node, 
    source_edge);  

  if (!twoshapelet) return 0;


  // Run Optimization
  this->optimize_twoshapelet_method_2(twoshapelet, final_twoshapelet);
  // ----------------------
  
  source_edge->fragment()->update_edge_with_shapelet(
    final_twoshapelet->shapelet_start(), source_node);

  dbsksp_shock_node_sptr mid_node = source_edge->opposite(source_node);

  dbsksp_shock_edge_sptr end_edge = 
    this->shock_graph()->cyclic_adj_succ(source_edge, mid_node);

  end_edge->fragment()->update_edge_with_shapelet(final_twoshapelet->shapelet_end(), 
    source_edge->opposite(source_node));

  return 1;
}



// ----------------------------------------------------------------------------
//: Extend the shock graph at the ``terminal_edge" with two fragments whose
// chord lengths are ``chord0" and ``chord1", then deform the two new fragments
// to match with the underlying image
int dbsksp_shock_snake::
extend_and_deform_2_edges(dbsksp_shock_edge_sptr& terminal_edge,
                  double chord0, double chord1,
                  dbsksp_twoshapelet_sptr& final_twoshapelet)
{
  // Preliminary check
  if (!terminal_edge->is_terminal_edge()) return 0;

  // Get a hold on the nodes
  dbsksp_shock_node_sptr start_node = (terminal_edge->target()->degree()==1) ?
    terminal_edge->source() : terminal_edge->target();
  dbsksp_shock_node_sptr end_node = terminal_edge->opposite(start_node);

  // add first fragment
  dbsksp_shock_edge_sptr e0 = this->shock_graph()->insert_shock_edge(
    terminal_edge, chord0, end_node == terminal_edge->target(), 0);
  terminal_edge = *end_node->edges_begin();
  this->shock_graph()->compute_all_dependent_params();
  

  // add second fragment
  dbsksp_shock_edge_sptr e1 = this->shock_graph()->insert_shock_edge(
    terminal_edge, chord1, end_node == terminal_edge->target(), 0);
  terminal_edge = *end_node->edges_begin();
  this->shock_graph()->compute_all_dependent_params();
  

  // optimize them
  return this->optimize_two_active_edges(start_node, e0, final_twoshapelet);
}



//: Extend the shock graph at an edge with one fragment so that the total 
// chord length is ``chord_length" then deform the two new fragments to match
// with the underlying image
int dbsksp_shock_snake::
add_one_edge_and_deform(const dbsksp_shock_node_sptr& source_node,
                    dbsksp_shock_edge_sptr& source_edge,
                    double total_chord, 
                    dbsksp_twoshapelet_sptr& final_twoshapelet)
{
  // Get a hold on the nodes
  dbsksp_shock_node_sptr mid_node = source_edge->opposite(source_node);

  double chord1 = total_chord - source_edge->chord_length();
  if (chord1 < 0) chord1 = 0;
  // add first fragment
  dbsksp_shock_edge_sptr e0 = this->shock_graph()->insert_shock_edge(
    source_edge, chord1, mid_node == source_edge->source(), 0);
  this->shock_graph()->compute_all_dependent_params();

  final_twoshapelet = this->build_twoshapelet(source_node, source_edge); 

  // optimize the two shapelet
  return this->optimize_two_active_edges(source_node, source_edge, final_twoshapelet);
}


// ----------------------------------------------------------------------------
//: Extend the shock graph at the ``terminal_edge" with two fragments whose
// chord lengths are ``chord0" and ``chord1", then deform the two new fragments
// to match with the underlying image. Do this ``num_times" times
int dbsksp_shock_snake::
extend_and_deform_2_edges_n_times(dbsksp_shock_edge_sptr& terminal_edge,
                                  double chord0, double chord1,
                                  int num_times,
                                  dbsksp_twoshapelet_sptr& final_twoshapelet)
{
  for (int n=0; n<num_times; ++n)
  {
    if (this->verbal())
    {
      vcl_cerr << "n = " << n << vcl_endl;
    }
    int error_code = this->extend_and_deform_2_edges(terminal_edge,
                  chord0, chord1, final_twoshapelet);
    if (error_code <= 0)
      return error_code;
  }
  return 1;
}

// ----------------------------------------------------------------------------
//: Extend and deform the shock graph at the ``terminal_edge" with two
// fragments untill a ``target_point" is within reach.
int dbsksp_shock_snake::
extend_and_deform_2_edges_till_reach_target(
  dbsksp_shock_edge_sptr& terminal_edge,
  double chord0, double chord1,
  const vgl_point_2d<double >& target_point,
  int max_num_times,
  dbsksp_twoshapelet_sptr& final_twoshapelet)
{
  // Preliminary check
  if (!terminal_edge->is_terminal_edge()) return 0;

  // Get a hold on the nodes
  dbsksp_shock_node_sptr end_node = (terminal_edge->target()->degree()==1) ?
    terminal_edge->target() : terminal_edge->source();
  
  for (int n=0; n<max_num_times; ++n)
  {
    if (this->verbal())
    {
      vcl_cerr << "n = " << n << vcl_endl;
    }
    int error_code = this->extend_and_deform_2_edges(terminal_edge,
                  chord0, chord1, final_twoshapelet);
    if (error_code <= 0)
      return error_code;

    // check if the target point is ``within reach''
    if (vgl_distance(end_node->pt(),target_point) <= (chord0 + end_node->radius()))
    {
      vcl_cerr << "The target point has been reached.\n";
      break;
    }
  }

  return 1;
}


// ----------------------------------------------------------------------------
//: Build a two-shapelet given a source_node and a source_edge
// A group of two edges, starting from source_node and extending in the direction
// of source_edge, are used to construct the two-shapelet
// Return 0 if no twoshapelet is constructed
// Possible reason for failure
// - source_edge is not incident to source_node
// - there is ambiguity in extending to two edges (hitting a A_1^3 node)
dbsksp_twoshapelet_sptr dbsksp_shock_snake::
build_twoshapelet(const dbsksp_shock_node_sptr& source_node,
                  const dbsksp_shock_edge_sptr& source_edge)
{
  // preliminary check
  if (!source_edge->is_vertex(source_node))
    return 0;
  
  // determine middle node
  dbsksp_shock_node_sptr mid_node = source_edge->opposite(source_node);
  
  // determine ending edge
  if (mid_node->degree() != 2)
    return 0;

  dbsksp_shock_edge_sptr end_edge = 
    this->shock_graph()->cyclic_adj_succ(source_edge, mid_node);

  
  // start shapelet
  dbsksp_shapelet_sptr start = source_edge->fragment()->get_shapelet();
  if (source_node != source_edge->source())
    start = start->reversed_dir();

  // end shapelet
  dbsksp_shapelet_sptr end = end_edge->fragment()->get_shapelet();
  if (mid_node != end_edge->source())
  {
    end = end->reversed_dir();
  }

  // form twoshapelet from the two shapelets
  dbsksp_twoshapelet_sptr s = new dbsksp_twoshapelet(
    start->x0(), start->y0(), start->theta0(), start->r0(), 
    start->phi0(), start->m0(), start->len(),
    end->phi0(), end->m0(), end->len(), end->phi_end());
  return s;
}

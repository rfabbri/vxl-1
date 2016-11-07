// This is dbsksp/pro/dbsksp_shock_snake_process.cxx

//:
// \file

#include "dbsksp_shock_snake_process.h"
#include <vnl/algo/vnl_powell.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vnl/vnl_math.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_sobel_3x3.h>




#include <bpro1/bpro1_parameters.h>
#include <bil/algo/bil_edt.h>
#include <bil/algo/bil_edge_indicator.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_fragment.h>
#include <dbsksp/algo/dbsksp_shock_snake_cost_function_old.h>
#include <dbsksp/algo/dbsksp_twoshapelet_cost_function.h>


#include <vpdfl/vpdfl_gaussian.h>
#include <vpdfl/vpdfl_gaussian_builder.h>
#include <vpdfl/vpdfl_sampler_base.h>


//: Constructor
dbsksp_shock_snake_process::
dbsksp_shock_snake_process()
{
  vcl_vector<vcl_string > param_type_choices;
  param_type_choices.push_back("Extrinsic");
  param_type_choices.push_back("Intrinsic");
  param_type_choices.push_back("Both extrinsic and intrinsic");
  param_type_choices.push_back("One active edge");
  param_type_choices.push_back("One active edge - 3 params");
  param_type_choices.push_back("Two active edges");

  vcl_vector<vcl_string > energy_type_choices;
  energy_type_choices.push_back("With alignment");
  energy_type_choices.push_back("Without alignment");

  if( 
    !parameters()->add("Type of parameters: " , "-param_type" , param_type_choices, 0) ||
    !parameters()->add("Type of energy: " , "-energy_type" , energy_type_choices, 0) ||
    !parameters()->add("d max: " , "-dmax" , 20.0f ) ||
    !parameters()->add("d min: " , "-dmin" , 0.0f ) ||
    !parameters()->add( "Max function evaluations: " , "-max_evals" , 10 ) ||
    !parameters()->add( "Initial step size: " , "-init_step" , 0.01f ) ||
    !parameters()->add( "linmin_xtol: " , "-linmin_xtol" , 0.0001f ) ||
    !parameters()->add( "Sampling length: " , "-samp_len" , 1.0f ) ||
    !parameters()->add( "Internal-external energy ration: " , "-lambda" , 1.0f )
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbsksp_shock_snake_process::
~dbsksp_shock_snake_process()
{
}


//: Clone the process
bpro1_process* dbsksp_shock_snake_process::
clone() const
{
  return new dbsksp_shock_snake_process(*this);
}

//: Returns the name of this process
vcl_string dbsksp_shock_snake_process::
name()
{ 
  return "Shock snake"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_shock_snake_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbsksp_shock" );
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_shock_snake_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.clear();
  return to_return;
}

//: Return the number of input frames for this process
int dbsksp_shock_snake_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsksp_shock_snake_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_shock_snake_process::
execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << "In dbsksp_shock_snake_process::execute() - "
             << "not exactly one input images" << vcl_endl;
    return false;
  }

  
  

  // ------------- USER PARAMS --------------------------------------
  unsigned int param_type = 100;
  parameters()->get_value( "-param_type" , param_type);


  unsigned int energy_type = 10;
  parameters()->get_value( "-energy_type" , energy_type);


  int max_evals;
  parameters()->get_value( "-max_evals" , max_evals);

  float ds = 1.0f;
  parameters()->get_value( "-samp_len" , ds);

  float init_step = 0.01f;
  parameters()->get_value( "-init_step", init_step);

  float linmin_xtol = 0.0001f;
  parameters()->get_value( "-linmin_xtol", linmin_xtol);

  float lambda = 0;
  parameters()->get_value( "-lambda", lambda);

  float dmax = 0;
  parameters()->get_value( "-dmax", dmax);

  float dmin = 0;
  parameters()->get_value( "-dmin", dmin);


  // ------------- STORAGE CLASSES --------------------------------------

  // get the shock graph from storage classes
  dbsksp_shock_storage_sptr sksp_storage;
  sksp_storage.vertical_cast(input_data_[0][0]);

  // get the image from the storage class and convert to gray
  vidpro1_image_storage_sptr image_storage;
  image_storage.vertical_cast(input_data_[0][1]);
  vil_image_resource_sptr image_resource = image_storage->get_image();
  vil_image_view<unsigned int > image_view;
  if (image_resource->nplanes()==1)
  {
    unsigned int dummy = 0;
    image_view = *vil_convert_cast(dummy, image_resource->get_view());
  }
  else
  {
    unsigned int dummy = 0;
    image_view = *vil_convert_cast(dummy, 
    vil_convert_to_grey_using_rgb_weighting(image_resource->get_view()));
  }

  // ------------- PROCESS DATA --------------------------------------

  // >> the shock graph
  dbsksp_shock_graph_sptr shock_graph = sksp_storage->shock_graph();

  // >> external energy
  dbsksp_external_energy_function* image_cost = 0;
  
  if (energy_type == 0) // distance transform
  {
    for (unsigned int i=0; i<image_view.ni(); ++i)
    for (unsigned int j=0; j<image_view.nj(); ++j)
      image_view(i, j) = (image_view(i, j) < 128) ? 0 : 255;

    // Compute distance transform of the image
    vil_image_view<float> dt_image;
    bil_edt_signed(image_view, dt_image);

    // the potential energy function based on dt image
    dbsksp_potential_energy_function* dt_cost = new dbsksp_potential_energy_function();
    dt_cost->set_potential_field(dt_image);
    dt_cost->set_sampling_length(ds);

    // set the image cost
    image_cost = dt_cost;
  }
  else // Normalized inverse Gradient magnitude
  {
    vil_image_view<float> grad_mag_cost;
    double sigma = 0.5;
    bil_normalized_inverse_gradient(image_view, sigma, grad_mag_cost);


    // --------------------------------------------------------
    // the potential energy function based on gradient image
    dbsksp_no_alignment_potential_energy_function* grad_cost = 
      new dbsksp_no_alignment_potential_energy_function();
    grad_cost->set_potential_field(grad_mag_cost);
    grad_cost->set_sampling_length(ds);
    grad_cost->d_max_ = dmax;
    grad_cost->d_min_ = dmin;

    // set the image cost
    image_cost = grad_cost;
  }

  // >> Shock model
  if (param_type == 0 || param_type == 1 || param_type == 2)
  {
    // determine the shock model depending on the varying parameters
    dbsksp_shock_model* shock_model = 0;
    if (param_type == 0)
    {
      shock_model = new dbsksp_extrinsic_info_model(shock_graph);
    }
    else if (param_type == 1)
    {
      shock_model = new dbsksp_intrinsic_phi_m_L_shock_model(shock_graph);
    }
    else if (param_type == 2)
    {
      shock_model = new dbsksp_phi_m_L_shock_model(shock_graph);
    }
  

    // set up the cost function
    // shock snake cost function
    dbsksp_shock_snake_cost_function snake_cost;
    snake_cost.set_shock_model(shock_model);
    snake_cost.set_external_energy_function(image_cost);

    // initial parameter !!!!!!
    vnl_vector<double > x;
    shock_model->compute_params_from_shock_graph(x);

    

    // non-linear minimizer
    vnl_powell powell_minimizer(&snake_cost);

    // minimizer configuration
    powell_minimizer.set_max_function_evals(max_evals);
    powell_minimizer.set_trace(true);
    powell_minimizer.set_verbose(true);
    powell_minimizer.set_initial_step(init_step);
    powell_minimizer.set_linmin_xtol(linmin_xtol);
    powell_minimizer.minimize(x);
    vcl_cout << "Start error = " << powell_minimizer.get_start_error() << vcl_endl;
    vcl_cout << "End error = " << powell_minimizer.get_end_error() << vcl_endl;
    vcl_cout << "Failure code = " << powell_minimizer.get_failure_code() << vcl_endl;


    //// non-linear minimizer
    //vnl_amoeba minimizer(snake_cost);
    //minimizer.set_f_tolerance(linmin_xtol);
    //minimizer.set_max_iterations(max_evals);
    //minimizer.set_x_tolerance(linmin_xtol/100);
    //minimizer.verbose = 1;
    //minimizer.minimize(x);
    
    // Update the shock graph
    shock_model->update_shock_graph(x);
    
    // clean up
    if (shock_model)
      delete shock_model;
    
  }
  else if (param_type == 3 || param_type == 4)
  {
    vcl_cout << "Snake on active edge";
    
    dbsksp_shock_edge_sptr e = sksp_storage->active_edge();
    dbsksp_shock_node_sptr v = sksp_storage->active_node();

    if (!e || !v) 
    {
      vcl_cerr << "ERROR: No active edge or node found. " << vcl_endl;
      return false;
    }

    if (v != e->source() && v != e->target()) 
    {
      vcl_cerr << "ERROR: Active edge is not incident to active node.\n";
      return false;
    }
    
    if (!e->fragment()) 
    {
      e->form_fragment();
    }

    dbsksp_shapelet_sptr shapelet = e->fragment()->get_shapelet();
    if (v==e->target())
    {
      shapelet = shapelet->reversed_dir();
    }

    if (param_type == 3)
    {
    
      // shock snake cost function
      dbsksp_half_shapelet_cost_function shapelet_cost(e, v, image_cost, lambda);

      
      // initial parameter !!!!!!
      vnl_vector<double > x = shapelet_cost.get_free_params(e, v);
      
      // non-linear minimizer
      vnl_powell powell_minimizer(&shapelet_cost);

      // minimizer configuration
      powell_minimizer.set_max_function_evals(max_evals);
      powell_minimizer.set_trace(true);
      powell_minimizer.set_verbose(true);
      powell_minimizer.set_initial_step(init_step);
      powell_minimizer.set_linmin_xtol(linmin_xtol);


      ///////////////////////////////////////////////////
      powell_minimizer.minimize(x);
      ///////////////////////////////////////////////////
      
      
      vcl_cout << "Start error = " << powell_minimizer.get_start_error() << vcl_endl;
      vcl_cout << "End error = " << powell_minimizer.get_end_error() << vcl_endl;
      vcl_cout << "Failure code = " << powell_minimizer.get_failure_code() << vcl_endl;

      // update the shape with the new shapelet params
      sksp_storage->set_active_shapelet(shapelet_cost.get_shapelet(x));

      e->fragment()->update_edge_with_shapelet(shapelet_cost.get_shapelet(x));
      shock_graph->compute_all_dependent_params();
      
      return true;
    }
    else
    {
      // optimize over different values of L

      // shock snake cost function
      dbsksp_half_shapelet_cost_function shapelet_cost(e, v, image_cost, lambda);

      double init_length = shapelet_cost.chord_length();

      vnl_vector<double > optimized_params;
      double optimized_length;
      double min_cost = vnl_numeric_traits<double >::maxval;

      // optimize over 6 values of L
      for (double t = 0.5; t <= 1; t = t + 0.1)
      {
        double chord_length = init_length * t;
        shapelet_cost.set_chord_length(chord_length);
        
        // initial parameter !!!!!!
        vnl_vector<double > x = shapelet_cost.get_free_params(e, v);
        
        // non-linear minimizer
        vnl_powell powell_minimizer(&shapelet_cost);

        // minimizer configuration
        powell_minimizer.set_max_function_evals(max_evals);
        powell_minimizer.set_trace(true);
        powell_minimizer.set_verbose(true);
        powell_minimizer.set_initial_step(init_step);
        powell_minimizer.set_linmin_xtol(linmin_xtol);


        ///////////////////////////////////////////////////
        powell_minimizer.minimize(x);
        ///////////////////////////////////////////////////
        
        
        vcl_cout << "Start error = " << powell_minimizer.get_start_error() << vcl_endl;
        vcl_cout << "End error = " << powell_minimizer.get_end_error() << vcl_endl;
        vcl_cout << "Failure code = " << powell_minimizer.get_failure_code() << vcl_endl;

        dbsksp_shapelet_sptr sub_optim_shapelet = shapelet_cost.get_shapelet(x);
        // compute boundary length of this shapelet
        double total_length = 0;
        for (int i=0; i<2; ++i)
        {
          total_length += sub_optim_shapelet->bnd_arc(i).length();        
        }

        //double sub_optim_cost = powell_minimizer.get_end_error() / (total_length * vcl_sqrt(t));
        double sub_optim_cost = powell_minimizer.get_end_error() / (total_length);

        if ( sub_optim_cost < min_cost)
        {
          min_cost = powell_minimizer.get_end_error();
          optimized_params = x;
          optimized_length = chord_length;
        }
      }     
      shapelet_cost.set_chord_length(optimized_length);
      dbsksp_shapelet_sptr optimzed_shapelet = 
        shapelet_cost.get_shapelet(optimized_params);

      
      
      // update the shape with the new shapelet params
      sksp_storage->set_active_shapelet(optimzed_shapelet);

      e->fragment()->update_edge_with_shapelet(optimzed_shapelet);
      shock_graph->compute_all_dependent_params();

      return true;
    }
  }
  else if (param_type == 5)
  {
    vcl_cout << "Snake on two shapelet";
    
    dbsksp_shock_edge_sptr e0 = sksp_storage->edge0();
    dbsksp_shock_edge_sptr e1 = sksp_storage->edge1();
    
    dbsksp_shock_node_sptr v0 = 0;
    dbsksp_twoshapelet_sptr twoshapelet = this->form_twoshapelet(e0, e1, v0);
    if (twoshapelet == 0)
    {
      vcl_cout << "ERROR: Could not form two shapelet from the two edges;";
      return false;
    }

    // shock snake cost function
    dbsksp_twoshapelet_cost_function twoshapelet_cost(twoshapelet, image_cost, lambda);

    
    // initial parameter !!!!!!
    vnl_vector<double > x_init = twoshapelet_cost.get_free_params(twoshapelet);
    
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
    for (int i=0;i<n_samples;++i)
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

    // non-linear minimizer
    vnl_amoeba amoeba_minimizer(twoshapelet_cost);

    // minimizer configuration
    amoeba_minimizer.set_max_iterations(max_evals); //set_max_function_evals(max_evals);
    amoeba_minimizer.verbose = true; //set_verbose(true);
    amoeba_minimizer.set_relative_diameter(init_step); //set_initial_step(init_step);
    amoeba_minimizer.set_f_tolerance (linmin_xtol); //set_linmin_xtol(linmin_xtol);


    ///////////////////////////////////////////////////
    // amoeba_minimizer.minimize(x);
    ///////////////////////////////////////////////////

    // update the shape with the new twoshapelet params
    vcl_cout << "Min cost = " << twoshapelet_cost.f(x) << vcl_endl;

    dbsksp_twoshapelet_sptr final = twoshapelet_cost.get_twoshapelet(x);

    e0->fragment()->update_edge_with_shapelet(final->shapelet_start(), v0);
    e1->fragment()->update_edge_with_shapelet(final->shapelet_end(), e0->opposite(v0));
    shock_graph->compute_all_dependent_params();

    sksp_storage->clear_shapelet_list();
    sksp_storage->add_shapelet(final->shapelet_start());
    sksp_storage->add_shapelet(final->shapelet_end());
    
    return true;
  }


  else
  {
    vcl_cout << "Unknown option";
    return false;
  }

  //// create the output storage class
  //vidpro1_vsol2D_storage_sptr vsol_storage = vidpro1_vsol2D_storage_new();
  //output_data_[0].push_back(vsol_storage);
  //vsol_storage->add_objects(shock_model->shock_graph()->trace_boundary(), "shock boundary");

  if (image_cost)
  {
    delete image_cost;
  }

  return true;
}


// ----------------------------------------------------------------------------
bool dbsksp_shock_snake_process::
finish()
{
  return true;
}



dbsksp_twoshapelet_sptr dbsksp_shock_snake_process::
form_twoshapelet(const dbsksp_shock_edge_sptr& e0,
                 const dbsksp_shock_edge_sptr& e1,
                 dbsksp_shock_node_sptr& node_start)
{
  node_start = 0;
  if (e0 == e1) return 0;


  // determine starting node
  node_start = e0->source();
  if (node_start==e1->source() || node_start==e1->target())
  {
    node_start = e0->target();
  }

  // determine middle node
  dbsksp_shock_node_sptr mid_node = e0->opposite(node_start);
  if ((mid_node != e1->source()) && (mid_node != e1->target()))
    return 0;
  
  // start shapelet
  dbsksp_shapelet_sptr start = e0->fragment()->get_shapelet();
  if (node_start != e0->source())
    start = start->reversed_dir();

  // end shapelet
  dbsksp_shapelet_sptr end = e1->fragment()->get_shapelet();
  if (mid_node != e1->source())
  {
    end = end->reversed_dir();
  }

  // form twoshapelet from the two shapelets
  dbsksp_twoshapelet_sptr s = new dbsksp_twoshapelet(
    start->x0(), start->y0(), start->theta0(), start->r0(), start->phi0(), start->m0(), start->len(),
    end->phi0(), end->m0(), end->len(), end->phi_end());
  

  return s;
}







////:Some generic return codes that apply to all minimizers.
//00113   enum ReturnCodes {
//00114     ERROR_FAILURE               =-1,
//00115     ERROR_DODGY_INPUT           = 0,
//00116     CONVERGED_FTOL              = 1,
//00117     CONVERGED_XTOL              = 2,
//00118     CONVERGED_XFTOL             = 3,
//00119     CONVERGED_GTOL              = 4,
//00120     FAILED_TOO_MANY_ITERATIONS  = 5,
//00121     FAILED_FTOL_TOO_SMALL       = 6,
//00122     FAILED_XTOL_TOO_SMALL       = 7,
//00123     FAILED_GTOL_TOO_SMALL       = 8,
//00124     FAILED_USER_REQUEST         = 9
//00125   };

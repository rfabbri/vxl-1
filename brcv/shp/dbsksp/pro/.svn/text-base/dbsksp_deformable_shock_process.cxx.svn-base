// This is dbsksp/pro/dbsksp_deformable_shock_process.cxx

//:
// \file

#include "dbsksp_deformable_shock_process.h"
#include <vnl/algo/vnl_powell.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vnl/vnl_math.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vsol/vsol_point_2d.h>



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
#include <dbsksp/algo/dbsksp_shock_snake_cost_function.h>

#include <dbsksp/algo/dbsksp_shock_snake.h>

//: Constructor
dbsksp_deformable_shock_process::
dbsksp_deformable_shock_process()
{
  this->deform_type_descriptions_.push_back("Two active edges");
  this->deform_type_descriptions_.push_back(
    "Insert two edges at terminal node and deform");

  vcl_vector<vcl_string > energy_type_choices;
  energy_type_choices.push_back("Distance Transform");
  energy_type_choices.push_back("Negative Normalized Gradient");

  if( 
    !parameters()->add("Type of deformation: " , "-deform_type" , this->deform_type_descriptions_, 0) ||
    !parameters()->add("Type of energy: " , "-energy_type" , energy_type_choices, 0) ||
    !parameters()->add("chord0 = " , "-chord0" , 10.0f ) ||
    !parameters()->add("chord1 = " , "-chord1" , 10.0f ) ||
    !parameters()->add("num_times = " , "-num_times" , (int)1 ) ||
    !parameters()->add("Evolve till reaching target_point?" , "-till_target" , false) ||
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
dbsksp_deformable_shock_process::
~dbsksp_deformable_shock_process()
{
}


//: Clone the process
bpro1_process* dbsksp_deformable_shock_process::
clone() const
{
  return new dbsksp_deformable_shock_process(*this);
}

//: Returns the name of this process
vcl_string dbsksp_deformable_shock_process::
name()
{ 
  return "Deformable Shock"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_deformable_shock_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbsksp_shock" );
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_deformable_shock_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back("vsol2D");
  return to_return;
}

//: Return the number of input frames for this process
int dbsksp_deformable_shock_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsksp_deformable_shock_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_deformable_shock_process::
execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << "In dbsksp_deformable_shock_process::execute() - "
             << "not exactly one input images" << vcl_endl;
    return false;
  }

  
  

  // ------------- USER PARAMS --------------------------------------
  unsigned int deform_type = 100;
  parameters()->get_value( "-deform_type" , deform_type);


  unsigned int energy_type = 10;
  parameters()->get_value( "-energy_type" , energy_type);

  float chord0 = -1.0f;
  parameters()->get_value( "-chord0" , chord0);

  float chord1 = -1.0f;
  parameters()->get_value( "-chord1" , chord1);

  int num_times = -1;
  parameters()->get_value( "-num_times" , num_times);

  
  bool till_target = false;
  parameters()->get_value( "-till_target" , till_target);

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
  vil_image_view<float > image_view;
  if (image_resource->nplanes()==1)
  {
    image_view = *vil_convert_cast(float(), image_resource->get_view());
  }
  else
  {
    image_view = *vil_convert_cast(float(), 
    vil_convert_to_grey_using_rgb_weighting(image_resource->get_view()));
  }

  // ------------- PROCESS DATA --------------------------------------

  // >> the shock graph
  dbsksp_shock_graph_sptr shock_graph = sksp_storage->shock_graph();

  // >> external energy
  dbsksp_external_energy_function* image_cost = 0;
  
  if (energy_type == 0) // distance transform
  {
    vil_image_view<unsigned int > image_unsigned(image_view.ni(), image_view.nj(), 0);
    for (unsigned int i=0; i<image_view.ni(); ++i)
    for (unsigned int j=0; j<image_view.nj(); ++j)
      image_unsigned(i, j) = (image_view(i, j) < 128) ? 0 : 255;

    // Compute distance transform of the image
    vil_image_view<float> dt_image;
    bil_edt_signed(image_unsigned, dt_image);

    // the potential energy function based on dt image
    dbsksp_potential_integrator* dt_cost = new dbsksp_potential_integrator();
    dt_cost->set_potential_field(dt_image);
    dt_cost->set_sampling_length(ds);

    // set the image cost
    image_cost = dt_cost;
  }
  else // Normalized inverse Gradient magnitude
  {
    
    vil_image_view<float> grad_mag_cost;
    double sigma = 0.5;
    
    bil_negative_normalized_gradient_magnitude(image_view, sigma, 0.2, grad_mag_cost);

    // --------------------------------------------------------
    // the potential energy function based on gradient image
    dbsksp_potential_integrator* grad_cost = 
      new dbsksp_potential_integrator();
    grad_cost->set_potential_field(grad_mag_cost);
    grad_cost->set_sampling_length(ds);

    // set the image cost
    image_cost = grad_cost;
  }

  // >> Deform the shock graph
  if (this->deform_type_descriptions_[deform_type] == "Two active edges")
  {
    vcl_cout << "Snake on two shapelet";
   
    dbsksp_shock_snake snake;
    snake.set_shock_graph(sksp_storage->shock_graph());
    snake.set_ext_cost_integrator(image_cost);

    dbsksp_twoshapelet_sptr final;

    int error_code = snake.optimize_two_active_edges(sksp_storage->active_node(), 
      sksp_storage->active_edge(), final);

    if (error_code == 0)
    {
      vcl_cerr << "ERROR running the process.\n";
      return false;
    }
    shock_graph->compute_all_dependent_params();

    sksp_storage->clear_shapelet_list();
    sksp_storage->add_shapelet(final->shapelet_start());
    sksp_storage->add_shapelet(final->shapelet_end());
    
    return true;
  }
  else if (this->deform_type_descriptions_[deform_type] ==
    "Insert two edges at terminal node and deform")
  {
    // Parameter check-up
    if (!sksp_storage->active_edge()->is_terminal_edge())
    {
      vcl_cerr << "ERROR: the active edge needs to be a terminal edge "
        << "to run this process.\n";
      return false;
    }

    if (chord0 <=0 || chord1 <= 0)
    {
      vcl_cerr << "ERROR: the chord lengths need to be strictly positive"
        << "to run this process.\n";
      return false;
    }

    if (num_times <= 0)
    {
      vcl_cerr << "ERROR: num_times needs to be strictly positive"
        << "to run this process.\n";
      return false;
    }

    dbsksp_shock_snake snake;
    snake.set_shock_graph(sksp_storage->shock_graph());
    snake.set_ext_cost_integrator(image_cost);

    dbsksp_twoshapelet_sptr end_twoshapelet;

    int error_code = 0;
    if ( !(till_target && sksp_storage->target_point()) )
    {
      dbsksp_shock_edge_sptr terminal_edge = sksp_storage->active_edge();
      error_code = snake.extend_and_deform_2_edges_n_times(
      terminal_edge, 
      chord0, chord1, num_times, end_twoshapelet);
    }
    else
    {
      dbsksp_shock_edge_sptr terminal_edge = sksp_storage->active_edge();
      error_code = snake.extend_and_deform_2_edges_till_reach_target(
        terminal_edge, chord0, chord1,
        sksp_storage->target_point()->get_p(), num_times, end_twoshapelet);
    }

    if (error_code <= 0)
    {
      vcl_cerr << "ERROR running the process.\n";
      return false;
    }
    shock_graph->compute_all_dependent_params();

    sksp_storage->clear_shapelet_list();
    sksp_storage->add_shapelet(end_twoshapelet->shapelet_start());
    sksp_storage->add_shapelet(end_twoshapelet->shapelet_end());
    
    return true;
  }
  else
  {
    vcl_cout << "Unknown option";
    return false;
  }

  // >> Clean up
  if (image_cost) delete image_cost;

  // >> Output the traced boundary of the new contour
  vidpro1_vsol2D_storage_sptr vsol_storage = vidpro1_vsol2D_storage_new();
  vsol_storage->add_objects(shock_graph->trace_boundary(), "shock boundary");
  output_data_[0].push_back(vsol_storage);

  return true;
}


// ----------------------------------------------------------------------------
bool dbsksp_deformable_shock_process::
finish()
{
  return true;
}


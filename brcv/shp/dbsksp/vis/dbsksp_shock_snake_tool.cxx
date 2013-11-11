// This is shp/dbsksp/dbsksp_shock_snake_tool.cxx
//:
// \file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date June 28, 2006

#include "dbsksp_shock_snake_tool.h"

#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>

#include <vnl/vnl_math.h>

#include <dbsksp/vis/dbsksp_soview_shock.h>
#include <dbsksp/dbsksp_shock_edge.h>
#include <dbsksp/dbsksp_shock_fragment.h>
#include <dbsksp/dbsksp_shock_node.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shapelet.h>

#include <vsol/vsol_point_2d.h>


#include <vgui/vgui_dialog.h>
#include <vidpro1/vidpro1_repository.h>
#include <bgui/bgui_image_tableau.h>
#include <bvis1/bvis1_manager.h>


#include <vnl/algo/vnl_powell.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vnl/vnl_math.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vsol/vsol_point_2d.h>



//#include <bpro1/bpro1_parameters.h>
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
dbsksp_shock_snake_tool::
dbsksp_shock_snake_tool() : 
  tableau_(0), 
  shock_storage_(0),
  image_storage_(0),
  lambda_(0.1f),
  chord_length_(20.0),
  num_times_(1),
  max_width_(-1),
  min_width_(-1)
{
  
}


//: Destructor
dbsksp_shock_snake_tool::
~dbsksp_shock_snake_tool()
{
}


//: set the tableau to work with
bool dbsksp_shock_snake_tool::
set_tableau ( const vgui_tableau_sptr& tableau ){
  if(!tableau)
    return false;
  if( tableau->type_name() == "dbsksp_shock_tableau" ){
    if( !this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)) )
      return false;
    this->tableau_.vertical_cast(tableau);
    return true;
  }
  tableau_ = 0;
  return false;
}


//: Set the storage class for the active tableau
bool dbsksp_shock_snake_tool::
set_storage ( const bpro1_storage_sptr& storage_sptr){
  if (!storage_sptr)
    return false;
  
  //make sure its a vsol storage class
  if (storage_sptr->type() == "dbsksp_shock"){
    this->shock_storage_.vertical_cast(storage_sptr);
    return true;
  }
  this->shock_storage_ = 0;
  return false;
}


// ----------------------------------------------------------------------------
//: Return the name of this tool
vcl_string dbsksp_shock_snake_tool::
name() const {
  return "Shock Snake";
}


// ----------------------------------------------------------------------------
//: Return the storage of the tool
dbsksp_shock_storage_sptr dbsksp_shock_snake_tool::
shock_storage()
{
  return this->shock_storage_;
}



// ----------------------------------------------------------------------------
//: Return the tableau of the tool
dbsksp_shock_tableau_sptr dbsksp_shock_snake_tool::
tableau(){
  return this->tableau_;
}


//: Set the image storage
bool dbsksp_shock_snake_tool::
set_image_storage ( const bpro1_storage_sptr& image_storage)
{
  if (!image_storage)
    return false;
  
  //make sure its a vsol storage class
  if (image_storage->type() == "image")
  {
    this->image_storage_.vertical_cast(image_storage);
    return true;
  }
  this->image_storage_.vertical_cast(0);
  return false;
}
  


//: Allow the tool to add to the popup menu as a tableau would
void dbsksp_shock_snake_tool::
get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  menu.add("Choose image to segment", 
    new dbsksp_shock_snake_choose_image_command(this)); 
  
  menu.add("Change tool parameters", 
    new dbsksp_shock_snake_change_params_command(this)); 
  
  menu.add("Fit two shock edges", 
    new dbsksp_fit_two_edges_command(this));  

  menu.add("Add one edge and deform", 
    new dbsksp_add_one_edge_and_deform_command(this));  


}

// ----------------------------------------------------------------------------
//: Handle events
bool dbsksp_shock_snake_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{  
  return false;
}







// ============================================================================
// dbsksp_shock_snake_choose_image_command
// ============================================================================

void dbsksp_shock_snake_choose_image_command::
execute()
{

  //Prompt the user to select input/output variable
  vgui_dialog io_dialog("Select image to segment" );

  //display input options
  vcl_vector< vcl_string > input_type_list;
  input_type_list.push_back("image");
  io_dialog.message("Select Input(s) From Available ones:");

  //store the choices
  vcl_vector<int> input_choices(input_type_list.size());
  vcl_vector< vcl_vector <vcl_string> > available_storage_classes(input_type_list.size());
  vcl_vector< vcl_string > input_names(input_type_list.size());
  
  //get the repository and extract the qualified ones
  vidpro1_repository_sptr repository_sptr = bvis1_manager::instance()->repository();
  for( unsigned int i = 0; i < input_type_list.size(); ++i )
  {
    //for this input type allow user to select from available storage classes in the repository
    available_storage_classes[i] = repository_sptr->get_all_storage_class_names(input_type_list[i]);

    //Multiple choice - with the list of options.
    io_dialog.choice(input_type_list[i].c_str(), available_storage_classes[i], input_choices[i]); 
  }

  //display dialog
  io_dialog.ask();

  // get the names of the user-select image storage
  vcl_string image_name = available_storage_classes[0].at(input_choices[0]);

  // get input storage class
  bpro1_storage_sptr storage = repository_sptr->get_data_by_name(image_name);
  this->tool()->set_image_storage(storage);

  return;
}




// ============================================================================
// dbsksp_shock_snake_set_params_command
// ============================================================================

void dbsksp_shock_snake_change_params_command::
execute()
{
  vgui_dialog dialog("dLivewire");
  dialog.field("Weight shape prior", this->tool()->lambda_);
  dialog.field("Total chord length of fragment pair", this->tool()->chord_length_);
  dialog.field("Number of times: ", this->tool()->num_times_);
  dialog.field("Hard constraint on the maximum width (-1 for no constraint): ",
    this->tool()->max_width_ );

  dialog.field("Hard constraint on the minimum width (-1 for no constraint): ",
    this->tool()->min_width_ );


  bool ok = dialog.ask();
  return;
}



// ============================================================================
// dbsksp_fit_two_edges_command
// ============================================================================

// ----------------------------------------------------------------------------
void dbsksp_fit_two_edges_command::
execute()
{

  vcl_vector<vcl_string > deform_type_descriptions_;
  deform_type_descriptions_.push_back("Two active edges");
  deform_type_descriptions_.push_back(
    "Insert two edges at terminal node and deform");

  vcl_vector<vcl_string > energy_type_choices;
  energy_type_choices.push_back("Negative Normalized Gradient");




  // ------------- USER PARAMS --------------------------------------
  unsigned int deform_type = 0;
  unsigned int energy_type = 0;



  float sampling_length = 1.0f; // ds


  // STORAGE CLASSES --------------------------------------

  // get the shock graph from storage classes
  dbsksp_shock_storage_sptr sksp_storage = this->tool()->shock_storage();

  // get the image from the storage class and convert to gray
  vidpro1_image_storage_sptr image_storage = this->tool()->image_storage();

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
  dbsksp_potential_integrator* image_cost = 0;
  vil_image_view<float > potential_image;

  if (energy_type_choices[energy_type] == "Negative Normalized Gradient")
  {  
    vil_image_view<float> grad_mag_cost;
    double sigma = 0.5;
    
    bil_normalized_inverse_gradient(image_view, sigma, grad_mag_cost, "quadratic");
    potential_image = grad_mag_cost;

    // --------------------------------------------------------
    // the potential energy function based on gradient image
    dbsksp_potential_integrator* grad_cost = 
      new dbsksp_potential_integrator();
    grad_cost->set_potential_field(grad_mag_cost);
    grad_cost->set_sampling_length(sampling_length);

    // set the image cost
    image_cost = grad_cost;
  }

  // >> Deform the shock graph
  if (deform_type_descriptions_[deform_type] == "Two active edges")
  {
    vcl_cout << "Snake on two shapelet \n";

    if (!this->tool()->image_storage())
    {
      vcl_cout << "ERROR: no image to segment.\n";
    }

    // set up the shock snake
    dbsksp_shock_snake snake;
    snake.set_shock_graph(shock_graph);
    snake.set_potential_image(potential_image);
    snake.set_lambda(this->tool()->lambda_);
    // HACK ----------------------------------
    snake.max_width_ = this->tool()->max_width_;
    snake.min_width_ = this->tool()->min_width_;
    


    dbsksp_twoshapelet_sptr final;

    int error_code = snake.optimize_two_active_edges(sksp_storage->active_node(), 
      sksp_storage->active_edge(), final);

    if (error_code == 0)
    {
      vcl_cerr << "ERROR running the process.\n";
      return;
    }

    shock_graph->compute_all_dependent_params();

    sksp_storage->clear_shapelet_list();
    sksp_storage->add_shapelet(final->shapelet_start());
    sksp_storage->add_shapelet(final->shapelet_end());
    return;
  }
  else
  {
    vcl_cout << "Unknown option";
    return;
  }

  // >> Clean up
  if (image_cost) delete image_cost;

  bvis1_manager::instance()->display_current_frame();


  return;
}












// ============================================================================
// dbsksp_extend_and_fit_two_edges_command
// ============================================================================



void dbsksp_add_one_edge_and_deform_command::
execute()
{
  vcl_vector<vcl_string > energy_type_choices;
  energy_type_choices.push_back("Negative Normalized Gradient");

  // ------------- USER PARAMS --------------------------------------
  unsigned int energy_type = 0;
  bool till_target = false;
  float sampling_length = 1.0f; // ds

  // STORAGE CLASSES --------------------------------------

  // get the shock graph from storage classes
  dbsksp_shock_storage_sptr sksp_storage = this->tool()->shock_storage();

  // get the image from the storage class and convert to gray
  vidpro1_image_storage_sptr image_storage = this->tool()->image_storage();

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
  vil_image_view<float > potential_image;

  if (energy_type_choices[energy_type] == "Negative Normalized Gradient")
  {  
    vil_image_view<float> grad_mag_cost;
    double sigma = 0.5;
    
    bil_normalized_inverse_gradient(image_view, sigma, grad_mag_cost, "quadratic");
    potential_image = grad_mag_cost;
  }

  // >> Deform the shock graph
  vcl_cout << "Extend and fit pairs of shapelets \n";

  if (!this->tool()->image_storage())
  {
    vcl_cout << "ERROR: no image to segment.\n";
  }

  // parameter check up
  if (this->tool()->chord_length_ <= 0)
  {
    vcl_cerr << "ERROR: the chord lengths need to be strictly positive"
      << "to run this process.\n";
    return;
  }

  if (this->tool()->num_times_ <= 0)
  {
    vcl_cerr << "ERROR: num_times needs to be strictly positive"
      << "to run this process.\n";
    return;
  }

  // set up the shock snake
  dbsksp_shock_snake snake;
  snake.set_shock_graph(shock_graph);
  snake.set_potential_image(potential_image);
  snake.set_lambda(this->tool()->lambda_);

  // HACK ----------------------------------
  snake.max_width_ = this->tool()->max_width_;
  snake.min_width_ = this->tool()->min_width_;


  
  // data to pass to the snake to deform
  dbsksp_shock_edge_sptr source_edge = sksp_storage->active_edge();
  dbsksp_shock_node_sptr source_node = sksp_storage->active_node();
  dbsksp_twoshapelet_sptr end_twoshapelet;

  double chord_length = this->tool()->chord_length_;

  for (unsigned i=0; i<this->tool()->num_times_; ++i)
  {
    ///////////////////////////////////////////////////////////////////////
    int error_code = snake.add_one_edge_and_deform(source_node, source_edge,
      chord_length, end_twoshapelet);
    ///////////////////////////////////////////////////////////////////////
    if (error_code <= 0)
    {
      vcl_cerr << "ERROR running the process.\n";
      return;
    }
    source_node = source_edge->opposite(source_node);
    source_edge = shock_graph->cyclic_adj_succ(source_edge, source_node);
  }
  shock_graph->compute_all_dependent_params();
  sksp_storage->set_active_edge(source_edge);

  sksp_storage->clear_shapelet_list();
  sksp_storage->add_shapelet(end_twoshapelet->shapelet_start());
  sksp_storage->add_shapelet(end_twoshapelet->shapelet_end());
  
  
  bvis1_manager::instance()->display_current_frame();
  return;
}

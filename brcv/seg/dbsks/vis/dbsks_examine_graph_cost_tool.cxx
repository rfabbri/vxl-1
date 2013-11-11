// This is shp/dbsksp/dbsks_examine_graph_cost_tool.cxx
//:
// \file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Dec 20, 2007

#include "dbsks_examine_graph_cost_tool.h"

#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_dialog.h>

#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <dbsksp/vis/dbsksp_soview_shock.h>
#include <dbsksp/dbsksp_shock_edge.h>
#include <dbsksp/dbsksp_shock_fragment.h>
#include <dbsksp/dbsksp_shock_graph.h>

#include <dbsks/dbsks_dp_match.h>
#include <dbsks/pro/dbsks_shapematch_storage.h>

#include <vul/vul_sprintf.h>



// -----------------------------------------------------------------------------
//: Constructor
dbsks_examine_graph_cost_tool::
dbsks_examine_graph_cost_tool() : 
  tableau_(0), 
  shock_storage_(0),
  image_storage_(0)
{
  this->gesture_display_fragment_cost_ = 
    vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  this->gesture_display_graph_cost_ = 
    vgui_event_condition(vgui_key('g'), vgui_MODIFIER_NULL,true);


  this->gesture_increase_ref_x_ = vgui_event_condition(vgui_key('i'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_ref_x_ = vgui_event_condition(vgui_key('i'), vgui_SHIFT, true);

  this->gesture_increase_ref_y_ = vgui_event_condition(vgui_key('j'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_ref_y_ = vgui_event_condition(vgui_key('j'), vgui_SHIFT, true);

  this->gesture_increase_ref_psi_ = vgui_event_condition(vgui_key('t'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_ref_psi_ = vgui_event_condition(vgui_key('t'), vgui_SHIFT, true);

  
}

// -----------------------------------------------------------------------------
//: Destructor
dbsks_examine_graph_cost_tool::
~dbsks_examine_graph_cost_tool()
{
}


// -----------------------------------------------------------------------------
//: set the tableau to work with
bool dbsks_examine_graph_cost_tool::
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



// -----------------------------------------------------------------------------
//: Set the storage class for the active tableau
bool dbsks_examine_graph_cost_tool::
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
vcl_string dbsks_examine_graph_cost_tool::
name() const {
  return "Examine graph cost";
}


// ----------------------------------------------------------------------------
//: Return the storage of the tool
dbsksp_shock_storage_sptr dbsks_examine_graph_cost_tool::
shock_storage()
{
  return this->shock_storage_;
}



// ----------------------------------------------------------------------------
//: Return the tableau of the tool
dbsksp_shock_tableau_sptr dbsks_examine_graph_cost_tool::
tableau(){
  return this->tableau_;
}



// -----------------------------------------------------------------------------
//: Set the image storage
bool dbsks_examine_graph_cost_tool::
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
  






//: Set shapematch storage
bool dbsks_examine_graph_cost_tool::
set_shapematch_storage(const bpro1_storage_sptr& shapematch_storage)
{
  if (!shapematch_storage)
    return false;
  
  //make sure its a vsol storage class
  if (shapematch_storage->type() == "dbsks_shapematch")
  {
    this->shapematch_storage_.vertical_cast(shapematch_storage);
    return true;
  }
  this->shapematch_storage_.vertical_cast(0);
  return false;
}



//: Retrieve the DP shape matcher from shapematch storage
dbsks_dp_match_sptr dbsks_examine_graph_cost_tool::
dp_engine() const
{
  return this->shapematch_storage()->dp_engine();
}





// -----------------------------------------------------------------------------
//: Allow the tool to add to the popup menu as a tableau would
void dbsks_examine_graph_cost_tool::
get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  //
  menu.add("Choose image and shapematcher to segment", 
    new dbsks_egc_choose_image_and_shapematch_command(this)); 
}

// ----------------------------------------------------------------------------
//: Handle events
bool dbsks_examine_graph_cost_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{


  // gestures to move the graph around
  if (this->gesture_increase_ref_x_(e))
  {
    return this->handle_change_ref_origin(1, 0);
  }

  if (this->gesture_decrease_ref_x_(e))
  {
    return this->handle_change_ref_origin(-1, 0);
  }

  if (this->gesture_increase_ref_y_(e))
  {
    return this->handle_change_ref_origin(0, 1);
  }

  if (this->gesture_decrease_ref_y_(e))
  {
    return this->handle_change_ref_origin(0, -1);
  }

  if (this->gesture_increase_ref_psi_(e))
  {
    return this->handle_change_ref_dir(0.02);
  }

  if (this->gesture_decrease_ref_psi_(e))
  {
    return this->handle_change_ref_dir(-0.02);
  }

  // gestures to display costs
  if (this->gesture_display_fragment_cost_(e))
  {
    if (!this->check_necessary_vars_available())
      return false;

    return this->handle_display_fragment_cost();
  }

  if (this->gesture_display_graph_cost_(e))
  {
    if (!this->check_necessary_vars_available())
      return false;
    return this->handle_display_graph_cost();
  }

  return false;
}








// ----------------------------------------------------------------------------
//:
bool dbsks_examine_graph_cost_tool::
handle_change_ref_origin(double dx, double dy)
{
  dbsksp_shock_graph_sptr shock_graph(this->tableau()->shock_graph());
  vgl_point_2d<double > pt = shock_graph->ref_origin();
  shock_graph->set_ref_origin(vgl_point_2d<double >(pt.x()+dx, pt.y() + dy));
  
  shock_graph->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return false;
}




// ----------------------------------------------------------------------------
//:
bool dbsks_examine_graph_cost_tool::
handle_change_ref_dir(double rot_angle)
{
  dbsksp_shock_graph_sptr shock_graph(this->tableau()->shock_graph());
  vgl_vector_2d<double > v = shock_graph->ref_direction();
  shock_graph->set_ref_direction(rotated(v, rot_angle));

  shock_graph->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return false;
}



// -----------------------------------------------------------------------------
//: Display cost of the clicked component (boundary arc or fragment)
bool dbsks_examine_graph_cost_tool::
handle_display_fragment_cost()
{
  unsigned int highlighted_id = this->tableau()->get_highlighted();
  if (highlighted_id)
  {
    vgui_soview* so = vgui_soview::id_to_object(highlighted_id);
    if (so->type_name() == "dbsksp_soview_shock_edge_chord")
    {
      dbsksp_soview_shock_edge_chord* chord = 
        static_cast<dbsksp_soview_shock_edge_chord* >(so);

      // Edge of the hypothesized graph
      dbsksp_shock_edge_sptr edge_h = chord->edge(); 

      

      // >> Retrieve edge id
      unsigned edge_id = edge_h->id();

      // Shapelet of the test edge
      dbsksp_shapelet_sptr frag_h = edge_h->fragment()->get_shapelet();
      if (this->dp_engine()->need_reverse_frag_dir(edge_id))
        frag_h = frag_h->reversed_dir();

      // Retrieve the corresponding edge in the model graph
      dbsksp_shock_edge_sptr edge_m = this->dp_engine()->graph()->edge_from_id(edge_id);
      
      // Compute cost of the fragments
      float total_cost, deform_cost, image_cost, frag_size;

      this->dp_engine()->compute_frag_cost(edge_id, frag_h,
        total_cost, false, false, &deform_cost, &image_cost, &frag_size);

      vcl_cout << 
        vul_sprintf("\nEdge Id[ %d ]    Total    Deform      Image     FSize\n", edge_id);
      vcl_cout << 
        vul_sprintf("  Real cost: %8.2f  %8.2f   %8.2f  %8.2f \n", 
        total_cost, deform_cost, image_cost, frag_size);

      this->dp_engine()->compute_frag_cost(edge_id, frag_h,
        total_cost, true, false, &deform_cost, &image_cost, &frag_size);

      vcl_cout << 
        vul_sprintf("  Grid cost: %8.2f  %8.2f   %8.2f  %8.2f \n", 
        total_cost, deform_cost, image_cost, frag_size);

      return true;
    }    
  }
  return true;
}


// -----------------------------------------------------------------------------
//: Display the total cost of a graph
bool dbsks_examine_graph_cost_tool::
handle_display_graph_cost()
{
  // Display cost between two graphs
  float real_cost = this->dp_engine()->compute_graph_cost(
        this->shock_storage()->shock_graph());

  

  // Compute total chord length of the graph
  dbsksp_shock_graph_sptr sg = this->shock_storage()->shock_graph();

  double chord_length = 0;
  for (dbsksp_shock_graph::edge_iterator eit = sg->edges_begin();
    eit != sg->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;

    if (e->is_terminal_edge())
      continue;

    chord_length += e->chord_length();
  }

  // Cost normalized by chord length
  double real_cost_norm = real_cost / chord_length;


  vcl_cout << vul_sprintf("Graph:      real_cost    chord_len norm_real   \n");
  vcl_cout << 
    vul_sprintf("            %8.2f %8.2f %8.2f\n", real_cost, chord_length, real_cost_norm);

  // Display approximated shapelets of the fragments
  

  vcl_map<unsigned int, vgl_point_2d<int > > i_state_map;
  for (dbsksp_shock_graph::edge_iterator eit = sg->edges_begin();
    eit != sg->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;

    if (e->is_terminal_edge())
      continue;

    dbsksp_shapelet_sptr s_e = e->fragment()->get_shapelet();
    if (this->dp_engine()->need_reverse_frag_dir(e->id()))
      s_e = s_e->reversed_dir();
    

    // find approximation of this fragment on the grid
    dbsks_shapelet_grid& grid_e = this->dp_engine()->shapelet_grid(e->id());

    int i_xy, i_plane;
    grid_e.approx_shapelet_on_grid(s_e, i_xy, i_plane);
    vgl_point_2d<int > i_state(i_xy, i_plane);

    // save the approximation
    i_state_map.insert(vcl_make_pair(e->id(), i_state));
  }

  this->shock_storage()->clear_shapelet_list();
  vcl_vector<dbsksp_shapelet_sptr > fragment_list;
  this->dp_engine()->construct_graph(i_state_map, fragment_list);

  for (unsigned i =0; i < fragment_list.size(); ++i)
  {
    this->shock_storage()->add_shapelet(fragment_list[i]);
  }

  bvis1_manager::instance()->display_current_frame();





  return true;
}



//: Check if necessary variables are set
bool dbsks_examine_graph_cost_tool::
check_necessary_vars_available() const
{
  if (!this->shock_storage_ || !this->image_storage_ || !this->shapematch_storage_)
  {
    vcl_cout << "At least one storage has not been set.\n";
    return false;
  }
  return true;
}




// ============================================================================
// dbsks_egc_choose_image_and_shapematch_command
// ============================================================================

void dbsks_egc_choose_image_and_shapematch_command::
execute()
{
  //Prompt the user to select input/output variable
  vgui_dialog io_dialog("Select image to segment" );

  //display input options
  vcl_vector< vcl_string > input_type_list;
  input_type_list.push_back("image");
  input_type_list.push_back("dbsks_shapematch");
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
  bpro1_storage_sptr image_storage = repository_sptr->get_data_by_name(image_name);
  this->tool()->set_image_storage(image_storage);

  // get the name of shapematch storage
  vcl_string shapematch_name = available_storage_classes[1].at(input_choices[1]);
  bpro1_storage_sptr shapematch_storage = repository_sptr->get_data_by_name(shapematch_name);
  this->tool()->set_shapematch_storage(shapematch_storage);
  return;
}


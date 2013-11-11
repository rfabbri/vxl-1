// This is breye1/bvis1/tool/dbmrf_tools.cxx
//:
// \file

#include "dbmrf_bmrf_tools.h"
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include "dbmrf_bmrf_displayer.h"
#include <vcl_iostream.h>
#include <vcl_map.h>
#include <vcl_limits.h>
#include <vcl_fstream.h>
#include <vgui/vgui.h> 
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/internals/vgui_draw_line.h>
#include <dbmrf/pro/dbmrf_bmrf_storage.h> 
#include <dbmrf/pro/dbmrf_curvel_3d_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <bmrf/bmrf_network.h>
#include <bmrf/bmrf_node.h>
#include <bmrf/bmrf_arc.h>
#include <bmrf/bmrf_epi_seg.h>
#include <bmrf/bmrf_epi_seg_compare.h>
#include <bmrf/bmrf_gamma_func.h>
#include <bmrf/bmrf_epi_transform.h>
#include <bmrf/bmrf_curvel_3d.h>
#include <bmrf/bmrf_curve_3d.h>
#include <bmrf/bmrf_epi_point.h>
#include <vgl/vgl_line_2d.h>




static void write_vrml_header(vcl_ofstream& str)
{
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 1 1 1 ]\n"
      << "  groundColor [ 1 1 1 ]\n"
      << "}\n"
      << "PointLight {\n"
      << "  on FALSE\n"
      << "  intensity 1\n"
      << "ambientIntensity 0\n"
      << "color 1 1 1\n"
      << "location 0 0 0\n"
      << "attenuation 1 0 0\n"
      << "radius 100\n"
      << "}\n";
}

static void write_vrml_episeg( vcl_ofstream& str,
                               const bmrf_epi_seg_sptr& seg, 
                               const bmrf_gamma_func_sptr& gamma,
                               double d_scale)
{
  str << "Shape {\n"
      << "  appearance NULL\n"
      << "    geometry IndexedLineSet {\n"
      << "      color NULL\n"
      << "      coord Coordinate{\n"
      << "       point[\n";
  for (int i=0; i<seg->n_pts(); ++i){
    double depth = d_scale / (*gamma)(seg->p(i)->alpha());
    str << seg->p(i)->x() << ' ' << seg->p(i)->y() << ' ' << depth << '\n';
  }
  str << "   ]\n"
      << "  }\n";

  str << "   coordIndex [\n"; 
  for (int i=0; i<seg->n_pts(); ++i)
    str << i << ", ";
  str << " -1\n";
  str << "  ]\n";
  str << " }\n"
      << "}\n";
}

//------------------------------------


//: Constructor - protected
dbmrf_tool::dbmrf_tool()
 :  tableau_(NULL), storage_(NULL)
{
}


//: Set the tableau to work with
bool
dbmrf_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
  if( !this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)) )
    return false;

  if( tableau.ptr() != NULL /*&& tableau->type_name() == "vgui_easy2D_tableau"*/ ){
    tableau_.vertical_cast(tableau);
    return true;
  }

  tableau_ =  vgui_easy2D_tableau_sptr(NULL);
  return false;
}


//: Set the storage class for the active tableau
bool
dbmrf_tool::set_storage ( const bpro1_storage_sptr& storage )
{
  if (!storage.ptr())
    return false;
  //make sure its a bmrf storage class
  if (storage->type() == "bmrf"){
    storage_.vertical_cast(storage);
    return true;
  }
  return false;
}

//----------------------dbmrf_inspector_tool---------------------------


//: Constructor
dbmrf_inspector_tool::dbmrf_inspector_tool()
 : draw_epipolar_line_(true), draw_intensity_(true), draw_neighbors_(true),
   color_by_intensity_(false), print_stats_(false),
   object_(NULL), curr_node_(NULL), last_node_(NULL), 
   last_x_(0.0), last_y_(0.0)
{
  neighbor_style_ = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 3.0);
}


//: Destructor
dbmrf_inspector_tool::~dbmrf_inspector_tool()
{
}


//: Return the name of this tool
vcl_string
dbmrf_inspector_tool::name() const
{
  return "Node Inspector"; 
}


//: prune the network by probability
void
dbmrf_inspector_tool::prune_by_probability()
{
  vgui_dialog prune_dlg("Prune neighbors by probability");
  static double thresh = 0.5;
  static bool rel = true;
  prune_dlg.field("Threshold:", thresh);
  prune_dlg.checkbox("Relative", rel);
  if( !prune_dlg.ask())
    return;

  storage_->network()->prune_by_probability(thresh, rel);
  bvis1_manager::instance()->regenerate_all_tableaux();
  bvis1_manager::instance()->display_current_frame();
}


//: prune the network by gamma
void
dbmrf_inspector_tool::prune_by_gamma()
{
  vgui_dialog prune_dlg("Prune nodes by gamma");
  static double min_gamma = 0.02;
  static double max_gamma = 0.05;
  prune_dlg.field("Min Gamma:", min_gamma);
  prune_dlg.field("Max Gamma", max_gamma);
  if( !prune_dlg.ask())
    return;

  storage_->network()->prune_by_gamma(min_gamma, max_gamma);
  bvis1_manager::instance()->regenerate_all_tableaux();
  bvis1_manager::instance()->display_current_frame();
}


//: prune the network of one directional links
void 
dbmrf_inspector_tool::prune_directed()
{
  storage_->network()->prune_directed();
  bvis1_manager::instance()->regenerate_all_tableaux();
  bvis1_manager::instance()->display_current_frame();
}


//: Print stats about the node 
void 
dbmrf_inspector_tool::print_arc_stats(const bmrf_node_sptr node) const
{
  vcl_cout << "gamma mean = " << node->gamma()->mean() <<vcl_endl;
  vcl_cout << "probability = " << node->probability() <<vcl_endl;
  vcl_cout << "-------------------------------------"<<vcl_endl;
  return;

  
  vcl_map<double, double> results;
  vcl_map<double, double> results2;
  vcl_map<double, double> results3;
  //double prob = curr_node_->probability();
  double sum = 0.0;
  for ( bmrf_node::arc_iterator a_itr = node->begin(); a_itr != node->end(); ++a_itr ){
    double int_var = 0.001; // intensity variance
    results[(*a_itr)->probability()] = vcl_exp(-(*a_itr)->induced_match_error()/2.0 
                                               -(*a_itr)->avg_intensity_error()/(2.0*int_var));
    results2[(*a_itr)->probability()] = (*a_itr)->induced_gamma();
    double exp_gamma = (*a_itr)->to()->gamma()->mean();
    exp_gamma /= 1.0 + exp_gamma*(*a_itr)->time_step();
    results3[(*a_itr)->probability()] = exp_gamma;
    sum += (*a_itr)->probability()*vcl_abs((*a_itr)->induced_gamma() - exp_gamma);
    //results2[(*a_itr)->probability()] = (*a_itr)->avg_intensity_error();
    //results3[(*a_itr)->probability()] = (*a_itr)->induced_match_error();
  }
  for ( vcl_map<double, double>::iterator itr = results.begin(); itr != results.end(); ++itr){
    vcl_cout << itr->first << " \t- " << itr->second 
             << " \t- " << results2[itr->first] << " \t- " << results2[itr->first]-results3[itr->first] << vcl_endl;
  }
  vcl_cout << "sum = " << sum << vcl_endl;
  vcl_cout << "-----------------------------------" << vcl_endl;
}


//: Handle events
bool
dbmrf_inspector_tool::handle( const vgui_event & e, 
                                  const bvis1_view_tableau_sptr& view )
{
  if( !tableau_.ptr() )
    return false;

  bool is_mouse_over = ( bvis1_manager::instance()->active_tableau()
                         == view->selector()->active_tableau() );

  if ( e.type == vgui_DRAW )
    return false;

  double frame = view->frame();

  // Draw neighbors as overlays
  if( e.type == vgui_DRAW_OVERLAY){
    const bmrf_epipole& epipole = storage_->network()->epipole(int(frame));
    if (curr_node_){
      if(draw_neighbors_){
        if(color_by_intensity_){
          const double int_var = 0.001; // intensity variance
          double total_prob = 0.0;
          for ( bmrf_node::arc_iterator a_itr = curr_node_->begin(); 
                a_itr != curr_node_->end();  ++a_itr ){
            if ( (*a_itr)->to()->frame_num() != frame )
              continue;
            double alpha_range = (*a_itr)->max_alpha() - (*a_itr)->min_alpha();
            total_prob += alpha_range * vcl_exp(- (*a_itr)->avg_intensity_error()/(2.0*int_var));
          }
          for ( bmrf_node::arc_iterator a_itr = curr_node_->begin(); 
                a_itr != curr_node_->end();  ++a_itr ){
            if ( (*a_itr)->to()->frame_num() != frame )
              continue;
            double alpha_range = (*a_itr)->max_alpha() - (*a_itr)->min_alpha();
            double prob = alpha_range * vcl_exp(- (*a_itr)->avg_intensity_error()/(2.0*int_var)) / total_prob;
            neighbor_style_->rgba[0] = prob ;
            neighbor_style_->rgba[2] = 1.0;
            neighbor_style_->apply_all();  
            bgui_bmrf_epi_seg_soview2D((*a_itr)->to()->epi_seg()).draw();
          }
        }
        else{
          double prob = curr_node_->probability();
          for ( bmrf_node::arc_iterator a_itr = curr_node_->begin(); 
          a_itr != curr_node_->end();  ++a_itr ){
            if ( (*a_itr)->to()->frame_num() != frame )
              continue;
            
            neighbor_style_->rgba[0] = (*a_itr)->probability() / prob ;
            neighbor_style_->rgba[2] = 1.0;
            neighbor_style_->apply_all();  
            bgui_bmrf_epi_seg_soview2D((*a_itr)->to()->epi_seg()).draw();
          }
        }
      }
      if ( curr_node_->frame_num() == frame ){
        vgui_style::new_style(0.0, 0.0, 1.0, 1.0, 3.0)->apply_all();
        bgui_bmrf_epi_seg_soview2D(curr_node_->epi_seg(),epipole, draw_intensity_).draw();
      }
    }
     
    // Draw the epipolar line through the mouse position
    if(draw_epipolar_line_){
      vgui_style::new_style(1.0f, 1.0f, 1.0f, 5.0f, 1.0f)->apply_all();
      vgl_line_2d<double> epl(epipole.location(), vgl_point_2d<double>(last_x_,last_y_));
      vgui_soview2D_point(epipole.location().x(), epipole.location().y()).draw();
      vgui_draw_line(epl.a(), epl.b(), epl.c());
    }
    return !is_mouse_over;
  }

  // Make VRML for current frame
  if( e.type == vgui_KEY_PRESS && e.key == 'v' ){  
    vgui_dialog save_vrml_dlg("Save VRML for this frame");
    static vcl_string file_name = "";
    static vcl_string ext = "*.wrl";
    save_vrml_dlg.file("File:", ext, file_name);
    if( !save_vrml_dlg.ask())
      return true;

    vcl_ofstream vrml(file_name.c_str());
    write_vrml_header(vrml);
    for( vcl_map<bmrf_epi_seg_sptr, bmrf_node_sptr>::const_iterator itr = storage_->network()->begin(storage_->frame());
         itr != storage_->network()->end(storage_->frame()); ++itr ){
      itr->second->probability();
      if(itr->second->gamma().ptr() != NULL)
        if(itr->second->gamma()->mean() > 0.02 && itr->second->gamma()->mean() < 0.05)
          write_vrml_episeg(vrml, itr->first, itr->second->gamma(), 50.0);
    }
    vrml.close();
    return true;
  }

  if( !is_mouse_over )
    return true;
    
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  last_x_ = ix;
  last_y_ = iy;
  

  if( e.type == vgui_MOUSE_DOWN && e.button == vgui_LEFT ){
    static double total_error = 0.0;
    if( curr_node_ ){
      if(last_node_ && curr_node_ != last_node_){
        bmrf_node::arc_iterator a_itr = last_node_->begin();
        for ( ; a_itr != last_node_->end(); ++a_itr ){
          if((*a_itr)->to() == curr_node_)
            break;
        }
        if(a_itr != last_node_->end()){
          double alpha_range = (*a_itr)->max_alpha() - (*a_itr)->min_alpha();
          double int_var = 0.001; // intensity variance
          double error = alpha_range *( (*a_itr)->induced_match_error() 
                                      + (*a_itr)->avg_intensity_error()/int_var );
          total_error += error;
          vcl_cout << "==== Error: " << error << "\t Prob: " << exp(-error/2.0) << vcl_endl;
          vcl_cout << "==== Total Error: " << total_error 
                   << "\t Total Prob: " << exp(-total_error/2.0) << vcl_endl;
        }
        else
          total_error = 0.0;
      }

      last_node_ = curr_node_;
      if(print_stats_)
        print_arc_stats(curr_node_);
    }
  }

  if( e.type == vgui_MOTION){
    tableau_->motion(e.wx, e.wy);
    
    vgui_soview2D* curr_obj =  (vgui_soview2D*)tableau_->get_highlighted_soview();
    if( curr_obj != object_ ){
 
      object_ = (bgui_bmrf_epi_seg_soview2D*)curr_obj;
      curr_node_ = NULL;
      if( curr_obj && curr_obj->type_name() == "bgui_bmrf_epi_seg_soview2D"){   
        curr_node_ = storage_->network()->seg_to_node(object_->seg_sptr(), storage_->frame());

      }
      
      bvis1_manager::instance()->post_overlay_redraw();
    }
    if(draw_epipolar_line_)
      tableau_->post_overlay_redraw();

    double s, a;
    storage_->network()->epipole(storage_->frame()).to_epi_coords(double(ix),double(iy),s,a);
    vgui::out.width(6);
    vgui::out.fill(' ');
    vgui::out.precision(6);
    vgui::out << "image coords ("<<ix<<", "<<iy<<")  epipolar coords ("<<s<<", "<<a<<")\n";

    return true;
  }
  return false;
}


//: A vgui command used to do pruning
class bvis1_prune_command : public vgui_command
{
  public:  
  enum prune_types {PROB, GAMMA, DIRECT};

  bvis1_prune_command(dbmrf_inspector_tool* tool, prune_types type) 
    : tool_(tool), type_(type) {}

  void execute()
  {
    switch(type_){
    case PROB:
      tool_->prune_by_probability();
      break;
    case GAMMA:
      tool_->prune_by_gamma();
      break;
    case DIRECT:
      tool_->prune_directed();
      break;
    }
  }

  dbmrf_inspector_tool* tool_;
  prune_types type_;
};


//: Add popup menu items
void 
dbmrf_inspector_tool::get_popup( const vgui_popup_params& params, 
                                     vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";
  menu.add( ((draw_epipolar_line_)?on:off)+"Epipolar Line", 
            bvis1_tool_toggle, (void*)(&draw_epipolar_line_) );
  menu.add( ((draw_intensity_)?on:off)+"Intensity", 
            bvis1_tool_toggle, (void*)(&draw_intensity_) );
  menu.add( ((draw_neighbors_)?on:off)+"Neighbors", 
            bvis1_tool_toggle, (void*)(&draw_neighbors_) );
  menu.add( ((color_by_intensity_)?on:off)+"Color by Intensity", 
            bvis1_tool_toggle, (void*)(&color_by_intensity_) );
  menu.add( ((print_stats_)?on:off)+"Arc Stats", 
            bvis1_tool_toggle, (void*)(&print_stats_) );

  menu.add( "Prune by Probability", new bvis1_prune_command(this, bvis1_prune_command::PROB));
  menu.add( "Prune by Gamma", new bvis1_prune_command(this, bvis1_prune_command::GAMMA));
  menu.add( "Prune by Directed", new bvis1_prune_command(this, bvis1_prune_command::DIRECT));
}


//----------------------dbmrf_inspect_3d_tool---------------------------


//: Constructor
dbmrf_inspect_3d_tool::dbmrf_inspect_3d_tool()
 : curvel_storage_(NULL), curr_node_(NULL)
{
}


//: Destructor
dbmrf_inspect_3d_tool::~dbmrf_inspect_3d_tool()
{
}


//: Return the name of this tool
vcl_string
dbmrf_inspect_3d_tool::name() const
{
  return "3D Curvel Inspector"; 
}


//: This is called when the tool is activated
void 
dbmrf_inspect_3d_tool::activate()
{
  vgui_dialog storage_dialog("Select Reconstruction");

  vcl_vector<vcl_string> choices = bvis1_manager::instance()
    ->repository()->get_all_storage_class_names("curvel_3d");

  int selection = -1;
  storage_dialog.choice("curvel_3d storage", choices, selection);

  if( !storage_dialog.ask() || selection < 0 ){
    bvis1_manager::instance()->set_active_tool(NULL);
    return;
  }
  bpro1_storage_sptr c_storage = bvis1_manager::instance()
    ->repository()->get_data("curvel_3d", 0, selection);

  curvel_storage_.vertical_cast(c_storage);
}


//: Handle events
bool
dbmrf_inspect_3d_tool::handle( const vgui_event & e, 
                                   const bvis1_view_tableau_sptr& view )
{
  if( !tableau_.ptr() )
    return false;

  bool is_mouse_over = ( bvis1_manager::instance()->active_tableau()
                         == view->selector()->active_tableau() );

  if ( e.type == vgui_DRAW )
    return false;

  double frame = view->frame();

  // Draw neighbors as overlays
  if( e.type == vgui_DRAW_OVERLAY){
    if (!curr_curve_)
      return false;
      
    //vcl_set<bmrf_node_sptr> nodes;
    vgui_style_sptr line_style = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 1.0);
    line_style->apply_all();
    vcl_vector<float> pos_x, pos_y;
    for ( bmrf_curve_3d::iterator itr = curr_curve_->begin();
          itr != curr_curve_->end();  ++itr )
    {
      //bmrf_node_sptr proj = (*itr)->node_at_frame(frame);
      //if(proj)
      //  nodes.insert(proj);

      vnl_double_2 point;
      if( (*itr)->pos_in_frame((unsigned int)frame, point) ){
        pos_x.push_back((float)point[0]);
        pos_y.push_back((float)point[1]);
        vgui_soview2D_point((float)point[0], (float)point[1]).draw(); 
      }
      else if(!pos_x.empty()){
        vgui_soview2D_linestrip(pos_x.size(), &*pos_x.begin(), &*pos_y.begin()).draw();
        pos_x.clear();
        pos_y.clear();
      }
    }

    vgui_soview2D_linestrip(pos_x.size(), &*pos_x.begin(), &*pos_y.begin()).draw();
    //for ( vcl_set<bmrf_node_sptr>::iterator itr = nodes.begin();
    //      itr != nodes.end();  ++itr )
    //{   
    //  bgui_bmrf_epi_seg_soview2D((*itr)->epi_seg()).draw();
    //}
    
    return !is_mouse_over;
  }

  if( !is_mouse_over )
    return true;
    
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  

  if( e.type == vgui_MOUSE_DOWN && e.button == vgui_LEFT ){
    tableau_->motion(e.wx, e.wy);

    vgui_soview2D* curr_obj =  (vgui_soview2D*)tableau_->get_highlighted_soview();
    curr_node_ = NULL;
    vcl_set<bmrf_curve_3d_sptr> all_curves, found_curves;

    if( curr_obj && curr_obj->type_name() == "bgui_bmrf_epi_seg_soview2D"){ 
      bgui_bmrf_epi_seg_soview2D* bmrf_object = (bgui_bmrf_epi_seg_soview2D*)curr_obj;
      curr_node_ = storage_->network()->seg_to_node(bmrf_object->seg_sptr(), storage_->frame());

      curvel_storage_->get_curvel_3d(all_curves);
      
      int curve_num = 1;
      for ( vcl_set<bmrf_curve_3d_sptr>::iterator itr1 = all_curves.begin();
            itr1 != all_curves.end();  ++itr1, ++curve_num )
      {
        for ( bmrf_curve_3d::const_iterator itr2 = (*itr1)->begin();
              itr2 != (*itr1)->end();  ++itr2 )
        {
          if ( (*itr2)->is_projection(curr_node_) ){
            found_curves.insert(*itr1);
            vcl_cout << "found curve number " << curve_num << vcl_endl;
            break;
          }
        }
      }
    }
    
    if( found_curves.size() > 1 )
      vcl_cerr << "Warning found " << found_curves.size() << " matches" << vcl_endl;
    else if( found_curves.size() == 1 ){
      curr_curve_ = *(found_curves.begin());
      for ( bmrf_curve_3d::iterator itr = curr_curve_->begin();
            itr != curr_curve_->end();  ++itr )
      {
        vcl_cout << (*itr)->num_projections(true) 
                 << " gamma = " << (*itr)->gamma_avg() 
                 << " std = " << (*itr)->gamma_std() << vcl_endl;
      }
      vcl_cout << vcl_endl;
    }
    else
      curr_curve_ = NULL;

    bvis1_manager::instance()->post_overlay_redraw();
    return true;
  }

  return false;
}


//------------------------dbmrf_xform_tool-----------------------------

//: Constructor
dbmrf_xform_tool::dbmrf_xform_tool( const vgui_event_condition& lift,
                                            const vgui_event_condition& drop )
 : gesture_lift_(lift), gesture_drop_(drop),
   active_(false), node_(NULL)
{
}


//: Destructor
dbmrf_xform_tool::~dbmrf_xform_tool()
{
}


//: Return the name of this tool
vcl_string
dbmrf_xform_tool::name() const
{
  return "Segment Transform";
}


//: Handle events
bool
dbmrf_xform_tool::handle( const vgui_event & e, 
                              const bvis1_view_tableau_sptr& view )
{
  static bgui_bmrf_epi_seg_soview2D* overlay_seg = NULL;
  
  if( !tableau_.ptr() )
    return false;

  int frame = view->frame();

  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  double s, a;
  storage_->network()->epipole(storage_->frame()).to_epi_coords(double(ix),double(iy),s,a);

  if( active_ ){
    if( gesture_drop_(e) ){
      active_ = false;
      node_ = NULL;
      if(overlay_seg)
        delete overlay_seg;
      overlay_seg = NULL;
      cached_tableau_ =  vgui_easy2D_tableau_sptr(NULL);
      return true;
    }
    else if ( e.type == vgui_MOTION ){
      float gamma = 1.0 - init_s_/s;
      gamma_func_ = new bmrf_const_gamma_func(gamma);
      double prob = node_->probability(gamma_func_, -1) 
                    *node_->probability(gamma_func_, 1);
      vgui::out << "probablility = "<<prob<< "   gamma = "<<gamma<<"\n";

      double sum = 0.0;
      for ( bmrf_node::arc_iterator a_itr = node_->begin(); 
            a_itr != node_->end(); ++a_itr ){
        double exp_gamma = (*a_itr)->to()->gamma()->mean();
        exp_gamma /= 1.0 + exp_gamma*(*a_itr)->time_step();
        /*const double int_var = 0.001; // intensity variance
        double total_error = (*a_itr)->induced_match_error()/2.0
                            +(*a_itr)->avg_intensity_error()/(2.0*int_var);*/
        double alpha_range = (*a_itr)->max_alpha() - (*a_itr)->min_alpha();
        sum += (gamma - exp_gamma)*(gamma - exp_gamma)/*vcl_exp(-total_error)*/*alpha_range;
      }

      vcl_cout << gamma << '\t'<<prob<< '\t' << sum<< vcl_endl;

      cached_tableau_->post_overlay_redraw();
      return true;
    }
    else if ( e.type == vgui_DRAW_OVERLAY ){ 
      float time = frame - start_frame_;
      if (time == 0)
        time = 1.0; 
      bmrf_epi_seg_sptr temp = bmrf_epi_transform(node_->epi_seg(),gamma_func_,time,true);
      if(overlay_seg)
        delete overlay_seg;
      overlay_seg = new bgui_bmrf_epi_seg_soview2D(temp);
      overlay_seg->draw();
    }  
  }
  else{   
    if( gesture_lift_(e) ){
      vgui_soview2D* curr_obj = (vgui_soview2D*)tableau_->get_highlighted_soview();
      if( curr_obj && curr_obj->type_name() == "bgui_bmrf_epi_seg_soview2D"){
        bgui_bmrf_epi_seg_soview2D* seg_obj = (bgui_bmrf_epi_seg_soview2D*)curr_obj;
        node_ = storage_->network()->seg_to_node(seg_obj->seg_sptr(), storage_->frame());
        if( node_->epi_seg().ptr() ){
          active_ = true;

          start_frame_ = frame;
          cached_tableau_ = tableau_;
          init_s_ = s;
          init_a_ = a;
          double prob = node_->probability();
          vcl_cout << "The best probability is " << prob << vcl_endl;
          vcl_cout << "Selected "<< node_<<vcl_endl;
          double ri = node_->epi_seg()->avg_right_int();
          double li = node_->epi_seg()->avg_left_int();
          vcl_cout << "avg left int = " << li << " \tavg right int = " << ri << vcl_endl;
          for ( bmrf_node::arc_iterator a_itr = node_->begin(); 
                a_itr != node_->end(); ++a_itr ){
            if((*a_itr)->time_step() > 0){
              const double int_var = 0.001; // intensity variance
              double total_error = (*a_itr)->induced_match_error()/2.0
                                  +(*a_itr)->avg_intensity_error()/(2.0*int_var);
              double alpha_range = (*a_itr)->max_alpha() - (*a_itr)->min_alpha();
              vcl_cout << (*a_itr)->induced_gamma() <<'\t'<< vcl_exp(-total_error)*alpha_range 
                       << '\t' << (*a_itr)->probability()<< vcl_endl;
            }
          }
          vcl_cout << "--------------------------" << vcl_endl;
          for ( bmrf_node::arc_iterator a_itr = node_->begin(); 
                a_itr != node_->end(); ++a_itr ){
            if((*a_itr)->time_step() < 0){
              const double int_var = 0.001; // intensity variance
              double total_error = (*a_itr)->induced_match_error()/2.0
                                  +(*a_itr)->avg_intensity_error()/(2.0*int_var);
              double alpha_range = (*a_itr)->max_alpha() - (*a_itr)->min_alpha();
              vcl_cout << (*a_itr)->induced_gamma() <<'\t'<< vcl_exp(-total_error)*alpha_range 
                       << '\t' << (*a_itr)->probability()<< vcl_endl;
            }
          }
          vcl_cout << "--------------------------" << vcl_endl;
          return true; 
        }
      }
    }
  }

  return false;
}

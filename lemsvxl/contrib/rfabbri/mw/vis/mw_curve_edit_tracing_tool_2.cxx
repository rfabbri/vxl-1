#include "mw_curve_edit_tracing_tool_2.h"
#include "mw_curve_tracing_tool_common_2.h"

#include <vnl/vnl_random.h>
#include <vil/algo/vil_colour_space.h>
#include <vgui/vgui_dialog.h>
#include <mw/algo/mw_discrete_corresp_algo.h>
#include <mw/pro/mw_discrete_corresp_storage.h>


static vnl_random g_myrand;

mw_curve_edit_tracing_tool_2::
mw_curve_edit_tracing_tool_2()
  :
    mw_curve_tracing_tool_2(),
    tau_cost_(30.0)
{
  init();
}

void mw_curve_edit_tracing_tool_2::
init()
{
  gesture_query_corresp_= vgui_event_type(vgui_OVERLAY_DRAW);
  //: style of the objects in correspondence.
  corresp_objs_style_  = vgui_style::new_style(0.5, 1, 0.5, 
                                    5.0f/*ptsize*/, 3.0f/*linesize*/); // green

  o0_query_style_ = vgui_style::new_style(0, 0, 1, 
                                    5.0f/*ptsize*/, 5.0f/*linesize*/); // Blue

  o0_query_off_style_ = vgui_style::new_style(1, 0, 0, 
                                    5.0f/*ptsize*/, 5.0f/*linesize*/); // Red

  //: basic stuff is in layer 50
  //: higher numbers mean higher layers
  best_match_layer_ = "layer91";
  corresp_objs_layer_ = "layer60";
  o0_query_layer_ = "layer85";
}

vcl_string
mw_curve_edit_tracing_tool_2::name() const
{
  return "mcs Corresp. Editor v.2";
}

void mw_curve_edit_tracing_tool_2::
activate()
{
  vcl_cout << "mw_curve_edit_tracing_tool_2 ON\n";
  mw_curve_tracing_tool_2::activate();

  lock_corresp_query_ = false;
  o0_query_soview_ = NULL;
  o1_query_is_corresp_ = false;
  o0_corresp_soview_.resize(s_->num_curves(0), 0);

  get_corresp();
  color_objs_having_correpondents();
}

void mw_curve_edit_tracing_tool_2::
get_corresp()
{
  // Prompt the user to select input/output variable
  vgui_dialog io_dialog("Select Inputs" );

  vcl_vector< vcl_string > input_type_list;
  input_type_list.push_back("mw pt corresp");

  io_dialog.message("Select Input(s) From Available ones:");
  vcl_vector<int> input_choices(input_type_list.size());
  vcl_vector< vcl_vector <vcl_string> > available_storage_classes(input_type_list.size());
  vcl_vector< vcl_string > input_names(input_type_list.size());

  for( unsigned int i = 0 ; i < input_type_list.size(); i++ ) {
    // for this input type allow user to select from available storage classes in the repository
    available_storage_classes[i] = MANAGER->repository()->get_all_storage_class_names(input_type_list[i]);
    available_storage_classes[i].push_back("(NONE)");

    // Multiple choice - with the list of options.
    io_dialog.choice(input_type_list[i].c_str(), available_storage_classes[i], input_choices[i]);
  }

  if (!io_dialog.ask()) {
    vcl_cout << "Canceled\n";
    return;
  } else {
    vgui_dialog null_inputs("Name Missing Inputs");
    bool found_missing = false;
    for ( unsigned int i=0; i < input_choices.size(); i++) {
      if( input_choices[i]+1 < (int)available_storage_classes[i].size() ) {
        input_names[i] = available_storage_classes[i][input_choices[i]];
        null_inputs.message((input_type_list[i]+" : "+input_names[i]).c_str());
      }
      else{
        null_inputs.field(input_type_list[i].c_str(), input_names[i]);
        found_missing = true;
      }
    }
    if(found_missing)
      null_inputs.ask();
  }

  vcl_cout << "Selected input: " << input_names[0] << " ";
  
  bpro1_storage_sptr 
    p = MANAGER->repository()->get_data_by_name_at(input_names[0], frame_v_[0]);

  mw_discrete_corresp_storage_sptr c_sto;

  c_sto.vertical_cast(p);
  if(!p) {
    if (input_names.front().empty())
      input_names.front() = "auto_created_corresp";
    vcl_cerr << "Required storage of type '" << input_type_list.front() << "' missing. Creating a new blank one with name " 
      << input_names.front()
      << vcl_endl;
    bpro1_storage_sptr new_data = MANAGER->repository()->new_data(input_type_list.front(), input_names.front());
    c_sto.vertical_cast(new_data);
    assert(c_sto);
  }
  
  corr_ = c_sto->corresp();
  if(!corr_) {
    vcl_cout << "Empty storage - allocating data" << vcl_endl;
    corr_ = new mw_discrete_corresp(s_->num_curves(0), s_->num_curves(1));
    corr_->set_checksum(mw_discrete_corresp_algo::compute_checksum(*s_));
    c_sto->set_corresp(corr_); // storage deletes it
  } else {
    vcl_cout << "Non-empty storage" << vcl_endl;
    if (corr_->n_objects_view_0() != s_->num_curves(s_->v0())) {
      vcl_cerr << "ERROR: input correspondence is not valid for current curves\n" 
        << vcl_endl;
      vcl_cerr << "corresp objs v0: " << corr_->n_objects_view_0() << vcl_endl;
      vcl_cerr << "curve frag map size v0: " << s_->num_curves(s_->v0()) << vcl_endl;
      deactivate();
      return;
    }
    if (corr_->n_objects_view_1() != s_->num_curves(s_->v1())) {
      vcl_cerr << "ERROR: input correspondence is not valid for current curves\n" 
        << vcl_endl;
      vcl_cerr << "corresp objs v1: " << corr_->n_objects_view_1() << vcl_endl;
      vcl_cerr << "curve frag map size v1: " << s_->num_curves(s_->v1()) << vcl_endl;
      vcl_cerr << "Trying to fix it (be careful - results might be misleading!)" << vcl_endl;

      corr_->set_size(s_->num_curves(s_->v0()), s_->num_curves(s_->v1()));
    }
    unsigned long cksum = mw_discrete_corresp_algo::compute_checksum(*s_);
    if (corr_->checksum() != cksum) {
      vcl_cerr << "********************************************************\n";
      vcl_cerr << "WARNING: input correspondence checksum doesn't match. This means that either: \n"
        << "1) is not valid for current curves or 2) it is an old version or 3) whoever produced it didn't set the checksum.\n"
        << vcl_endl;
      vcl_cerr << "corresp checksum: " << corr_->checksum() << vcl_endl
               << "computed checksum: " << cksum << vcl_endl;
      vcl_cerr << "We will be fixing the checksum, in case it was old.\n";
      vcl_cerr << "********************************************************\n";
      vgui_dialog d("Warning");
      corr_->set_checksum(cksum);
      d.message("Warning: fixed checksum.");
      d.ask();
    }
  }
  vcl_cout << "Corresp NAME: " << c_sto->name() << vcl_endl;
  vcl_cout << "Corresp: " << " : \n" << *corr_ << vcl_endl;
}

void mw_curve_edit_tracing_tool_2::
color_objs_having_correpondents()
{
  // coloring of points in image 0 having any correspondents
  mw_discrete_corresp &corr = *corr_;

  // XXX modified for CVPR result
  /*
  tab_[0]->set_current_grouping(corresp_objs_layer_.c_str());
  for (unsigned i=0; i < corr_->size(); ++i) {
    if (! corr[i].empty() ) {
      o0_corresp_soview_[i] = 
        tab_[0]->add_vsol_polyline_2d(s_->curves(0,i));

      o0_corresp_soview_[i]->set_style(corresp_objs_style_);
    }
  }
  tab_[0]->post_redraw();
  */

  // New one:

  tab_[0]->set_current_grouping(corresp_objs_layer_.c_str());
  tab_[1]->set_current_grouping(corresp_objs_layer_.c_str());
  for (unsigned i=0; i < corr_->size(); ++i) {
    if (! corr[i].empty() ) {

      double r;
      double g;
      double b;

      // randomize hue
      double h = g_myrand.drand64(0,360);

      vil_colour_space_HSV_to_RGB<double>(h,1,255,&r, &g,&b);
      r/=255.0;
      g/=255.0;
      b/=255.0;

      vgui_style_sptr oldstyle = corresp_objs_style_;
      vgui_style_sptr newstyle = vgui_style::new_style(r,g,b,oldstyle->point_size,oldstyle->line_width);

      o0_corresp_soview_[i] = 
        tab_[0]->add_vsol_polyline_2d(s_->curves(0,i));

      o0_corresp_soview_[i]->set_style(newstyle);

      // Now color curve in second view:
      vcl_list<mw_attributed_object>::const_iterator itr;
      itr = corr[i].begin(); unsigned ii=0;
      for (; itr != corr[i].end(); 
           ++itr, ++ii) {

        vcl_cout << "Num_curves: " << s_->num_curves(1) << "itr->obj: " << itr->obj_ << vcl_endl;
        o1_corresp_soview_.push_back(
          tab_[1]->add_vsol_polyline_2d(s_->curves(1,itr->obj_)));

        o1_corresp_soview_.back()->set_style(newstyle);
      }
    }
  }
  tab_[0]->post_redraw();
  tab_[1]->post_redraw();
}

void mw_curve_edit_tracing_tool_2::
deactivate ()
{
  mw_curve_tracing_tool_2::deactivate();
  vcl_cout << "mw_curve_edit_tracing_tool_2 OFF\n";
}

bool
mw_curve_edit_tracing_tool_2::
handle( const vgui_event & e, 
        const bvis1_view_tableau_sptr& view )
{
  //: mouse motion
  if (e.type == gesture_query_corresp_) {
    if ( view->frame() == frame_v_[0] ) {
      if (!lock_corresp_query_)
        handle_corresp_query_at_view_0(e,view);
    }
    return false;
  }

  bool used_event = mw_curve_tracing_tool_2::handle(e, view);

  if (e.type == vgui_KEY_PRESS)
    return used_event || handle_key(e.key);

  //: click
  if (gesture0_(e)) {
    bool base_handle_successfully_used_click = used_event;
    if (view->frame() == frame_v_[1] && base_handle_successfully_used_click)
      return handle_mouse_event_at_view_1(e, view);
  }

  return false;
}

bool mw_curve_edit_tracing_tool_2::
handle_key(vgui_key key)
{
  mw_discrete_corresp &corr = *corr_;

  switch (key) {
    case 277: // Del
      if (o1_query_is_corresp_) {
        vcl_cout << "Removing " <<  o0_query_idx_ << ",  " << *o1_query_itr_ <<  vcl_endl;
        corr[o0_query_idx_].erase(o1_query_itr_);
        o1_query_is_corresp_ = false;

        // - delete this guy from correspondents soview list

        tab_[1]->remove(*(correspondents_idx_[o1_query_idx_]));

        correspondents_soview_.erase(correspondents_idx_[o1_query_idx_]);
        correspondents_idx_.erase(o1_query_idx_);

        if (corr[o0_query_idx_].empty() && 
            o0_corresp_soview_[o0_query_idx_] != 0) {
          tab_[0]->remove(o0_corresp_soview_[o0_query_idx_]);
          o0_corresp_soview_[o0_query_idx_] = 0;
        }

        tab_[1]->post_redraw();
        tab_[0]->post_redraw();
      } else {
        vcl_cout << "Selected correspondence inexistent\n";
      }

      return true;
      break;

    case 275: // Home (think "Ins", but macs don't have ins, so I used home)
      if (!o1_query_is_corresp_) {

        mw_discrete_corresp::one_corresp_list_iter itr;
        bool stat = 
          corr_->add_unique( mw_attributed_object(o1_query_idx_), o0_query_idx_, &itr);

        if (stat) {
          vcl_cout << "Inserting " << o0_query_idx_ << ",  " << *itr << vcl_endl;

          o1_query_is_corresp_ = true;

          o1_query_itr_ = itr;
          o1_query_idx_ = itr->obj_;

          tab_[1]->set_current_grouping( corresp_objs_layer_.c_str() );
          // - add this guy to correspondents soview list; breaking sort order
          correspondents_soview_.push_back(
              tab_[1]->add_vsol_polyline_2d(s_->curves(1, itr->obj_)) );
          correspondents_soview_.back()->set_style(corresp_objs_style_);
          correspondents_idx_[o1_query_idx_] = --correspondents_soview_.end();

          tab_[0]->set_current_grouping(corresp_objs_layer_);
          if (o0_corresp_soview_[o0_query_idx_] == 0) {
            o0_corresp_soview_[o0_query_idx_] = tab_[0]->add_vsol_polyline_2d(
                s_->curves(0, o0_query_idx_) );
            o0_corresp_soview_[o0_query_idx_]->set_style(corresp_objs_style_);
          }

          tab_[1]->post_redraw();
          tab_[0]->post_redraw();
        } else {
          vcl_cout << "Insert: object is already in corresp datastructure; ignoring.\n";
          vcl_cout << "o1_query_idx_:" << o1_query_idx_ << "  o0_query_idx_:" 
            << o0_query_idx_ << vcl_endl;
        }
      } else {
        vcl_cout << "Selected correspondence already inserted\n";
      }
      break;

    case 'p': // print misc info / debug
      vcl_cout << *corr_ << vcl_endl;

      return true;
      break;
    case 'l':
      toggle_lock_correspondence_query();
      return true;
      break;
    case '7':
      vcl_cout << "Filling correspondence structure with epiband candidates. (Not implemented)\n";
      //      get_epipolar_candidates();
    break;
    case '8':
      vcl_cout << "Writing corresp. costs to file... (Not implemented)";
      vgui::out << "Writing corresp. costs to file... (Not implemented)";
      // write_energies();
      vcl_cout << "done!\n";
      vgui::out << "done!\n";
    break;

    case '9':
      vcl_cout << "Trimming any ones below cost threshold...";
      corr_->threshold_by_cost(tau_cost_);
      vcl_cout << "done!\n";
    break;

    case '0': // misc. stuff
      vcl_cout << "Sorting corrresps....";
      corr_->sort();
      vcl_cout << "done!\n";
      break;
    default:
      vcl_cout << "(curve_edit_tracing_tool) Unassigned key: " << key << " pressed.\n";
      return false;
      break;
  }
  return true;
}

void mw_curve_edit_tracing_tool_2::
toggle_lock_correspondence_query()
{
  vcl_string state;
  if (lock_corresp_query_) {
    lock_corresp_query_= false;
    state = "off";
  } else {
    lock_corresp_query_= true;
    state = "on";
  }
  vcl_cout << "Inspect correspondence " << state << vcl_endl;;
  vgui::out << "Inspect correspondence " << state << vcl_endl;;
}

//: Returns true if mouse over valid edge in 1st view
bool mw_curve_edit_tracing_tool_2::
handle_corresp_query_at_view_0
    ( const vgui_event & /*e*/, 
    const bvis1_view_tableau_sptr& /*view*/ )
{
  vgui_soview* selected_soview_base =  
    curve_tableau_current_->get_highlighted_soview();

  bgui_vsol_soview2D_polyline *selected_soview
    = dynamic_cast<bgui_vsol_soview2D_polyline *>(selected_soview_base); 

  if (!selected_soview || !selected_soview->sptr())
    return false;

  selected_obj_in_corresp_ = selected_soview->sptr();

  mw_discrete_corresp &corr = *corr_;
  // identify the index of the selected obj in our data
  unsigned idx;
  
  bool found = s_->get_index_of_curve(selected_obj_in_corresp_, 0, &idx);

  if (!found) {
    vcl_cout << "Error: selected curve not found among vsols in view 0\n";
    return false;
  }

  o0_query_idx_ = idx;

  if (o0_query_idx_ >= s_->num_curves(0)) {
    vcl_cout << "Error: index is wrong: " << vcl_endl;
    vcl_cout << "index" << o0_query_idx_ << " out of " 
              << s_->num_curves(0) << vcl_endl;
    return false;
  }

  //  vcl_cout << "View[0]: mouse over object[" << o0_query_idx_ << "] out of " 
  //            << s_->num_curves(0) << vcl_endl;

  // mark selected edgel
  if (o0_query_soview_) {
    // Just cosmetic patch up to increase responsiveness.
    o0_query_off_style_->apply_all();
    o0_query_soview_->set_style(o0_query_off_style_);
    o0_query_soview_->draw();

    tab_[0]->remove(o0_query_soview_);
  }

  tab_[0]->set_current_grouping(o0_query_layer_);
  o0_query_soview_ = tab_[0]->add_vsol_polyline_2d(s_->curves(0,idx));
  o0_query_soview_->set_style(o0_query_style_);
  o0_query_style_->apply_all();
  o0_query_soview_->draw();

  // add the correspondents in 2nd view
  for (vcl_list<bgui_vsol_soview2D *>::iterator 
      itr = correspondents_soview_.begin() ; 
      itr != correspondents_soview_.end(); ++itr) {
    tab_[1]->remove(*itr);
  }
  correspondents_soview_.clear();
  correspondents_idx_.clear();

  vcl_list<mw_attributed_object>::const_iterator itr;
  itr = corr[idx].begin(); unsigned ii=0;
  for (; itr != corr[idx].end(); 
       ++itr, ++ii) {

    if (ii < best_match_style_.size()) { //: display top 5 matches
      tab_[1]->set_current_grouping( best_match_layer_ );
    } else {
      tab_[1]->set_current_grouping( corresp_objs_layer_ );
    }

    correspondents_soview_.push_back(
        tab_[1]->add_vsol_polyline_2d(s_->curves(1, itr->obj_)) );
    correspondents_idx_[itr->obj_] = --correspondents_soview_.end();

    if (ii < best_match_style_.size()) { //: display top 5 matches
      correspondents_soview_.back()->set_style(best_match_style_[ii]);
    } else {
      correspondents_soview_.back()->set_style(corresp_objs_style_);
    }
  }

  tab_[1]->post_redraw();
  return true;
}

void mw_curve_edit_tracing_tool_2::
get_popup( const vgui_popup_params& /*params*/, vgui_menu &menu )
{
  menu.add( "tau_cost", 
            bvis1_tool_set_param, (void*)(&tau_cost_) );
}

bool mw_curve_edit_tracing_tool_2::
handle_mouse_event_at_view_1(
    const vgui_event & /*e*/,
    const bvis1_view_tableau_sptr& /*view*/ )
{
  vcl_cout << "Clicked----------------------\n" << vcl_endl;
  mw_discrete_corresp &corr = *corr_;

  o1_query_idx_ = s_->selected_crv_id(1);
  vcl_cout << "View[1]: mouse click on point[" << o1_query_idx_ << "] out of " 
            << s_->num_curves(1) << vcl_endl;

  // find clicked point among current correspondences + print info

  vcl_list<mw_attributed_object>::iterator itr = corr[o0_query_idx_].begin();  
  unsigned  ii=0;
  for (; itr != corr[o0_query_idx_].end(); ++itr, ++ii) {
    if (itr->obj_ == o1_query_idx_)
      break;
  }

  if (itr != corr[o0_query_idx_].end()) {
    vcl_cout << "Clicked on corresponding curve [" <<  ii << "]"  << " out of " 
      << corr[o0_query_idx_].size() << " correspondents" << vcl_endl;
    vcl_cout << "    " << *itr << vcl_endl;
    o1_query_itr_ = itr;
    o1_query_is_corresp_ = true;
  } else {
    vcl_cout << "Clicked point NOT found among correspondents\n";
    o1_query_is_corresp_ = false;
  }

  // reconstruct+reproject + add soview
  return true;
}

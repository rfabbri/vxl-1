// This is brcv/seg/dbdet/vis/dbdet_livewire_tool.cxx
//:
// \file

#include "dbdet_livewire_tool.h"
#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <vil1/vil1_vil.h>
#include <vil1/vil1_memory_image_of.h>

#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>

#include <vgui/vgui_projection_inspector.h>
#include <vcl_iostream.h>
#include <vgui/vgui.h> 
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>

#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_point_2d.h>
#include <dbgl/algo/dbgl_eulerspiral.h>     
#include <brip/brip_vil_float_ops.h>

//: Constructor - protected
dbdet_livewire_tool::dbdet_livewire_tool()
 :  tableau_(NULL), seed_picked(false), edit(false), edit1_picked(false), edit2_picked(false),
    start_ind(0), end_ind(0), seed_x(0), seed_y(0)
{
  gesture_seed = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture_clear = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  gesture_smooth = vgui_event_condition(vgui_key('s'), vgui_SHIFT, true);
  gesture_edit = vgui_event_condition(vgui_key('e'), vgui_SHIFT, true);
}

//: This is called when the tool is activated
void dbdet_livewire_tool::activate() {

  osl_canny_ox_params canny_params;
  dbdet_lvwr_params iparams;

  get_intscissors_params(&iparams, &canny_params);

  intsciss.set_params(iparams);
  intsciss.set_canny_params(canny_params);

  contour.clear();
  style = vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 3.0f);
  style->apply_all();

  vcl_cout << "The tool is activated!!!\n";

}

//: Set the tableau to work with
bool
dbdet_livewire_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
  if( tableau.ptr() != NULL && tableau->type_name() == "vgui_image_tableau" ){
    tableau_.vertical_cast(tableau);
    return true;
  }

  vcl_cout << "NON vgui_image_tableau is set!! name is : " << tableau->type_name() << " \n";
  tableau_ = NULL;
  return false;
}

bool
dbdet_livewire_tool::handle( const vgui_event & e, 
                             const bvis1_view_tableau_sptr& view )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  int mouse_x = int(ix+0.5);
  int mouse_y = int(iy+0.5);

  if (!seed_picked && gesture_seed(e)) {

    bpro1_storage_sptr input_storage_sptr = bvis1_manager::instance()->repository()->get_data("image",0,0);

    vidpro1_image_storage_sptr frame;
    frame.vertical_cast(input_storage_sptr);
    vil_image_resource_sptr image_sptr = frame->get_image();

    if (!(mouse_x >= 0 && mouse_x < (int)image_sptr->ni() && mouse_y >= 0 && mouse_y < (int)image_sptr->nj())) {
      vcl_cout << "Not a valid seed point on image!!\nSelect a new seed point inside the image!!\n"; 
      return false;
    }

    seed_picked = true;
    seed_x = mouse_x;
    seed_y = mouse_y;
    seed = new vgui_soview2D_point(seed_x, seed_y);
    seed->set_style(style);
    vcl_cout << *seed;
    top = new vgui_soview2D_lineseg(seed_x-intsciss.params_.window_w/2,
                    seed_y-intsciss.params_.window_h/2,
                    seed_x+intsciss.params_.window_w/2,
                    seed_y-intsciss.params_.window_h/2);
    left = new vgui_soview2D_lineseg(seed_x-intsciss.params_.window_w/2,
                     seed_y+intsciss.params_.window_h/2,
                     seed_x+intsciss.params_.window_w/2,
                     seed_y+intsciss.params_.window_h/2);
    right = new vgui_soview2D_lineseg(seed_x-intsciss.params_.window_w/2,
                      seed_y-intsciss.params_.window_h/2,
                    seed_x-intsciss.params_.window_w/2,
                    seed_y+intsciss.params_.window_h/2);
    bottom = new vgui_soview2D_lineseg(seed_x+intsciss.params_.window_w/2,
                     seed_y-intsciss.params_.window_h/2,
                     seed_x+intsciss.params_.window_w/2,
                     seed_y+intsciss.params_.window_h/2);
   
    vil_image_resource_sptr grey_image_sptr = vil_new_image_resource_of_view( *vil_convert_to_grey_using_rgb_weighting ( image_sptr->get_view() ) );
    vil_image_view<float> float_image = brip_vil_float_ops::convert_to_float(*grey_image_sptr);
    vil1_memory_image_of<float> img;
    if (intsciss.params_.use_given_image) {
      vil_image_view<double> double_img;
      vil_convert_stretch_range(float_image, double_img, 0.0f, 1.0f);
      vil_convert_cast(double_img, float_image);
    }
    img = vil1_from_vil_image_view(float_image);
    intsciss.compute(img, seed_x, seed_y);
    vcl_cout << "Paths are computed..., seed_x: " << seed_x << " seed_y: " << seed_y <<"\n";

    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  if (seed_picked && !edit && gesture_seed(e)) {
    for (unsigned int i = 0; i<temp.size(); i++)
      delete temp[i];

    temp.clear();
    cor.clear();

    bool out = false;
    out = intsciss.get_path(mouse_x, mouse_y, cor);
    if (!out) {
      seed_x = mouse_x;
      seed_y = mouse_y;

      if (cor.size() != 0) {
        vcl_vector<vcl_pair<int, int> >::reverse_iterator p;
        int i = contour.size();
        for (p = cor.rbegin(); p != cor.rend(); p++) {
          contour.push_back(new vgui_soview2D_point(p->second, p->first));
          contour[i]->set_style(style);
          i++;
        }
      }

      seed->x = seed_x;
      seed->y = seed_y;
      intsciss.compute_directions(seed_x, seed_y);

      translate_rectangle();
    }

    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  if (seed_picked && !edit && (e.modifier == vgui_MODIFIER_NULL) && (e.type == vgui_MOTION)) {
    for (unsigned int i = 0; i<temp.size(); i++)
      delete temp[i];

    temp.clear();
    cor.clear();

    bool out = false;
    out = intsciss.get_path(mouse_x, mouse_y, cor);

    if (cor.size() > 0) {
      if (!out) {
      for (unsigned int i = 0; i < cor.size(); i++) {
        temp.push_back(new vgui_soview2D_point(cor[i].second, cor[i].first));
        temp[i]->set_style(style);
      }

      } else {  // mouse pointer is out of the rectangle
        // find a candidate to be a seed
        float cost1, cost2, cost_dif, cost;
        cost1 = intsciss.get_global_cost(cor[0].second, cor[0].first);
        cost_dif = 0;
        for (unsigned int i = 1; i < cor.size(); i++) {
          cost2 = intsciss.get_global_cost(cor[i].second, cor[i].first);
          cost_dif += cost1-cost2;
          cost1 = cost2;
        }
        cost_dif /= cor.size(); // average cost difference

        last_xx = cor[1].second;
        last_yy = cor[1].first;
        last_x = seed_x;
        last_y = seed_y;
        cost1 = intsciss.get_global_cost(cor[0].second, cor[0].first);
        for (unsigned int i = 1; i < cor.size(); i++) {

          cost2 = intsciss.get_global_cost(cor[i].second, cor[i].first);
          cost = cost1-cost2;
          if (cost <= cost_dif) {
            last_x = cor[i].second;
            last_y = cor[i].first;
            break;
          }
          cost1 = cost2;
        }

        if ( vcl_abs(float(last_x - seed_x)) > 2 || vcl_abs(float(last_y - seed_y)) > 2) {
          seed_x = last_x;
          seed_y = last_y;
        } else {
          seed_x = last_xx;
          seed_y = last_yy;
        }

        intsciss.get_path(seed_x, seed_y, cor);

        if (cor.size() != 0) {
          vcl_vector<vcl_pair<int, int> >::reverse_iterator p;
          int i = contour.size();
          for (p = cor.rbegin(); p != cor.rend(); p++) {
            contour.push_back(new vgui_soview2D_point(p->second, p->first));
            contour[i]->set_style(style);
            i++;
          }
        }

        seed->x = seed_x;
        seed->y = seed_y;
        intsciss.compute_directions(seed_x, seed_y);
        translate_rectangle();
      }
    }

    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  // clear last 10 points from the current contour, and update seed
  if (seed_picked && !edit && gesture_clear(e)) {

    for (unsigned int i = 0; i<temp.size(); i++)
      delete temp[i];
    temp.clear();

    if (contour.size() > 10) {
      vcl_vector<vgui_soview2D_point*>::iterator p = contour.end()-1;
      for (unsigned i = 0; i<10; i++) {
        contour.erase(p);
        p = contour.end()-1;
      }
    } else if (contour.size() > 0) {
        vcl_vector<vgui_soview2D_point*>::iterator p = contour.end()-1;
        contour.erase(p);
    }

    if (contour.size() > 0) {
      vcl_vector<vgui_soview2D_point*>::iterator p = contour.end()-1;

      seed_x = int(0.5 + ((vgui_soview2D_point*)(*p))->x);
      seed_y = int(0.5 + ((vgui_soview2D_point*)(*p))->y);
      seed->x = seed_x;
      seed->y = seed_y;
      intsciss.compute_directions(seed_x, seed_y);

      translate_rectangle();
    } else {
      seed_picked = false;
      contour.clear();
      for (unsigned int i = 0; i<temp.size(); i++)
        delete temp[i];
      temp.clear();
      delete seed;
      delete top;
      delete left;
      delete bottom;
      delete right;
      intsciss.free_buffers();
    }

    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }


  // smooth the current contour
  if (seed_picked && !edit && gesture_smooth(e)) {
    vcl_cout << "in gesture smooth\n";
    vcl_vector<vgl_point_2d<double> > curve;
    curve.clear();
    
    for (unsigned i = 0; i<contour.size(); i++) {
      curve.push_back(vgl_point_2d<double>(((vgui_soview2D_point*)contour[i])->x, 
                                   ((vgui_soview2D_point*)contour[i])->y));
    }
  
    bdgl_curve_algs::smooth_curve(curve, 1.0);

    contour.clear();
    curve.erase(curve.begin());
    curve.erase(curve.begin());
    curve.erase(curve.begin());

    curve.erase(curve.end()-1);
    curve.erase(curve.end()-1);
    curve.erase(curve.end()-1);
   
    for (unsigned i = 0; i<curve.size(); i++) {
      contour.push_back(new vgui_soview2D_point(curve[i].x(), curve[i].y()));
    }
    
    for (unsigned int i = 0; i<temp.size(); i++)
          delete temp[i];
    temp.clear();
        
    vcl_vector<vgui_soview2D_point*>::iterator p = contour.end()-1;
    seed_x = int(0.5 + ((vgui_soview2D_point*)(*p))->x);
    seed_y = int(0.5 + ((vgui_soview2D_point*)(*p))->y);
    seed->x = seed_x;
    seed->y = seed_y;
    intsciss.compute_directions(seed_x, seed_y);
    translate_rectangle();

    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

    // save the current contour into a .cem file
  if (seed_picked && !edit && e.type == vgui_MOUSE_PRESS && e.button == vgui_RIGHT) {

    vcl_string image_filename = "";
    
    image_filename.append(".cem");

    static vcl_string regexp = "*.*";

    vgui_dialog save_dl("Save contour");
    save_dl.inline_file("Filename: ", regexp, image_filename);

    if (save_dl.ask()) {
      if (image_filename.find(".cem") == (vcl_string::size_type) -1)
         image_filename.append(".cem");

      vcl_ofstream fs(image_filename.c_str());
      vcl_string image_filename2(image_filename.c_str());
      vcl_string s2 = ".con";

      image_filename2.replace(image_filename.find(".cem") , s2.length(), s2 );
      
      vcl_ofstream fs2(image_filename2.c_str());
      
      if (!fs || !fs2)
        vcl_cout << "Problems in opening file: " << image_filename << "\n";
      else {
        fs << "CONTOUR_COUNT=1\nTOTAL_EDGE_COUNT=" << contour.size() << "\n";
        fs << "[BEGIN CONTOUR]\nEDGE_COUNT=" << contour.size() << "\n";
        fs2 << "CONTOUR\nOPEN\n" << contour.size() << "\n";
        for (unsigned i = 0; i<contour.size(); i++) {
          fs << "[0, 0] 0 0 [" << ((vgui_soview2D_point*)contour[i])->x << ", ";
          fs << ((vgui_soview2D_point*)contour[i])->y << "] 0 0\n";

          fs2 << ((vgui_soview2D_point*)contour[i])->x << " ";
          fs2 << ((vgui_soview2D_point*)contour[i])->y << "\n";
        }
        fs << "[END CONTOUR]\n";
        fs.close();
        fs2.close();
        vcl_cout << "Contour written to the specified files!\n";

        seed_picked = false;
        contour.clear();
        for (unsigned int i = 0; i<temp.size(); i++)
          delete temp[i];
        temp.clear();
        delete seed;
        delete top;
        delete left;
        delete bottom;
        delete right;
        intsciss.free_buffers();
      }
    }

    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  if (seed_picked && !edit && gesture_edit(e)) {
    edit = true;
    bvis1_manager::instance()->post_overlay_redraw();
    vcl_cout << "In Edit Mode!\nPress SHIFT+e again to go out of this mode.\n";
    return false;
  }

  if (seed_picked && edit && gesture_seed(e)) {
    if (!edit1_picked) {

      vcl_vector< vsol_point_2d_sptr > point_list;
      for (unsigned i = 0; i<contour.size(); i++) {
        point_list.push_back( vsol_point_2d_sptr( new vsol_point_2d( ((vgui_soview2D_point*)contour[i])->x , 
                                                                   ((vgui_soview2D_point*)contour[i])->y ) ) );
      }

      dc = new vsol_digital_curve_2d(point_list);
      if (!dc) {
        vcl_cout << "Problems in contour!!!\nSelect a point again!!\n";
        return false;
      } //else
        //vcl_cout << "dc created with size: " << dc->size() << " contour size was: " << contour.size() << "\n";

      double ind = closest_index(vgl_point_2d<double>(mouse_x, mouse_y), dc);
      start_ind = (int)floor(ind);
      if (start_ind - 3 < 0 || start_ind + 3 > (int)contour.size())
        vcl_cout << "!!!! Point too close to the ends of boundary!!!!\nSelect a new start edit point!!!!\n";
      else 
        edit1_picked = true;

    } else if (!edit2_picked) {
      double ind = closest_index(vgl_point_2d<double>(mouse_x, mouse_y), dc);
      end_ind = (int)floor(ind);
      
      if (end_ind - 3 < 0 || end_ind + 3 > (int)contour.size()) {
        vcl_cout << "!!!! Point too close to the ends of boundary!!!!\nSelect a new end edit point!!!!\n";
        return false;
      }
      
      edit2_picked = true;

      if (start_ind > end_ind) {
        int temp;
        temp = start_ind;
        start_ind = end_ind;
        end_ind = temp;
      }

      vcl_vector<vgui_soview2D_point*> contour1;
      vcl_vector<vgui_soview2D_point*> contour2;

      contour1.clear();
      contour2.clear();

      int i;
      for (i = 0; i<start_ind; i++)
        contour1.push_back(contour[i]);
    
      for (i=end_ind+1; i<(int)contour.size(); i++)
        contour2.push_back(contour[i]);

      contour.clear();
      for (i = 0; i<(int)contour1.size(); i++) 
        contour.push_back(contour1[i]);
      
      //vcl_cout << "Cut out " << end_ind - start_ind << " number of points from contour\n";

      dbgl_eulerspiral es_curve;

      //Point2D<double> start_pt, end_pt;
      double start_angle, end_angle;
      vgl_point_2d<double> point1 = dc->interp(start_ind);
      vgl_point_2d<double> point2 = dc->interp(end_ind);
      //start_pt.set(point1.x(), point1.y());
      double xs1, ys1, xs2, ys2, xe1, ye1, xe2, ye2, pi;
      pi = 3.14;
      assert(start_ind-3>-1);
      assert(contour2.size() > 2);
      xs1 = ((vgui_soview2D_point*)contour1[start_ind-3])->x;
      ys1 = ((vgui_soview2D_point*)contour1[start_ind-3])->y;
      xs2 = point1.x();
      ys2 = point1.y();

      xe1 = point2.x();
      ye1 = point2.y();
      xe2 = ((vgui_soview2D_point*)contour2[2])->x;
      ye2 = ((vgui_soview2D_point*)contour2[2])->y;

      if (xs1 == xs2) {
        if ((ys2 - ys1) > 0)
          start_angle = (pi/2);
        else 
          start_angle = (3*pi/2);
      } else if (ys1 == ys2) {
        if ((xs2 - xs1) > 0)
          start_angle = 0;
        else 
          start_angle = pi;
      } else {
        double ratio = (ys2-ys1)/(xs2-xs1);
        start_angle = atan(ratio);
        if ((ratio > 0 && (ys2-ys1) < 0) || (ratio < 0 && (ys2-ys1) > 0))
          start_angle = pi + start_angle;
      }
        
      //end_pt.set(point2.x(), point2.y());
      
      if (xe1 == xe2) {
        if ((ye2 - ye1) > 0)
          end_angle = (pi/2);
        else 
          end_angle = (3*pi/2);
      } else if (ye1 == ye2) {
        if ((xe2-xe1) > 0)
          end_angle = 0;
        else 
          end_angle = pi;
      } else {
        double ratio = (ye2-ye1)/(xe2-xe1);
        end_angle = atan(ratio);
        if ((ratio > 0 && (ye2-ye1) < 0) || (ratio < 0 && (ye2-ye1) > 0))
          end_angle = pi + end_angle;
      }
  
      // set euler spiral properties
      es_curve = dbgl_eulerspiral(point1, start_angle, point2, end_angle);

      vcl_vector<vgl_point_2d<double> > point_samples;
      es_curve.compute_spiral(point_samples, 1.0);
      //vcl_cout << "Number of points in ES curve " << i << " = " << point_samples.size() << vcl_endl;

      for (unsigned int i = 0; i<point_samples.size(); i++)
        contour.push_back(new vgui_soview2D_point(point_samples[i].x(), point_samples[i].y()));

      for (unsigned int i = 0; i<contour2.size(); i++)
        contour.push_back(contour2[i]);

      contour1.clear();
      contour2.clear();
      point_samples.clear();
      edit1_picked = false;
      edit2_picked = false;
    }

    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  if (seed_picked && edit && gesture_edit(e)) {
    edit = false;
    edit1_picked = false;
    edit2_picked = false;
    return false;
  }

  if (e.type == vgui_DRAW_OVERLAY) {
    if (seed_picked) {
      top->draw();
      left->draw();
      right->draw();
      bottom->draw();
      seed->draw();
      if (!edit)
        for (unsigned int i = 0; i<temp.size(); i++)
          temp[i]->draw();
      for (unsigned int i = 0; i<contour.size(); i++)
        contour[i]->draw();
    }

    return false;
  }

  return false;
}

vcl_string
dbdet_livewire_tool::name() const
{
  return "Livewire";
}

void dbdet_livewire_tool::translate_rectangle() {
  // translate box around the new seed point
  top->x0 = seed_x-intsciss.params_.window_w/2;
  top->y0 = seed_y-intsciss.params_.window_h/2;
  top->x1 = seed_x+intsciss.params_.window_w/2;
  top->y1 = seed_y-intsciss.params_.window_h/2;
  left->x0 = seed_x-intsciss.params_.window_w/2;
  left->y0 = seed_y+intsciss.params_.window_h/2;
  left->x1 = seed_x+intsciss.params_.window_w/2;
  left->y1 = seed_y+intsciss.params_.window_h/2;
  right->x0 = seed_x-intsciss.params_.window_w/2;
  right->y0 = seed_y-intsciss.params_.window_h/2;
  right->x1 = seed_x-intsciss.params_.window_w/2;
  right->y1 = seed_y+intsciss.params_.window_h/2;
  bottom->x0 = seed_x+intsciss.params_.window_w/2;
  bottom->y0 = seed_y-intsciss.params_.window_h/2;
  bottom->x1 = seed_x+intsciss.params_.window_w/2;
  bottom->y1 = seed_y+intsciss.params_.window_h/2;
}



//-----------------------------------------------------------------------------
//: Make and display a dialog box to get Intelligent Scissors parameters.
//-----------------------------------------------------------------------------
bool dbdet_livewire_tool::get_intscissors_params(dbdet_lvwr_params* iparams, osl_canny_ox_params* params)
{
  vgui_dialog* intscissors_dialog = new vgui_dialog("Intelligent Scissors");
  intscissors_dialog->message("%%%%%% USAGE %%%%%%: Start by left clicking to select initial seed");
  intscissors_dialog->message("As mouse moves outside the rectangle, a new seed is selected automatically, and the rectangle is translated.");
  intscissors_dialog->message("To enforce a new seed, left click again in the rectangle.");
  intscissors_dialog->message("SHIFT + left click deletes the last 10 point portion of the contour.");
  intscissors_dialog->message("SHIFT + e activates edit mode. Select two points on the contour. An euler spiral completes the portion in between automatically.");
  intscissors_dialog->message("SHIFT + e in edit mode turns back to normal mode.");
  intscissors_dialog->message("SHIFT + s smoothes the current contour.");
  intscissors_dialog->message("right click (with or without SHIFT) saves the current contour to the specified file and restarts the tool.");

  //intscissors_dialog->field("Standard deviation (sigma)", params->sigma);
  //intscissors_dialog->field("Max smoothing kernel width", params->max_width);
  //intscissors_dialog->field("Gauss tail", params->gauss_tail);
  //intscissors_dialog->field("Low hysteresis threshold", params->low);
  //intscissors_dialog->field("High hysteresis threshold", params->high);
  //intscissors_dialog->field("Min edge pixel intensity", params->edge_min);
  //intscissors_dialog->field("Min pixels in curve", params->min_length);
  //intscissors_dialog->field("Border size", params->border_size);
  //intscissors_dialog->field("Scale", params->scale);
  //intscissors_dialog->field("Strategy", params->follow_strategy);
  //intscissors_dialog->checkbox("Enable pixel jumping?", params->join_flag);
  //intscissors_dialog->field("Junction option", params->junction_option);
  //intscissors_dialog->checkbox("Verbose?", params->verbose);
  
  //intscissors_dialog->field("Weight for Laplacian Zero-Crossing", iparams->weight_z);
  //intscissors_dialog->field("Weight for Gradient Magnitude", iparams->weight_g);
  //intscissors_dialog->field("Weight for Gradient Direction", iparams->weight_d);
  //intscissors_dialog->field("Sigma for Gaussian smoothing", iparams->gauss_sigma);
  intscissors_dialog->field("Window width for optimum paths' evaluation", iparams->window_w);
  intscissors_dialog->field("Window height for optimum paths' evaluation", iparams->window_h);
  //intscissors_dialog->field("Weight for Path Length penalty", iparams->weight_l);
  //intscissors_dialog->field("Normalization factor for path length", iparams->path_norm);
  intscissors_dialog->checkbox("Use canny", iparams->canny);
  intscissors_dialog->checkbox("Use image", iparams->use_given_image);

  intscissors_dialog->field("Weight for costs from canny edges", iparams->weight_canny);
  intscissors_dialog->field("Weight for path length on using canny", iparams->weight_canny_l);
  

  return intscissors_dialog->ask();
}


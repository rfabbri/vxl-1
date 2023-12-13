#include "mw_curve_tracing_tool_2_maro.h"
#include "mw_curve_tracing_tool_common_2.h"

#include <vnl/vnl_random.h>
#include <vil/algo/vil_colour_space.h>

#include <sdetd/pro/sdetd_sel_storage.h>
#include <vpgld/io/vpgld_io_cameras.h>
#include <bmcsd/algo/bmcsd_algo_util.h>

static double focal_length_;
static vnl_random g_myrand;


mw_curve_tracing_tool_2_maro::
mw_curve_tracing_tool_2_maro()
{
  focal_length_ = 11.; // working with a specific dataset
  init(new bmcsd_curve_stereo);
}

mw_curve_tracing_tool_2_maro::
mw_curve_tracing_tool_2_maro(bmcsd_curve_stereo *impl)
{
  init(impl);
}

void mw_curve_tracing_tool_2_maro::
init(bmcsd_curve_stereo *impl)
{
  s_ = impl;
  p0_ = 0; 
  pn_ = 0; 
  cc_style_ = vgui_style::new_style(0.2, 0.5, 1, 1.0f, 3.0f); 
  display_all_intersections_ = false; 
  display_all_right_epips_ = true; 
  display_all_left_epips_ = false; 
  display_all_nth_view_epips_ = false; 
  compute_isets_ = false;
  click_selects_whole_curve_ = true;

  ep_style_ = vgui_style::new_style();
  ep_style_->rgba[0] = 1;
  ep_style_->rgba[1] = 1;
  ep_style_->rgba[2] = 1;
  ep_style_->rgba[3] = 0.07;
  ep_style_->line_width = 1;
  ep_style_->point_size = 1;
  gesture0_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);

  best_match_style_.push_back(vgui_style::new_style(1, 1, 0, 1.0f, 3.0f));   // Yellow
  best_match_style_.push_back(vgui_style::new_style(1, 0.5, 0, 1.0f, 3.0f)); // Orange
  best_match_style_.push_back(vgui_style::new_style(1, 0, 1, 1.0f, 3.0f));   // Magenta
  best_match_style_.push_back(vgui_style::new_style(0, 1, 1, 1.0f, 3.0f));   // Green blue
  best_match_style_.push_back(vgui_style::new_style(0.7, 0.7, 0.7, 1.0f, 3.0f)); // Gray

  color_pn_.r = 1; color_pn_.g = 1; color_pn_.b = 0;
  color_p0_.r = 0; color_p0_.g = 1; color_p0_.b = 0.2;
  selected_crv_id_view0_ = MW_INVALID_CURVE_ID;
}


mw_curve_tracing_tool_2_maro::
~mw_curve_tracing_tool_2_maro()
{
  delete s_;
  s_ = NULL;
}

bool 
mw_curve_tracing_tool_2_maro::set_tableau( const vgui_tableau_sptr& tableau )
{
  curve_tableau_current_.vertical_cast(tableau);
  if(!curve_tableau_current_)  {
    std::cerr << "Warning: working in a tableau which is not expected\n";
    return false;
  }

  return true;
}

std::string
mw_curve_tracing_tool_2_maro::name() const
{
  return "Multiview Curve Tracing v.2 Maro Project";
}

void mw_curve_tracing_tool_2_maro::
activate()
{
  assert(s_);
  std::cout << "mw_curve_tracing_tool_2_maro ON\n";

  std::vector< bvis1_view_tableau_sptr > views;
  views = MANAGER->get_views();

  if ( !s_->set_nviews(views.size()) ) {
    vgui::out << "Error setting the number of views\n";
    std::cerr <<  "Error setting the number of views\n";
    return;
  }
  sels_.reserve(nviews());
  tab_.resize(nviews());
  ep0_soview_.resize(nviews());
  epn_soview_.resize(nviews());
  ep_soviews_.resize(nviews()-1);
  p_reproj_soviews_.resize(nviews());
  selected_crv_soviews_n_.resize(nviews()-1,0);
  ep0_soview_2n_.resize(nviews()-2);
  epn_soview_2n_.resize(nviews()-2);
  ep_soviews_2n_.resize(nviews()-2);
  reproj_soview_.resize(nviews()-2);
  frame_v_.resize(nviews());

  std::cout << "Working in frames ";
  for (unsigned i=0; i<nviews(); ++i) {
    frame_v_[i] = views[i]->frame();
    std::cout << frame_v(i) << "  ";
  }
  std::cout << std::endl;

  // -------- Get camera matrices from each frame

  get_cameras();
  get_curves(views);
  init_tableaux(views);
  draw_marked_points();
}

void   
mw_curve_tracing_tool_2_maro::
deactivate ()
{
  std::cout << "mw_curve_tracing_tool_2_maro OFF\n";
}

bool
mw_curve_tracing_tool_2_maro::
handle( const vgui_event & e, 
        const bvis1_view_tableau_sptr& view )
{
  if (!tab_[0])
    return false;

  if (gesture0_(e))
    return handle_mouse_click(e,view);
  // else nothing selected

  if (e.type == vgui_KEY_PRESS && p0_) {
    switch (e.key) {
      case 's': {
          swap_p0_and_pn();

        return true;
        break;
      }
      case '.': { // advance pn outwards
        vgui::out << "Moving outwards\n";
        std::cout  << "Moving outwards\n";

        vsol_point_2d_sptr pt = s_->advance_endpoint();
        if (pt)
          update_pn(pt);

        return true;
        break;
      }
      case ',': { // advance pn inwards
        vgui::out << "Moving inwards\n";
        std::cout  << "Moving inwards\n";

        vsol_point_2d_sptr pt = s_->recede_endpoint();
        if (pt)
          update_pn(pt);

        return true;
        break;
      }

      case 'u': {
        vgui::out << "Equalizing matching subcurves\n";
        std::cout  << "Equalizing matching subcurves\n";

        equalize_matching_subcurve();
        return true;
        break;
      }

      case 'g': { 
        // toggle if should display all intersections of pencil ep0..ep1 and edges in image 2
        std::cout << "Toggling display intersections\n";
        vgui::out << "Toggling display intersections\n";

        display_all_intersections_ = !display_all_intersections_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        update_display_for_intersection_stuff();

        return true;
        break;
      }
      case 'i': {
        // toggle if should display+compute all intersections of pencil ep0..ep1 and edges in image 2
        std::cout << "Toggling compute intersections\n";
        vgui::out << "Toggling compute intersections\n";

        compute_isets_ = !compute_isets_;
        display_all_intersections_ = compute_isets_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        if (compute_isets_)
          draw_candidate_curves();
        update_display_for_intersection_stuff();

        return true;
        break;
      }
      case 'e': {
        // toggle if should display+compute all ep0..ep1 in image 2
        std::cout << "Toggling display epipolars in image 2\n";
        vgui::out << "Toggling display epipolars in image 2\n";

        display_all_right_epips_ = !display_all_right_epips_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        update_display_for_epipolar_curve_pencil();

        return true;
        break;
      }
      case 'w': {
        // toggle if should display+compute all ep0..ep1 in image 1
        std::cout << "Toggling display epipolars in image 1\n";
        vgui::out << "Toggling display epipolars in image 1\n";

        display_all_left_epips_ = !display_all_left_epips_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        update_display_for_epipolar_curve_pencil();

        return true;
        break;
      }
      case '4': {
        // toggle if should display+compute all ep0..ep1 in image 3
        std::cout << "Toggling display epipolars in image[v], v >= 2\n";
        vgui::out << "Toggling display epipolars in image[v], v >= 2\n";

        display_all_nth_view_epips_= !display_all_nth_view_epips_;
        //: if false, remove soviews; if true, add them (add_spatial_object)
        update_display_for_epipolar_curve_pencil();

        return true;
        break;
      }
      case 'y': {

        std::cout << "Reconstructing selected matches using all n views \n";
        vgui::out << "Reconstructing selected matches using all n views \n";
        s_->reconstruct_multiview();

        return true;
        break;
      }
      case '1': { 
        std::cout << "Breaking the curves into episegs\n";
        vgui::out << "Breaking the curves into episegs\n";
        break_curves_into_episegs();
        break;
      }
      case '2': { 
        std::string state;
        if (click_selects_whole_curve_) {
          click_selects_whole_curve_ = false;
          state = "off";
        } else {
          click_selects_whole_curve_ = true;
          state = "on";
        }
        std::cout << "Whole curve selection mode " << state << std::endl;;
        vgui::out << "Whole curve selection mode " << state << std::endl;;
        break;
      }
      case '>': { 

        increment_focalength(++focal_length_*1./11.);
        break;
      }
      case '<': { 
        increment_focalength(--focal_length_*1./11.);
      }

      default:
        std::cout << "Unassigned key: " << e.key << " pressed.\n";
        break;
    }
  }

  //  We are not interested in other events,
  //  so pass event to base class:
  return false;
}

static void get_corrs_old(
    unsigned ncams, 
    std::vector<vgl_point_2d<double> > *image_points_linearlist, 
    std::vector< std::vector<vgl_point_2d<double> > > *pimgpts_percam,
    std::vector<std::vector<bool> > *pmask)
{
  unsigned npts=17;
  std::vector< std::vector<vgl_point_2d<double> > > &imgpts_percam = *pimgpts_percam;
  std::vector<std::vector<bool> > &mask = *pmask;

  // make the mask (using all the points)
  mask = std::vector<std::vector<bool> > (ncams, std::vector<bool>(npts,true) );


  imgpts_percam.resize(ncams);

  imgpts_percam[0].resize(npts);
  imgpts_percam[0][0].set(891.0, 460.0);
  imgpts_percam[0][1].set(827.0, 550.0);
  imgpts_percam[0][2].set(826.0, 732.0);
  imgpts_percam[0][3].set(961.0, 734.0);
  imgpts_percam[0][4].set(961.0, 550.0);
  imgpts_percam[0][5].set(1026.0, 463.0);
  imgpts_percam[0][6].set(1059.0, 463.0);
  imgpts_percam[0][7].set(1043, 629);
  imgpts_percam[0][8].set(1063, 700);
  imgpts_percam[0][9].set(1025, 700);
  imgpts_percam[0][10].set(1042,659);
  imgpts_percam[0][11].set(1042,659);
  imgpts_percam[0][12].set(824,329);
  imgpts_percam[0][13].set(0,0);
  imgpts_percam[0][14].set(0,0);
  imgpts_percam[0][15].set(0,0);
  imgpts_percam[0][16].set(0,0);
  mask[0][13]=false;
  mask[0][14]=false;
  mask[0][15]=false;
  mask[0][16]=false;

  imgpts_percam[1].resize(npts);
  imgpts_percam[1][0].set(1011.5, 548.5);
  imgpts_percam[1][1].set(961.5, 635.2);
  imgpts_percam[1][2].set(958.5, 812.0);
  imgpts_percam[1][3].set(1048.5, 816.2);
  imgpts_percam[1][4].set(1051, 637);
  imgpts_percam[1][5].set(1100, 552);
  imgpts_percam[1][6].set(1125, 552);
  imgpts_percam[1][7].set(1111, 716);
  imgpts_percam[1][8].set(1126.5, 785);
  imgpts_percam[1][9].set(1101, 784);
  imgpts_percam[1][10].set(1130,751);
  imgpts_percam[1][11].set(1235,747.4);
  imgpts_percam[1][12].set(0,0);
  imgpts_percam[1][13].set(452.5,549.5);
  imgpts_percam[1][14].set(683.0,410);
  imgpts_percam[1][15].set(1100,744);
  imgpts_percam[1][16].set(1557,458);
  mask[1][12]=false;

  imgpts_percam[2].resize(npts);
  imgpts_percam[2][0].set(923, 81);
  imgpts_percam[2][1].set(854.5, 178.8);
  imgpts_percam[2][2].set(856, 373);
  imgpts_percam[2][3].set(1000, 372);
  imgpts_percam[2][4].set(999, 178);
  imgpts_percam[2][5].set(1070, 84);
  imgpts_percam[2][6].set(1102, 82);
  imgpts_percam[2][7].set(1087, 262);
  imgpts_percam[2][8].set(1108, 333);
  imgpts_percam[2][9].set(1067, 334);
  imgpts_percam[2][10].set(1083.5,297);
  imgpts_percam[2][11].set(1083.5,297);
  imgpts_percam[2][12].set(852,47);
  imgpts_percam[2][13].set(0,0);
  imgpts_percam[2][14].set(0,0);
  imgpts_percam[2][15].set(0,0);
  imgpts_percam[2][16].set(0,0);
  mask[2][13]=false;
  mask[2][14]=false;
  mask[2][15]=false;
  mask[2][16]=false;

  imgpts_percam[3].resize(npts);
  imgpts_percam[3][0].set(1069, 379);
  imgpts_percam[3][1].set(1018, 472.6);
  imgpts_percam[3][2].set(1018.5, 661);
  imgpts_percam[3][3].set(1115, 662);
  imgpts_percam[3][4].set(1114, 472);
  imgpts_percam[3][5].set(1165, 380);
  imgpts_percam[3][6].set(1191, 380);
  imgpts_percam[3][7].set(1179, 554);
  imgpts_percam[3][8].set(1194, 624);
  imgpts_percam[3][9].set(1168, 624);
  imgpts_percam[3][10].set(1201,589);
  imgpts_percam[3][11].set(1310,587);
  imgpts_percam[3][12].set(0,0);
  imgpts_percam[3][13].set(474.5,472.5);
  imgpts_percam[3][14].set(717.0,325.0);
  imgpts_percam[3][15].set(1158.0,674.0);
  imgpts_percam[3][16].set(1650.0,366.0);
  mask[3][12]=false;

  for (unsigned c = 0; c < ncams; ++c)
    for (unsigned pw=0; pw < npts; ++pw) {
      assert(imgpts_percam[c].size() == npts);
      if (mask[c][pw])
        image_points_linearlist->push_back(imgpts_percam[c][pw]);
    }
}

static void get_corrs(
    unsigned ncams, 
    std::vector<vgl_point_2d<double> > *image_points_linearlist, 
    std::vector< std::vector<vgl_point_2d<double> > > *pimgpts_percam,
    std::vector<std::vector<bool> > *pmask)
{
  unsigned npts=18;
  std::vector< std::vector<vgl_point_2d<double> > > &imgpts_percam = *pimgpts_percam;
  std::vector<std::vector<bool> > &mask = *pmask;

  // make the mask (using all the points)
  mask = std::vector<std::vector<bool> > (ncams, std::vector<bool>(npts,true) );


  imgpts_percam.resize(ncams);

  imgpts_percam[0].resize(npts);
  imgpts_percam[0][0].set(891.0, 460.0);
  imgpts_percam[0][1].set(827.0, 550.0);
  imgpts_percam[0][2].set(826.0, 732.0);
  imgpts_percam[0][3].set(961.0, 734.0);
  imgpts_percam[0][4].set(961.0, 550.0);
  imgpts_percam[0][5].set(1026.0, 463.0);
  imgpts_percam[0][6].set(1059.0, 463.0);
  imgpts_percam[0][7].set(1043, 629);
  imgpts_percam[0][8].set(1063, 700);
  imgpts_percam[0][9].set(1025, 700);
  imgpts_percam[0][10].set(1042,659);
  imgpts_percam[0][11].set(1042,659);
  imgpts_percam[0][12].set(824,329);
  imgpts_percam[0][13].set(0,0);
  imgpts_percam[0][14].set(0,0);
  imgpts_percam[0][15].set(0,0);
  imgpts_percam[0][16].set(0,0);
  imgpts_percam[0][17].set(0,0);
  mask[0][13]=false;
  mask[0][14]=false;
  mask[0][15]=false;
  mask[0][16]=false;
  mask[0][17]=false;

  imgpts_percam[1].resize(npts);
  imgpts_percam[1][0].set(1011.5, 548.5);
  imgpts_percam[1][1].set(961.5, 635.2);
  imgpts_percam[1][2].set(958.5, 812.0);
  imgpts_percam[1][3].set(1048.5, 816.2);
  imgpts_percam[1][4].set(1051, 637);
  imgpts_percam[1][5].set(1100, 552);
  imgpts_percam[1][6].set(1125, 552);
  imgpts_percam[1][7].set(1111, 716);
  imgpts_percam[1][8].set(1126.5, 785);
  imgpts_percam[1][9].set(1101, 784);
  imgpts_percam[1][10].set(1130,751);
  imgpts_percam[1][11].set(1235,747.4);
  imgpts_percam[1][12].set(0,0);
  imgpts_percam[1][13].set(452.5,549.5);
  imgpts_percam[1][14].set(683.0,410);
  imgpts_percam[1][15].set(1100,744);
  imgpts_percam[1][16].set(1557,458);
  imgpts_percam[1][17].set(1447,378);
  mask[1][12]=false;

  imgpts_percam[2].resize(npts);
  imgpts_percam[2][0].set(923, 81);
  imgpts_percam[2][1].set(854.5, 178.8);
  imgpts_percam[2][2].set(856, 373);
  imgpts_percam[2][3].set(1000, 372);
  imgpts_percam[2][4].set(999, 178);
  imgpts_percam[2][5].set(1070, 84);
  imgpts_percam[2][6].set(1102, 82);
  imgpts_percam[2][7].set(1087, 262);
  imgpts_percam[2][8].set(1108, 333);
  imgpts_percam[2][9].set(1067, 334);
  imgpts_percam[2][10].set(1083.5,297);
  imgpts_percam[2][11].set(1083.5,297);
  imgpts_percam[2][12].set(852,47);
  imgpts_percam[2][13].set(0,0);
  imgpts_percam[2][14].set(0,0);
  imgpts_percam[2][15].set(0,0);
  imgpts_percam[2][16].set(0,0);
  imgpts_percam[2][17].set(0,0);
  mask[2][13]=false;
  mask[2][14]=false;
  mask[2][15]=false;
  mask[2][16]=false;
  mask[2][17]=false;

  imgpts_percam[3].resize(npts);
  imgpts_percam[3][0].set(1069, 379);
  imgpts_percam[3][1].set(1018, 472.6);
  imgpts_percam[3][2].set(1018.5, 661);
  imgpts_percam[3][3].set(1115, 662);
  imgpts_percam[3][4].set(1114, 472);
  imgpts_percam[3][5].set(1165, 380);
  imgpts_percam[3][6].set(1191, 380);
  imgpts_percam[3][7].set(1179, 554);
  imgpts_percam[3][8].set(1194, 624);
  imgpts_percam[3][9].set(1168, 624);
  imgpts_percam[3][10].set(1201,589);
  imgpts_percam[3][11].set(1310,587);
  imgpts_percam[3][12].set(0,0);
  imgpts_percam[3][13].set(474.5,472.5);
  imgpts_percam[3][14].set(717.0,325.0);
  imgpts_percam[3][15].set(1158.0,674.0);
  imgpts_percam[3][16].set(1650.0,366.0);
  imgpts_percam[3][17].set(1525,282);
  mask[3][12]=false;

  for (unsigned c = 0; c < ncams; ++c)
    for (unsigned pw=0; pw < npts; ++pw) {
      assert(imgpts_percam[c].size() == npts);
      if (mask[c][pw])
        image_points_linearlist->push_back(imgpts_percam[c][pw]);
    }
}

static void initialize_maro_world(
    const std::vector<std::vector<vgl_point_2d<double> > > &imgpts,
    std::vector<vgl_point_3d<double> > *world)
{
  // assume all points show up in all cams
  unsigned npts = imgpts[0].size();
  *world = std::vector<vgl_point_3d<double> >(npts,vgl_point_3d<double>(7475.,   7500., 360.));
}

static void get_random_colors(
    unsigned npts, 
    std::vector<double> &rv,
    std::vector<double> &gv,
    std::vector<double> &bv)
{
  std::vector<double> hv;
  for (unsigned p=0; p < npts; ++p) {
    bool found_hue=false;
    // randomize hue
    unsigned nruns=0;
    double h;
    do {
      nruns++;
      h = g_myrand.drand64(0,360);

      for (unsigned i=0; i < hv.size(); ++i) {
        if (abs((int)h - (int)hv[i]) < 15)
          continue;
        found_hue=true;
      }
    } while (!found_hue && nruns < 100);

    double v = (nruns >= 100)? 0.6 : 1;

    double r;
    double g;
    double b;

    vil_colour_space_HSV_to_RGB<double>(h,v,180,&r, &g,&b);
    r/=255.0;
    g/=255.0;
    b/=255.0;

    rv.push_back(r);
    gv.push_back(g);
    bv.push_back(b);
    hv.push_back(h);
  }
}


void mw_curve_tracing_tool_2_maro::
draw_marked_points()
{
  std::vector<vgl_point_2d<double> > imgpts_linearlist; 
  std::vector< std::vector<vgl_point_2d<double> > > imgpts_percam;
  std::vector<std::vector<bool> > mask;
  
  get_corrs(nviews(), &imgpts_linearlist, &imgpts_percam, &mask);

  // Show all initial 2D points in random colors

  std::vector<double> r;
  std::vector<double> g;
  std::vector<double> b;

  get_random_colors(imgpts_percam[0].size(), r, g, b);
    
  for (unsigned v=0; v < nviews(); ++v) {
    tab_[v]->set_current_grouping( "Drawing" );
    tab_[v]->set_point_radius(13);

    for (unsigned p=0; p < imgpts_percam[v].size(); ++p) {
      tab_[v]->set_foreground(r[p], g[p], b[p]);
      if (mask[v][p])
        tab_[v]->add_point(imgpts_percam[v][p].x(), imgpts_percam[v][p].y());
    }
  }

  std::vector<vgl_point_3d<double> > ini_world;
  initialize_maro_world(imgpts_percam, &ini_world);

  // Project initial 3D points and show
  for (unsigned v=0; v < nviews(); ++v) {
    tab_[v]->set_current_grouping( "Drawing" );
    for (unsigned p=0; p < ini_world.size(); ++p) {
      tab_[v]->set_point_radius(8);
      tab_[v]->set_foreground(0.3, 0.8, 1);
      vgl_point_2d<double> pprj = s_->cams(v).Pr_(vgl_homg_point_3d<double>(ini_world[p]));
      tab_[v]->add_point(pprj.x(), pprj.y());

      tab_[v]->set_point_radius(50); // huge
      vgl_point_2d<double> oprj = s_->cams(v).Pr_(vgl_homg_point_3d<double>(0,0,0));
      tab_[v]->set_foreground(1, 1, 1);
      tab_[v]->add_point(oprj.x(), oprj.y());
    }
  }

  // Read in points file if any, and show in smaller color

  std::vector<vgl_point_3d<double> > pts3d;

  // optinally read 3D points, if exist
  if(myreadv("./pts3d-optimized.dat", pts3d)) {
    std::cout << "Read " << pts3d.size() << " points" << std::endl;
    for (unsigned p=0; p < pts3d.size(); ++p) {
      std::cout << "Point " << p << ": " << pts3d[p] << std::endl;
    }
    assert(pts3d.size() == ini_world.size() && pts3d.size() == r.size());
  }

  for (unsigned v=0; v < nviews(); ++v) {
    tab_[v]->set_current_grouping( "Drawing" );
    tab_[v]->set_point_radius(6);
    for (unsigned p=0; p < pts3d.size(); ++p) {
      tab_[v]->set_foreground(r[p]+0.5, g[p]+0.5, b[p]+0.5);
      vgl_point_2d<double> pprj = s_->cams(v).Pr_(vgl_homg_point_3d<double>(pts3d[p]));
      std::cout << "Projecting with camera " << s_->cams(v).Pr_ << std::endl
        << "Result " << pprj << std::endl;
      tab_[v]->add_point(pprj.x(), pprj.y());
    }
    tab_[v]->post_redraw();
  }
}

void mw_curve_tracing_tool_2_maro::
initialize_curve_selection()
{
  if (p0_) { //: if there's a previously selected curve segment, erase it 
     tab_[0]->remove(p0_);
     tab_[0]->remove(pn_);
     tab_[0]->remove(curvelet_soview_);
     for (unsigned k=1; k< nviews(); ++k) {
       tab_[k]->remove(ep0_soview_[k]);
       tab_[k]->remove(epn_soview_[k]);
     }
  }
  for (unsigned i=0 ; i < nviews(); ++i)
    tab_[i]->set_current_grouping( "Drawing" );

  curvelet_soview_ = tab_[0]->add_vsol_polyline_2d(s_->subcurve(), cc_style_);

  tab_[0]->set_point_radius(8);
  // Initially p0 and pn are equal
  tab_[0]->set_foreground(color_pn_.r, color_pn_.g, color_pn_.b);
  pn_ = tab_[0]->add_point(s_->subcurve()->vertex(0)->x(), s_->subcurve()->vertex(0)->y());
  tab_[0]->set_foreground(color_p0_.r, color_p0_.g, color_p0_.b);
  p0_ = tab_[0]->add_point(s_->subcurve()->vertex(0)->x(), s_->subcurve()->vertex(0)->y());

  // Add epipolar lines to tab_[1]

  tab_[1]->set_foreground(color_pn_.r, color_pn_.g, color_pn_.b);
  epn_soview_[1] = tab_[1]->add_infinite_line(
      s_->ep_end(0).a(), s_->ep_end(0).b(), s_->ep_end(0).c());
  tab_[1]->set_foreground(color_p0_.r, color_p0_.g, color_p0_.b);
  ep0_soview_[1] = tab_[1]->add_infinite_line(
      s_->ep_ini(0).a(), s_->ep_ini(0).b(), s_->ep_ini(0).c());

  // Add epipolar lines to tab_[v], v >= 2
  for (unsigned v=2; v < nviews(); ++v) {
    tab_[v]->set_foreground(color_pn_.r, color_pn_.g, color_pn_.b);
    epn_soview_[v] = tab_[v]->add_infinite_line(
        s_->ep_end(v-1).a(), s_->ep_end(v-1).b(), s_->ep_end(v-1).c());
    tab_[v]->set_foreground(color_p0_.r, color_p0_.g, color_p0_.b);
    ep0_soview_[v] = tab_[v]->add_infinite_line(
        s_->ep_ini(v-1).a(), s_->ep_ini(v-1).b(), s_->ep_ini(v-1).c());

    draw_candidate_curves();

    update_display_for_epipolar_curve_pencil();
    update_display_for_intersection_stuff();
    tab_[v]->post_redraw();
  }
}

void mw_curve_tracing_tool_2_maro::
update_pn(const vsol_point_2d_sptr &pt)
{
  // ---------------- ONLY GUI OPERATIONS -------------------
  for (unsigned i=0 ; i < nviews(); ++i) 
    tab_[i]->set_current_grouping( "Drawing" );
  
  tab_[0]->remove(curvelet_soview_);
  curvelet_soview_ = tab_[0]->add_vsol_polyline_2d(s_->subcurve(), cc_style_);

  tab_[0]->set_point_radius(8);
  tab_[0]->set_foreground(color_pn_.r, color_pn_.g, color_pn_.b);
  tab_[0]->remove(pn_);
  pn_ = tab_[0]->add_point(pt->x(), pt->y());
  tab_[1]->set_foreground(color_pn_.r, color_pn_.g, color_pn_.b);
  tab_[1]->remove(epn_soview_[1]);
  epn_soview_[1] = tab_[1]->add_infinite_line(
      s_->ep_end(0).a(),s_->ep_end(0).b(),s_->ep_end(0).c());

  for (unsigned v=2; v < nviews(); ++v) {
    tab_[v]->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
    tab_[v]->remove(epn_soview_[v]);
    epn_soview_[v] = tab_[v]->add_infinite_line(
        s_->ep_end(v-1).a(),s_->ep_end(v-1).b(),s_->ep_end(v-1).c());
  }

  draw_candidate_curves();
  update_display_for_epipolar_curve_pencil();

  update_display_for_intersection_stuff();

  std::list<bgui_vsol_soview2D_polyline *>::const_iterator itr;
  for (itr = crv_best_matches_soviews_.begin(); itr != crv_best_matches_soviews_.end(); ++itr)
    tab_[1]->remove(*itr);
  crv_best_matches_soviews_.clear();

  for (unsigned i=0 ; i < nviews(); ++i) 
    tab_[i]->post_redraw();
}

void mw_curve_tracing_tool_2_maro::
update_display_for_intersection_stuff()
{
  tab_[1]->set_current_grouping( "Drawing" );

  if (display_all_intersections_) {
    show_all_intersection_points();
  } else { //: remove soviews from tableau
    for (unsigned long i=0; i < all_intercept_pts_soviews_.size(); ++i)
      tab_[1]->remove(all_intercept_pts_soviews_[i]);
    all_intercept_pts_soviews_.clear();
  }

  tab_[1]->post_redraw();
  tab_[1]->set_current_grouping( "default" );
}

void mw_curve_tracing_tool_2_maro::
update_display_for_epipolar_curve_pencil()
{
  for (unsigned i=0 ; i < nviews(); ++i)
    tab_[i]->set_current_grouping( "Drawing" );

  if (display_all_right_epips_) {

    std::list<vgui_soview2D_infinite_line *>::const_iterator itr;
    for (itr = ep_soviews_[0].begin(); itr != ep_soviews_[0].end(); ++itr) {
      tab_[1]->remove(*itr);
    }
    ep_soviews_[0].clear();

    for (unsigned i=0; i < s_->ep(0).size(); ++i) {
      ep_soviews_[0].push_back(tab_[1]->add_infinite_line(s_->ep(0)[i].a(),s_->ep(0)[i].b(),s_->ep(0)[i].c() ));
      ep_soviews_[0].back()->set_style(ep_style_);
    }
  } else { //: remove soviews from tableau
    std::list<vgui_soview2D_infinite_line *>::const_iterator itr;
    for (itr = ep_soviews_[0].begin(); itr != ep_soviews_[0].end(); ++itr) {
      tab_[1]->remove(*itr);
    }
    ep_soviews_[0].clear();
  }

  tab_[1]->post_redraw();

  // LEFT
  if (display_all_left_epips_) {
    std::list<vgui_soview2D_infinite_line *>::const_iterator itr;
    for (itr = ep_soviews_left_.begin(); itr != ep_soviews_left_.end(); ++itr)
      tab_[0]->remove(*itr);
    ep_soviews_left_.clear();

    for (unsigned i=0; i < s_->ep(0).size(); ++i) {
      ep_soviews_left_.push_back(tab_[0]->add_infinite_line(
        s_->ep_left(i).a(), s_->ep_left(i).b(), s_->ep_left(i).c() ));
      ep_soviews_left_.back()->set_style(ep_style_);
    }
  } else { //: remove soviews from tableau
    std::list<vgui_soview2D_infinite_line *>::const_iterator itr;
    for (itr = ep_soviews_left_.begin(); itr != ep_soviews_left_.end(); ++itr)
      tab_[0]->remove(*itr);
    ep_soviews_left_.clear();
  }
  tab_[0]->post_redraw();

  // 3rd view and up

  if (display_all_nth_view_epips_) {
    for (unsigned v=2; v < nviews(); ++v) {
      std::list<vgui_soview2D_infinite_line *>::const_iterator itr;
      for (itr = ep_soviews_[v-1].begin(); itr != ep_soviews_[v-1].end(); ++itr)
        tab_[v]->remove(*itr);
      ep_soviews_[v-1].clear();

      for (unsigned i=0; i < s_->ep(v-1).size(); ++i) {
        ep_soviews_[v-1].push_back(tab_[v]->add_infinite_line(
              s_->ep(v-1)[i].a(), s_->ep(v-1)[i].b(), s_->ep(v-1)[i].c() ));
        ep_soviews_[v-1].back()->set_style(ep_style_);
      }
    }
  } else { //: remove soviews from tableau
    for (unsigned v=2; v < nviews(); ++v) {
      std::list<vgui_soview2D_infinite_line *>::const_iterator itr;
      for (itr = ep_soviews_[v-1].begin(); itr != ep_soviews_[v-1].end(); ++itr)
        tab_[v]->remove(*itr);
      ep_soviews_[v-1].clear();
    }
  }

  for (unsigned v=2; v < nviews(); ++v)
    tab_[v]->post_redraw();
}

void mw_curve_tracing_tool_2_maro::
show_all_intersection_points()
{

  for (unsigned long i=0; i < all_intercept_pts_soviews_.size(); ++i)
    tab_[1]->remove(all_intercept_pts_soviews_[i]);
  all_intercept_pts_soviews_.clear();

  tab_[1]->set_point_radius(3);
  tab_[1]->set_foreground(0,0.5,0);

  std::vector<std::vector<unsigned> > cand_pt_id;
  s_->get_candidate_intercepts(&cand_pt_id);

  for (unsigned ic=0; ic < cand_pt_id.size(); ++ic)
    for (unsigned p=0; p < cand_pt_id[ic].size(); ++p) {
      const vsol_point_2d &pt = *s_->crv_candidates_ptrs(ic)->vertex(cand_pt_id[ic][p]);
      all_intercept_pts_soviews_.push_back( tab_[1]->add_point(pt.x(),pt.y()) );
    }
}

void mw_curve_tracing_tool_2_maro::
swap_p0_and_pn()
{
  s_->swap_endpoints();

  vgui::out << "Active point changed\n";
  vgui_soview2D_point *aux; 
  aux = p0_; p0_ = pn_; pn_ = aux;
  color_aux_ = color_p0_;
  color_p0_  = color_pn_;
  color_pn_  = color_aux_;

  for (unsigned i=0; i < nviews(); ++i) {
    vgui_soview2D_infinite_line *ep_soview_tmp_;
    ep_soview_tmp_ = ep0_soview_[i];
    ep0_soview_[i] = epn_soview_[i];
    epn_soview_[i] = ep_soview_tmp_;
  }
}

void mw_curve_tracing_tool_2_maro::
draw_candidate_curves()
{
  s_->compute_epipolar_beam_candidates();
    
  // -------------- GUI -----------------------
  std::list<bgui_vsol_soview2D_polyline *>::const_iterator citer; 
  for (citer = crv_candidates_soviews_.begin(); 
       citer!= crv_candidates_soviews_.end(); ++citer)
    tab_[1]->remove(*citer);
  crv_candidates_soviews_.clear();

  for (unsigned j=0; j < s_->num_candidates(); ++j)
    crv_candidates_soviews_.push_back( 
        tab_[1]->add_vsol_polyline_2d(s_->crv_candidates_ptrs(j), cc_style_));

  std::cout << "Number of curves intercepting pencil: " << s_->num_candidates() << std::endl;
}

void mw_curve_tracing_tool_2_maro::
get_cameras()
{
  std::vector<bdifd_camera> cams(nviews());

  for (unsigned i=0; i<nviews(); ++i) {
    bpro1_storage_sptr 
      p = MANAGER->repository()->get_data_at("vpgl camera",frame_v_[i]);

    vpgld_camera_storage_sptr cam_storage;

    cam_storage.vertical_cast(p);
    if(!p) {
      std::cerr << "Error: tool requires a vpgl camera storage" << std::endl;
      return;
    }

    const vpgl_perspective_camera<double> *pcam;

    pcam = vpgld_cast_to_perspective_camera(cam_storage->get_camera());
    if(!pcam) {
      std::cerr << "Error: tool requires a perspective camera" << std::endl;
      return;
    }

    cams[i].set_p(*pcam);

    std::cout << "NAME: " << cam_storage->name() << std::endl;
    std::cout << "Camera " << i << " : \n" << cams[i].Pr_.get_matrix();
  }

  s_->set_cams(cams);
}

void mw_curve_tracing_tool_2_maro::
init_tableaux(std::vector< bvis1_view_tableau_sptr > &views)
{
  // Get two active tableaus (original edge vsols), one from each view
  for (unsigned i=0 ; i < nviews(); ++i) {
    vgui_tableau_sptr tab_ptr = views[i]->selector()->active_tableau();

    if (tab_ptr) {
      bgui_vsol2D_tableau_sptr curve_tab;
      curve_tab.vertical_cast(tab_ptr);
      if (!curve_tab) {
        std::cerr << "error: no active vsol in left tableau\n";
        return;
      }
    } else {
      std::cerr << "error: Could not find active child tableaus in selector; line" << __LINE__ << std::endl;
      return ;
    }
  }


  // -------- Add tableaus to draw on
 
  std::string type("vsol2D");
  std::string name("mw_curve_tracer");

  for (unsigned i=0 ; i < nviews(); ++i) {
    bpro1_storage_sptr 
      n_data  = MANAGER->repository()->new_data_at(type,name,frame_v_[i]);

    if (n_data) {
       MANAGER->add_to_display(n_data);
    } else {
       std::cerr << "error: unable to register new data\n";
       return ;
    }
  }

  MANAGER->display_current_frame();

  for (unsigned i=0 ; i < nviews(); ++i) {
    vgui_tableau_sptr tab_ptr1 = views[i]->selector()->get_tableau(name);
    if (tab_ptr1) {
      tab_[i].vertical_cast(tab_ptr1);
    } else {
      std::cerr << "error: Could not find child tableaus in selector\n";
      return ;
    }

    std::string active;
    active = views[i]->selector()->active_name();

    views[i]->selector()->set_active(name);
    views[i]->selector()->active_to_top();
    views[i]->selector()->set_active(active);
  }
}

void mw_curve_tracing_tool_2_maro::
get_curves(std::vector< bvis1_view_tableau_sptr > &views)
{
  std::vector<std::vector< vsol_polyline_2d_sptr > > curves (nviews()); 

  for (unsigned v=0 ; v < nviews(); ++v)
    if (!get_vsols(views[v], v, &curves[v]))
      if (!get_sels(views[v], &curves[v]))
        std::cerr << "Error: Neither vsols or sels are active in the current view." << std::endl;

  s_->set_curves(curves);
}

bool  mw_curve_tracing_tool_2_maro::
get_sels(
    const bvis1_view_tableau_sptr &view, 
    std::vector< vsol_polyline_2d_sptr > *pcurves)
{
  std::vector< vsol_polyline_2d_sptr > &curves = *pcurves;

  bpro1_storage_sptr 
      p = MANAGER->storage_from_tableau(view->selector()->active_tableau());

  sdetd_sel_storage_sptr sel_storage = NULL;
  sel_storage.vertical_cast(p);
  if(!sel_storage)
    return false;
  sels_.push_back(sel_storage);

  // Now extract the curve fragments and put in the curve std::vector.

  if (sels_.back()->CFG().frags.empty())
    return false;

  sdet_edgel_chain_list &frags = sels_.back()->CFG().frags;
  curves.reserve(frags.size());

  for (sdet_edgel_chain_list_iter f_it = frags.begin(); 
      f_it != frags.end(); ++f_it) {

    std::vector<vsol_point_2d_sptr> pts;
    bmcsd_algo_util::sdet_to_vsol((*f_it)->edgels, &pts);
    curves.push_back(new vsol_polyline_2d(pts));
  }

  return true;
}

bool mw_curve_tracing_tool_2_maro::
get_vsols(
    const bvis1_view_tableau_sptr &view, 
    unsigned v,
    std::vector< vsol_polyline_2d_sptr > *pcurves) const
{
  std::vector< vsol_polyline_2d_sptr > &curves = *pcurves;
  vidpro1_vsol2D_storage_sptr sto = 0;

  sto.vertical_cast(MANAGER->storage_from_tableau(view->selector()->active_tableau()));
  if (!sto) {
    std::cerr << "Warning: Could not find an active vsol, which is ok if you have SEL.\n";
    return false;
  }

  std::vector< vsol_spatial_object_2d_sptr > base = sto->all_data ();

  std::cout << "Number of vsols in storage named " << sto->name() <<  " in view index " << v+1 
    << ": " << base.size() << std::endl;

  curves.resize(base.size(),0);

  // Cast everything to polyline

  for (unsigned i=0; i<curves.size(); ++i) {
    curves[i] = dynamic_cast<vsol_polyline_2d *> (base[i].ptr());

    if (!curves[i]) {
      std::cout << "Non-polyline found active in 2nd frame; but only POLYLINES supported!" << std::endl;
      return false;
    }
  }
  return true;
}

void mw_curve_tracing_tool_2_maro::
clear_previous_selections() {
  // Clear previous selections
  if (reproj_soview_.front()) {
    for (unsigned k=0; k+2 < nviews(); ++k) {

      tab_[k+2]->remove(reproj_soview_[k]);
      tab_[k+2]->remove(ep0_soview_2n_[k]);
      tab_[k+2]->remove(epn_soview_2n_[k]);
      tab_[k+2]->post_redraw();
      reproj_soview_[k] = 0;
      
      std::list<vgui_soview2D_infinite_line *>::const_iterator itr;
      for (itr = ep_soviews_2n_[k].begin(); 
          itr != ep_soviews_2n_[k].end(); ++itr)
        tab_[k+2]->remove(*itr);
      ep_soviews_2n_[k].clear();
    }

      
    for (unsigned k=0; k < nviews(); ++k) {
      for (unsigned i=0; i < p_reproj_soviews_[k].size(); ++i)
        tab_[k]->remove(p_reproj_soviews_[k][i]);
      p_reproj_soviews_[k].clear();
    }
  }
}

bool mw_curve_tracing_tool_2_maro::
handle_mouse_click( 
    const vgui_event & e, 
    const bvis1_view_tableau_sptr& view)
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  ix = MW_ROUND(ix); iy = MW_ROUND(iy);

  clear_previous_selections();

  // Curve selection

  vgui_soview2D* selected_curve_soview =  
    (vgui_soview2D*)curve_tableau_current_->get_highlighted_soview();

  std::cout << "Frame index: " << view->frame() << std::endl;

  bgui_vsol_soview2D_polyline *selected_curve_soview_poly
    = dynamic_cast<bgui_vsol_soview2D_polyline *>(selected_curve_soview); 

  if (!selected_curve_soview_poly) {
    std::cout << "Selected non-Polyline object" << std::endl;
    return false;
  }

  vsol_polyline_2d_sptr scrv = selected_curve_soview_poly->sptr();
  std::cout << "Size of selected curve: " << scrv->size() << std::endl;


  if (view->frame() == frame_v(0)) {
    s_->set_selected_crv(0, scrv);

    if (selected_crv_id_view0() != curve_tableau_current_->get_highlighted()) {
      selected_new_curve_ = true;
      set_selected_crv_id_view0(curve_tableau_current_->get_highlighted());
    } else
      selected_new_curve_ = false;

    if (click_selects_whole_curve_) {
        std::cout << "Selecting whole curve.\n";

        s_->initialize_subcurve(0);
        initialize_curve_selection();
        s_->update_endpoint(s_->selected_crv(0)->size()-1);
        update_pn(s_->selected_crv(0)->p1());
    
    }  else { // ! click_selects_whole_curve_
      if (selected_new_curve_) {
        std::cout << "New curve selected.\n";

        // mark clicked point
        vsol_point_2d_sptr pt = new vsol_point_2d(ix,iy);
        unsigned mindist;
        unsigned near_id = bmcsd_util::find_nearest_pt(pt, s_->selected_crv(0), mindist);

        s_->initialize_subcurve(near_id);
        initialize_curve_selection();

      } else { // State C: p0 and pn selected; one of them is active

        vsol_point_2d_sptr pt = new vsol_point_2d(ix,iy);
        unsigned mindist;
        unsigned near_id = bmcsd_util::find_nearest_pt(pt, s_->selected_crv(0), mindist);
        s_->update_endpoint(near_id);
        pt = s_->selected_crv(0)->vertex(near_id);
        update_pn(pt);
      }
    } // endif ! click_selects_whole_curve_
  } else { 
    if (view->frame() == frame_v(1)) {
      std::cout << "Processing a curve in the second view" << std::endl;

      unsigned jnz;
      bool stat = s_->get_index_of_candidate_curve(scrv, &jnz);

      if (stat) {
        std::cout << "You selected curve[" << jnz << "] among " 
                 << s_->num_candidates() << " epipolar candidates." << std::endl;

        show_reprojections(jnz);
      } else
        std::cout << "Selected soview not found in list of candidate vsols.\n";

      s_->set_selected_crv(1, scrv);

      const unsigned v = 1;
      if (selected_crv_soviews_n_[v-1])
        tab_[v]->remove(selected_crv_soviews_n_[v-1]);

      selected_crv_soviews_n_[v-1]
        = tab_[v]->add_vsol_polyline_2d(s_->selected_crv(v), best_match_style_[3]); 
      // just any color, whatever
      
      tab_[v]->post_redraw();

    } else { // (view >= 2)

      const int v = view_from_frame_index(view->frame());

      s_->set_selected_crv(v, scrv);

      std::cout << "Processing a curve in view[v], v = " << v << std::endl;

      if (selected_crv_soviews_n_[v-1])
        tab_[v]->remove(selected_crv_soviews_n_[v-1]);

      selected_crv_soviews_n_[v-1]
        = tab_[v]->add_vsol_polyline_2d(s_->selected_crv(v), best_match_style_[3]); // just any color, whatever
      tab_[v]->post_redraw();
    }
  }
  return true;
}

//: Given selected curves in the first two views, reprojects the underlying 3D
// curve in all other views and displays it.
//
//\param[in] crv2_id : index into crv_candidates_ptrs_ of selected candidate curve in 2nd
// view
void mw_curve_tracing_tool_2_maro::
show_reprojections(unsigned crv2_id)
{
  //: reproj[view_id][pt_id]
  std::vector< std::vector<vsol_point_2d_sptr> > reproj; 

  s_->reproject_in_all_views(crv2_id, &reproj);

  // ---------- GUI -----------

  for (unsigned i=0; i < nviews(); ++i)
    tab_[i]->set_current_grouping( "Drawing" );

  for (unsigned v=2; v < nviews(); ++v) {
    // Add a soview with the reprojection. 
    vsol_polyline_2d_sptr reproj_poly = new vsol_polyline_2d(reproj[v]);
    vgui_style_sptr cc_style_pumped = vgui_style::new_style(0.2, 0.5, 1, 1.0f, 5.0f); 
    reproj_soview_[v-2] = tab_[v]->add_vsol_polyline_2d(reproj_poly, cc_style_pumped);

    // soview for epip. lines for reprojection: endpts
    tab_[v]->set_foreground(color_pn_.r,color_pn_.g,color_pn_.b);
    epn_soview_2n_[v-2] = tab_[v]->add_infinite_line(
        s_->ep_v1_end(v-2).a(),s_->ep_v1_end(v-2).b(),s_->ep_v1_end(v-2).c());

    tab_[v]->set_foreground(color_p0_.r,color_p0_.g,color_p0_.b);
    ep0_soview_2n_[v-2] = tab_[v]->add_infinite_line(
        s_->ep_v1_ini(v-2).a(),s_->ep_v1_ini(v-2).b(),s_->ep_v1_ini(v-2).c());
  }

  // epip. lines for reprojection: all other points
  if (display_all_nth_view_epips_) {
    ep_soviews_2n_.resize(nviews()-2);
    for (unsigned v=0; v+2 < nviews(); ++v) {
      // II: Soviews
      std::list<vgui_soview2D_infinite_line *>::const_iterator itr;
      for (itr = ep_soviews_2n_[v].begin(); itr != ep_soviews_2n_[v].end(); ++itr)
        tab_[v+2]->remove(*itr);
      ep_soviews_2n_[v].clear();

      for (unsigned i=0; i < s_->ep_v1(v).size(); ++i) {
        ep_soviews_2n_[v].push_back(
          tab_[v+2]->add_infinite_line(s_->ep_v1(v)[i].a(),s_->ep_v1(v)[i].b(),s_->ep_v1(v)[i].c() ));
        ep_soviews_2n_[v].back()->set_style(ep_style_);
      }
    }
  }
    
  for (unsigned i=0; i < nviews(); ++i)
    tab_[i]->post_redraw();
}

//: Break curves into epipolar tangent segments.
void mw_curve_tracing_tool_2_maro::
break_curves_into_episegs()
{
  std::vector<std::vector< vsol_polyline_2d_sptr > > broken_vsols;
  {
    std::vector<bbld_subsequence_set> sseq;
    s_->break_curves_into_episegs_pairwise(&broken_vsols, &sseq);
  }
  assert(broken_vsols.size() == nviews());

  for (unsigned v=0; v < nviews(); ++v) {
    // Create vsol storage; add to repository; make it active
    std::vector<vsol_spatial_object_2d_sptr> base_vsols2;
    base_vsols2.resize(broken_vsols[v].size());
    for (unsigned i=0; i< broken_vsols[v].size(); ++i)
      base_vsols2[i] = broken_vsols[v][i].ptr();

    // output {broken_vsols}i with the smallest non-existent i
    
    std::string broken_vsols_name("invalid_name");
    {
      std::vector< bvis1_view_tableau_sptr > views;
      views = MANAGER->get_views();
      vgui_selector_tableau &selector = *(views[v]->selector());
      for (unsigned i=0; i < 50; ++i) {
        std::ostringstream s;
        if (i != 0) {
          s << i;
        }

        std::string putative_name = std::string("broken_vsols") + s.str();
        vgui_tableau_sptr t = selector.get_tableau(putative_name);
        if (!t) {
          if (broken_vsols_name == "invalid_name")
            broken_vsols_name = putative_name;
        } else {
          broken_vsols_name = "invalid_name";
          assert(i != 49);
        }
      }
    }

    // create the output storage class
    {
      vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
      output_vsol->add_objects(base_vsols2, broken_vsols_name);
      output_vsol->set_name(broken_vsols_name);
      MANAGER->repository()->store_data_at(output_vsol, frame_v_[v]);
      MANAGER->add_to_display(output_vsol);
      MANAGER->display_current_frame();
    }

    // make it active
    std::vector< bvis1_view_tableau_sptr > views;
    views = MANAGER->get_views();
    assert (views.size() == nviews());
    views[v]->selector()->set_active(broken_vsols_name);
  }
}

unsigned mw_curve_tracing_tool_2_maro::
view_from_frame_index(unsigned fi) const
{
  std::vector<int>::const_iterator result = 
    std::find(frame_v_.begin(), frame_v_.end(), fi);
  
  return result - frame_v_.begin();
}

void mw_curve_tracing_tool_2_maro::
equalize_matching_subcurve()
{
  // this block could be made into a memer of bmcsd_curve_stereo if it turns out to be useful.
  unsigned ini_id_sub, end_id_sub;
  { 
  if (!s_->selected_crv(0) || !s_->selected_crv(1))
    std::cerr << "A curve must be selected in each view." << std::endl;

  unsigned ic;
  if (!s_->get_index_of_candidate_curve(s_->selected_crv(1), &ic))
    std::cerr << "Error: selected curve not found among candidates.\n";

  unsigned ini_id, 
           end_id; 

  s_->get_increasing_endpoints(&ini_id, &end_id);

  s_->get_matching_subcurve(
      ic,
      ini_id,
      end_id,
      &ini_id_sub,
      &end_id_sub);

  s_->set_subcurve(ini_id_sub, end_id_sub);
  }

  initialize_curve_selection();
  update_pn(s_->subcurve()->vertex(s_->subcurve()->size()-1));
}

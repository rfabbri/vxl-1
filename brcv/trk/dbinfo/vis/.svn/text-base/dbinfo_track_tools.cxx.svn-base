
// This is brcv/trk/dbinfo/vis/dbinfo_track_tools.cxx
//:
// \file

#include "dbinfo_track_tools.h"
#include <vcl_sstream.h>
#include <vnl/vnl_numeric_traits.h>
#include <vil/vil_image_resource.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_command.h> 
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <bgui/bgui_vsol_soview2D.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <bsol/bsol_algs.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_mapper.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/vidpro1_repository_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <bpro1/bpro1_storage.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbinfo/dbinfo_observation_generator.h>
#include <dbinfo/pro/dbinfo_track_storage.h>


//----------------------------------------------------------------------------
//: A vgui command to delete a track from storage
class dbinfo_delete_track_command : public vgui_command
{
public:
  dbinfo_delete_track_command(dbinfo_track_edit_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    dbinfo_track_sptr track = tool_->selected_track();
    if(!track)
      {
        vcl_cout <<"No track selected - can't delete\n";
        return;
      }
    dbinfo_track_storage_sptr track_store = tool_->track_storage();
    track_store->delete_track(track->id());
    //remove the track soview from the display
    tool_->remove_object();
  }
  dbinfo_track_edit_tool* tool_;
};


//get track storage from the storage pointer
static dbinfo_track_storage_sptr get_track_storage(bpro1_storage_sptr const& sto)
{
  dbinfo_track_storage_sptr track_storage;
  track_storage.vertical_cast(sto);
  return track_storage;
}

static bpro1_storage_sptr storage()
{
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
  bpro1_storage_sptr data = res->get_data_at("dbinfo_track_storage", -1);
  return data;
}


static vsol_polyline_2d_sptr 
get_polyline(vgui_displaylist2D_tableau_sptr const& tab, vgui_soview2D*& object)
{
  if( tab.ptr()){
    object = (vgui_soview2D*)tab->get_highlighted_soview();
    if( object != NULL ) {
      if(object->type_name()=="bgui_vsol_soview2D_polyline")
        {
          bgui_vsol_soview2D_polyline* vsov = 
            (bgui_vsol_soview2D_polyline*)object;
          return vsov->sptr();
        }
    }
  }
  return (vsol_polyline_2d*)0;
}

static vsol_polygon_2d_sptr 
get_polygon(vgui_displaylist2D_tableau_sptr const& tab, vgui_soview2D*& object)
{
  if( tab.ptr()){
    object = (vgui_soview2D*)tab->get_highlighted_soview();
    if( object != NULL ) {
      if(object->type_name()=="bgui_vsol_soview2D_polygon")
        {
          bgui_vsol_soview2D_polygon* vsov = 
            (bgui_vsol_soview2D_polygon*)object;
          return vsov->sptr();
        }
    }
  }
  return (vsol_polygon_2d*)0;
}

static vil_image_resource_sptr get_image()
{
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
  if(!res->go_to_frame(bvis1_manager::instance()->current_frame()))
    return (vil_image_resource*)0;
  bpro1_storage_sptr sto = res->get_data("image");
  if(!sto)
    return (vil_image_resource*)0;
  vcl_cout << "Image Storage Name " << sto->name() << '\n';
  vidpro1_image_storage_sptr image_storage;
  image_storage.vertical_cast(sto);
  if(!image_storage)
    return (vil_image_resource*)0;
  return image_storage->get_image();
}

static dbinfo_track_sptr 
select_track(vgui_displaylist2D_tableau_sptr const& tab,
             dbinfo_track_storage_sptr const& track_storage,
             vgui_soview2D*& object
             )
{
  vsol_polyline_2d_sptr pl = get_polyline(tab,object);
  if(pl)
    {
      if(!track_storage)
        return (dbinfo_track*)0;
      unsigned frm =
        static_cast<unsigned>(bvis1_manager::instance()->current_frame());
      return track_storage->track_by_spacetime(frm, pl->p1());
    }
  return (dbinfo_track*)0;
}
//=====================  descibe_track_tool =================
//Constructor
dbinfo_track_describe_tool::dbinfo_track_describe_tool( const vgui_event_condition& lift,
                                                        const vgui_event_condition& drop )
  : gesture_lift_(lift), gesture_drop_(drop), active_(false), object_(NULL),
    tableau_(NULL), cached_tableau_(NULL)
{
}


//: Destructor
dbinfo_track_describe_tool::~dbinfo_track_describe_tool()
{
}


//: Return the name of this tool
vcl_string
dbinfo_track_describe_tool::name() const
{
  return "Describe Track";
}


//: Set the tableau to work with
bool
dbinfo_track_describe_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_displaylist2D_tableau_sptr(dynamic_cast<vgui_displaylist2D_tableau*>(tableau.ptr()));
  if( !tableau_  ) 
    return false;
  
  return true;
}

void dbinfo_track_describe_tool::activate()
{
  bpro1_storage_sptr sto = storage();
  track_storage_ = get_track_storage(sto);
}
//: Handle events
bool
dbinfo_track_describe_tool::handle( const vgui_event & e, 
                                    const bvis1_view_tableau_sptr& view )
{ 
  if(gesture_lift_(e))
    {
      dbinfo_track_sptr track = 
        select_track(tableau_, track_storage_, object_);
      if(track)
        {
          vcl_cout << *track << '\n';
          return true;
        }
      else
        vcl_cout << "No track selected \n";
    }
  return false;
}
//============================== Track Edit Tool ============================

//Constructor
dbinfo_track_edit_tool::dbinfo_track_edit_tool( const vgui_event_condition& lift )
  : gesture_lift_(lift), object_(NULL), tableau_(NULL)
{
}


//: Destructor
dbinfo_track_edit_tool::~dbinfo_track_edit_tool()
{
}


//: Return the name of this tool
vcl_string dbinfo_track_edit_tool::name() const
{
  return "Edit Track";
}


//: Set the tableau to work with
bool
dbinfo_track_edit_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_displaylist2D_tableau_sptr(dynamic_cast<vgui_displaylist2D_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;

  return true;
}
//: Add popup menu items
void 
dbinfo_track_edit_tool::get_popup( const vgui_popup_params& params, 
                                   vgui_menu &menu )
{
  vgui_menu pop_menu;
  pop_menu.add("Delete Track", new dbinfo_delete_track_command(this));
  menu.add("Track", pop_menu); 
}


//: Handle events
bool
dbinfo_track_edit_tool::handle( const vgui_event & e,
                                const bvis1_view_tableau_sptr& view )
{
  if(gesture_lift_(e))
    {
      selected_track_ = select_track(tableau_, track_storage_,object_);
      if(selected_track_)
        {
          vcl_cout << *selected_track_ << '\n';
          return true;
        }
      else
        vcl_cout << "No track selected \n";
    }
  return false;
}
void dbinfo_track_edit_tool::activate()
{
  bpro1_storage_sptr sto = storage();
  track_storage_ = get_track_storage(sto);
}
//

//=== Class Members === 
//Constructor
dbinfo_region_minfo_tool::
dbinfo_region_minfo_tool( const vgui_event_condition& lift,
                          const vgui_event_condition& drop,
                          const vgui_event_condition& pick)
  : intensity_info_(true), gradient_info_(false), color_info_(false),
    //public parameters
    gamma_(0.5), mina_(0), maxa_(0),radius_(5.0f),step_(1.0f),
    //
    gesture_lift_(lift), gesture_drop_(drop), gesture_pick_(pick),
    pick_mode_(false), 
    diff_x(0), diff_y(0), last_x(0), last_y(0), x0(0), y0(0), tx(0), ty(0),
    picked_poly_(0), poly0_(NULL), poly_(NULL),
    obs_(NULL), active_(false), object_(NULL),
    tableau_(NULL), cached_tableau_(NULL)
{
  vvs_ = NULL;
#if 0
  vvs_ = new vidpro1_vsol2D_storage;
  vvs_->set_name("attribute_points");
  //add the storage to the repository
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
  res->store_data(vvs_);
#endif
}


//: Destructor
dbinfo_region_minfo_tool::~dbinfo_region_minfo_tool()
{
}


//: Return the name of this tool
vcl_string
dbinfo_region_minfo_tool::name() const
{
  return "Measure Mutual Info";
}


//: Set the tableau to work with
bool
dbinfo_region_minfo_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_displaylist2D_tableau_sptr(dynamic_cast<vgui_displaylist2D_tableau*>(tableau.ptr()));
  if( !tableau_  ) 
    return false;
  
  return true;
}

void dbinfo_region_minfo_tool::print_minfo(vgui_displaylist2D_tableau_sptr const& tab,
                                           vgui_soview2D*& object)
{
  vil_image_resource_sptr imgr = get_image();
  poly_ = get_polygon(tab, object);
  if(!imgr||!poly_)
    {
      vcl_cout << "print_minfo - No image or no region selected\n";
      return;
    }
  vcl_cout << "Polygon at Readout\n";
  poly_->describe(vcl_cout);
  float minfo = dbinfo_observation_matcher::minfo(obs_, imgr, poly_);
  vcl_cout << "Mutual Information = " << minfo << '\n';
}
void dbinfo_region_minfo_tool::set_observation()
{
  vil_image_resource_sptr imgr = get_image();
  if(!imgr||!picked_poly_)
    {
      vcl_cout << "set_observation - No image or no region selected\n";
      return;
    }
  poly0_ = new vsol_polygon_2d(*picked_poly_);
  vcl_cout << "Initial Polygon\n";
  poly0_->describe(vcl_cout);
  unsigned frame =
    static_cast<unsigned>(bvis1_manager::instance()->current_frame());
 
  obs_ = new dbinfo_observation(frame, imgr, poly0_,
                                intensity_info_, gradient_info_,
                                color_info_);
  vcl_cout << "Set Observation\n";
  vcl_cout << *obs_ << '\n';
}
//: Render the region mutual information display
void dbinfo_region_minfo_tool::render()
{
  //create a new mapper
  bvis1_mapper_sptr mapper = new bvis1_mapper(mina_, maxa_, gamma_);

  //set the mapper on the displayer
  bvis1_displayer_sptr displayer = 
    bvis1_manager::instance()->displayer("vsol2D");
  displayer->set_mapper(mapper);
  bvis1_manager::instance()->display_current_frame(true);
}

//: Scan a region and display a set of sample points with mutual information
bool dbinfo_region_minfo_tool::scan_region()
{
  vil_image_resource_sptr imgr = get_image();
  if(!imgr||!picked_poly_||!obs_)
    {
      vcl_cout << "scan_region - No image or no region selected"
               <<" or no observation\n";
      return false;
    }

  //The region to be scanned
  vsol_polygon_2d_sptr scan_region = new vsol_polygon_2d(*picked_poly_);
  vcl_cout << "scan_iterator debug vertices\n";
  scan_region->describe(vcl_cout);
  unsigned cols = imgr->ni(), rows = imgr->nj();
  vsol_point_2d_sptr poly_cog = scan_region->centroid();
  //get the cog of the currently set observation
  vsol_point_2d_sptr obs_cog = obs_->geometry()->cog();
  float obs_x = static_cast<float>(obs_cog->x()),
    obs_y = static_cast<float>(obs_cog->y());

  //create a region geometry instance to define the scan raster points
  dbinfo_region_geometry_sptr rg =
    new dbinfo_region_geometry(cols, rows, scan_region);
  vgl_h_matrix_2d<float> H;
  H.set_identity();

  //Scan the region using each region point as the cog of the new observation
  vcl_vector<vsol_spatial_object_2d_sptr> sos;
  vcl_vector<double> attrs;
  mina_ = vnl_numeric_traits<double>::maxval;
  maxa_ = -mina_;
  unsigned npts = rg->size();
  for(unsigned i = 0; i<npts; ++i)
    {
      vgl_point_2d<float> p = rg->point(i);
      //Debug for scan iterator
      vcl_cout << p.x() << ' ' << p.y() << '\n';
      float tx = p.x()-obs_x, ty = p.y()-obs_y;
      H.set_translation(tx, ty);
      dbinfo_observation_sptr new_obs = 
        dbinfo_observation_generator::generate(obs_, H);
      new_obs->scan(0, imgr);
      sos.push_back(new vsol_point_2d(p.x(), p.y()));
      double minfo = dbinfo_observation_matcher::minfo(obs_, new_obs);
      attrs.push_back(minfo);
      //determine range of mutual information values
      if(minfo<mina_)
        mina_ = minfo;
      if(minfo>maxa_)
        maxa_= minfo;
    }
  vidpro1_vsol2D_storage_sptr v2Ds = new vidpro1_vsol2D_storage;
  vcl_stringstream sstr;
  sstr << poly_cog->x() << '_'<< poly_cog->y();
  vcl_string name = "attribute_points:";
  name += sstr.str();
  v2Ds->set_name(name);
  v2Ds->add_objects(sos, attrs);
  //add the storage to the repository
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
  res->store_data(v2Ds);
  
  //render the display
  this->render();
  return true;
}
//: Explore a interval around the location of an observation and 
// print mutual information
bool dbinfo_region_minfo_tool::explore()
{
  vil_image_resource_sptr imgr = get_image();
  if(!obs_||!imgr)
    {
      vcl_cout << "Must have a set observation and valid image to explore\n";
      return false;
    }
  unsigned steps = static_cast<unsigned>(2.0f*radius_/step_);
  double minfo =0;
  vcl_cout << "Mathmatica Array\n";
  vcl_cout << "{";
  unsigned yi=0;
  for(float y=-radius_ ; yi<steps; y+=step_,++yi)
    {
      vcl_cout << "{";
      unsigned xi = 0;
      for(float x=-radius_; xi<steps; x+=step_, ++xi)
        {
          vgl_h_matrix_2d<float>H;
          H.set_identity();
          H.set_translation(x, y);
          dbinfo_observation_sptr obs = 
            dbinfo_observation_generator::generate(obs_, H);
          if(!obs||!obs->scan(0, imgr))
            {
              vcl_cout << "scan failed in explore\n";
              return false;
            }
          minfo = dbinfo_observation_matcher::minfo(obs_, obs);
          if(xi != steps-1)
            vcl_cout << minfo << ',';
          else
            vcl_cout << minfo ;
        }
      if(yi != steps-1)
        vcl_cout << "},";
      else
        vcl_cout << minfo << '}';
    }
  vcl_cout << "}\n";
  vcl_cout << "end array\n";
  return true;
}
// =====================HANDLER========================
//: Handle events
bool
dbinfo_region_minfo_tool::handle( const vgui_event & e, 
                                  const bvis1_view_tableau_sptr& view )
{ 
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  // cache the current position as the last known position
  if( e.type == vgui_MOTION ) {
    last_x = ix;
    last_y = iy;
  }
  
  if( active_ ){
    if( gesture_drop_(e) ){
      float x,y;
      object_->get_centroid( &x , &y );
      object_->translate( last_x - x - diff_x , last_y - y - diff_y );
      active_ = false;
      object_ = NULL;
      cached_tableau_->highlight( 0 );
      cached_tableau_->motion( e.wx , e.wy );
      cached_tableau_->post_redraw();
      cached_tableau_ = vgui_displaylist2D_tableau_sptr(NULL);
      tx = x-x0; ty = y-y0;
      print_minfo(tableau_, object_);
      return true;
    }
    else if ( e.type == vgui_MOTION ){
      float x,y;
      object_->get_centroid( &x , &y );
      object_->translate( last_x - x - diff_x , last_y - y - diff_y );
      cached_tableau_->post_redraw();
      return true;
    }
  }
  else{if( tableau_.ptr())
    {
      object_ = (vgui_soview2D*)tableau_->get_highlighted_soview();
      if( object_ != NULL )
        if(gesture_lift_(e))
          {
            float x, y;
            active_ = true;
            cached_tableau_ = tableau_;
            object_->get_centroid( &x , &y );
            // the distance to the centroid
            diff_x = last_x - x;
            diff_y = last_y - y;
            x0 = x; y0 = y;
            return true;
          }
        else if(gesture_pick_(e))
          picked_poly_ = get_polygon(tableau_, object_);
    }
  }
  return false;
}
//=============================  mutual info tool ========================
//
//                              ==Commands==

//----------------------------------------------------------------------------
//: A vgui command to set a observation for computing mutual information
class dbinfo_set_observation_command : public vgui_command
{
public:
  dbinfo_set_observation_command(dbinfo_region_minfo_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    if(!tool_->picked_poly())
      {
        vcl_cout << "dbinfo_set_observation_command(.) No polygon selected\n";
        return;
      }
    tool_->set_observation();
  }
  dbinfo_region_minfo_tool* tool_;
};

//----------------------------------------------------------------------------
//: A vgui command to scan a region and encode sample points with mutual info
class dbinfo_scan_region_command : public vgui_command
{
public:
  dbinfo_scan_region_command(dbinfo_region_minfo_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    if(!tool_->picked_poly())
      {
        vcl_cout << "dbinfo_scan_region_command(.) No region selected\n";
        return;
      }
    tool_->scan_region();
  }
  dbinfo_region_minfo_tool* tool_;
};

//----------------------------------------------------------------------------
//: A vgui command to set parameters for mutual information measurements
class dbinfo_set_parameters_command : public vgui_command
{
public:
  dbinfo_set_parameters_command(dbinfo_region_minfo_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    vgui_dialog param_dlg("Change Parameters");
    param_dlg.checkbox("Intensity Info", tool_->intensity_info_);
    param_dlg.checkbox("Gradient Info", tool_->gradient_info_);
    param_dlg.checkbox("Color Info", tool_->color_info_);
    param_dlg.field("Gamma", tool_->gamma_);
    param_dlg.field("Search Radius", tool_->radius_);
    param_dlg.field("Search Step", tool_->step_);
    if(!param_dlg.ask())
      return;
  }
  dbinfo_region_minfo_tool* tool_;
};

//----------------------------------------------------------------------------
//: A vgui command to render the mutual information region display
class dbinfo_render_command : public vgui_command
{
public:
  dbinfo_render_command(dbinfo_region_minfo_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    tool_->render();
  }
  dbinfo_region_minfo_tool* tool_;
};

//----------------------------------------------------------------------------
//: A vgui command to scan a search region about the base observation and
//  print mutual informationn
class dbinfo_explore_command : public vgui_command
{
public:
  dbinfo_explore_command(dbinfo_region_minfo_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    tool_->explore();
  }
  dbinfo_region_minfo_tool* tool_;
};

void dbinfo_region_minfo_tool::get_popup( const vgui_popup_params& params, 
                                          vgui_menu &menu )
{
  vgui_menu pop_menu;
  pop_menu.add("Set Parameters", new dbinfo_set_parameters_command(this));
  pop_menu.add("Render", new dbinfo_render_command(this));
  pop_menu.add("Set Observation", new dbinfo_set_observation_command(this));
  pop_menu.add("Scan Region", new dbinfo_scan_region_command(this));
  pop_menu.add("Explore MI Variation", new dbinfo_explore_command(this));
  menu.add("Mutual Info", pop_menu); 
}

// This is contrib/biotree/biov/biov_examiner_tableau.cxx

//:
// \file

#include <biov/biov_examiner_tableau.h>
#include <biov/biov_colormaps.h>
#include <biov/CvrGIMPGradient.h>

#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_slider_tableau.h>
#include <vgui/vgui_poly_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/projectors/SbSphereSheetProjector.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoImage.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoAnnotation.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <VolumeViz/details/SoVolumeRenderDetail.h>

void seeksensorCB(void * data, SoSensor * s);

// Bitmap representations of an "X", a "Y" and a "Z" for the axis cross.
static GLubyte xbmp[] = { 0x11,0x11,0x0a,0x04,0x0a,0x11,0x11 };
static GLubyte ybmp[] = { 0x04,0x04,0x04,0x04,0x0a,0x11,0x11 };
static GLubyte zbmp[] = { 0x1f,0x10,0x08,0x04,0x02,0x01,0x1f };

int count = 0;
SbVec3f d;

//: Constructor 
 
biov_examiner_tableau::biov_examiner_tableau(SoNode * scene_root, SoTransferFunction * n_transfunc)
 : bgui3d_fullviewer_tableau(scene_root),
   last_down_button_(vgui_BUTTON_NULL), seek_distance_(SEEK_HALF), 
   axis_visible_(true), last_timestamp_(0), last_viewport_sz_(-1, -1)
{

   interaction_type_ = CAMERA;
   spin_projector_ = new SbSphereSheetProjector(SbSphere(SbVec3f(0, 0, 0), 0.8f));
   SbViewVolume volume;
   volume.ortho(-1, 1, -1, 1, -1, 1);
   spin_projector_->setViewVolume( volume );
   _seekSensor = new SoTimerSensor( seeksensorCB, this );
   btf = new biov_transfer_function(this);
   transfunc = n_transfunc;
   map = transfunc->predefColorMap.getValue();

   // COLOR BAR AND SLIDERS ROOT
   //SoAnnotation *cmap_root_ = new SoAnnotation;
   SoSeparator *cmap_root_ = new SoSeparator;
   cmapCamera_ = new SoOrthographicCamera;
   cmapCamera_->position.setValue(SbVec3f(0.0, 0.0, 10.0));
   cmapCamera_->nearDistance = 8.0;
   cmapCamera_->farDistance = 12.0;
   cmapCamera_->focalDistance = 10.0;
   cmapCamera_->height.setValue(1.0) ;
   //cmapCamera_->viewportMapping = SoCamera::ADJUST_CAMERA;
   // translation color map to the upper left corner
   cmap_transform = new SoTransform;
   SoDirectionalLight* cmap_light = new SoDirectionalLight;
   cmap_light->direction.setValue(SbVec3f(0., 0., 1.0));
   cmap_light->color.setValue(SbVec3f(1.0, 1.0, 1.0));

   // COLOR BAR
   cmap_height_ = 310;
   cmap_width_ = 25;
   cmap_image_ = new SoImage;
   cmap_image_->vertAlignment = SoImage::TOP;
   cmap_image_->horAlignment = SoImage::LEFT;
   cmap_image_->height = cmap_height_;
   cmap_image_->width = cmap_width_;

   ((SoGroup*)(scene_root_))->addChild(cmap_root_);
   cmap_root_->addChild(cmapCamera_);
   cmap_root_->addChild(cmap_light);
   cmap_root_->addChild(cmap_transform);
   cmap_root_->addChild(cmap_image_);

   // MIN SLIDER
   this->min = 0;
   SoSeparator *min_root = new SoSeparator;
   min_transform = new SoTransform;
   min_transform->rotation.setValue(SbVec3f(0, 0, 1), 1.5707963f);
   min_transform->translation.setValue(SbVec3f(0.1, 0, 0));
   min_mark_ = new SoCone;
   min_mark_ ->bottomRadius.setValue(0.05);
   min_mark_ ->height.setValue(0.1);
  
   ((SoGroup*)(cmap_root_))->addChild(min_root);
   min_root->addChild(min_transform);
   min_root->addChild(min_mark_ );

   min_bar_grabbed_ = false;
   // MAX SLIDER
   this->max = 255;
   SoSeparator *max_root = new SoSeparator;
   max_transform = new SoTransform;
   max_transform->rotation.setValue(SbVec3f(0, 0, 1), 1.5707963f);
   max_transform->translation.setValue(SbVec3f(0.1, 0, 0));
   max_mark_ = new SoCone;
   max_mark_ ->bottomRadius.setValue(0.05);
   max_mark_ ->height.setValue(0.1);
  
   ((SoGroup*)(cmap_root_))->addChild(max_root);
   max_root->addChild(max_transform);
   max_root->addChild(max_mark_ );

   max_bar_grabbed_ = false;

   // POSITION COLOR BAR AND LOAD PIXEL VALUES
   loadColorMap();
}


//: Destructor
biov_examiner_tableau::~biov_examiner_tableau()
{
  //: Ming: no need to delete the SbSphereSheetProjector object
  ///delete spin_projector_;  
}



vcl_string biov_examiner_tableau::type_name() const {return "biov_examiner_tableau";}

//: Handle the events coming in
//:   Left mouse = spin
//:   Middle mouse = pan
//:   Middle mouse with control = zoom (dolly)
//:   's' and then click = seeks 2/3 of the distance to the object
//:   'i' toggles idle events
//:   'h' puts camera to the current view
//:   'h' with control sets the home view to the current view
//:   'v' views the entire scene
//:   The cursors pan
//:   The cursors with 'control' spin
//:   The up and down cursors with shift zoom (dolly)


bool biov_examiner_tableau::handle(const vgui_event& e)
{ 
  // to deal with multiple tableaus in a grid
  if( e.type == vgui_LEAVE )
    reset_log();
  if( e.type == vgui_ENTER )
    post_idle_request();

  // ALWAYS DO KEYPRESSES 
  if( e.type == vgui_KEY_PRESS )
  {
    //float aspect_ratio = get_viewport_region().getViewportAspectRatio();
    SbVec2f middle(0.5f, 0.5f);
    SbVec2f left(0.4f, 0.5f);
    SbVec2f right(0.6f, 0.5f);
    SbVec2f up(0.5f, 0.6f);
    SbVec2f down(0.5f, 0.4f);
    switch(e.key)
    {
      case 'v':
        view_all();
      break;
      
      case 'h':
        if(e.modifier == vgui_CTRL)
          save_home_position();
        else
          reset_to_home_position();
        break;
      

      case vgui_CURSOR_LEFT:
        if(e.modifier == vgui_CTRL)
          spin(left, middle);
        else if(e.modifier == vgui_MODIFIER_NULL) // rotate left
        {
          SbRotation camrot = scene_camera_->orientation.getValue();
          SbVec3f upvec(0, 1, 0); // init to default up vector
          camrot.multVec(upvec, upvec);
          SbRotation rotation(upvec, 0.025f );
          scene_camera_->orientation = camrot*rotation;
        }
      break;
      
      case vgui_CURSOR_RIGHT:
        if(e.modifier == vgui_CTRL)
          spin(right, middle);
        else if(e.modifier == vgui_MODIFIER_NULL) // rotate right
        {
          SbRotation camrot = scene_camera_->orientation.getValue();
          SbVec3f upvec(0, 1, 0); // init to default up vector
          camrot.multVec(upvec, upvec);
          SbRotation rotation(upvec, -0.025f );
          scene_camera_->orientation = camrot*rotation;
        
        }
      break;
      
      case vgui_CURSOR_UP:
        if(e.modifier == vgui_CTRL)
          spin(up, middle);
        else if(e.modifier == vgui_MODIFIER_NULL) // move forward
        {
          SbRotation camrot = scene_camera_->orientation.getValue();
          SbVec3f lookat(0, 0, -1); // init to default view direction vector
          camrot.multVec(lookat, lookat);
          SbVec3f pos = scene_camera_->position.getValue();
          pos += lookat;
          scene_camera_->position = pos;
          float foc = scene_camera_->focalDistance.getValue();
          foc -= lookat.length();
          scene_camera_->focalDistance = foc;
        }
        else if(e.modifier == vgui_SHIFT)
          zoom(1.0f);    
      break;

      case vgui_CURSOR_DOWN:
        if(e.modifier == vgui_CTRL)
          spin(down, middle);
        else if(e.modifier == vgui_MODIFIER_NULL) // move backward
        {
          SbRotation camrot = scene_camera_->orientation.getValue();
          SbVec3f lookat(0, 0, -1); // init to default view direction vector
          camrot.multVec(lookat, lookat);
          SbVec3f pos = scene_camera_->position.getValue();
          pos -= lookat;
          scene_camera_->position = pos;
          float foc = scene_camera_->focalDistance.getValue();
          foc += lookat.length();
          scene_camera_->focalDistance = foc;
        }
        else if(e.modifier == vgui_SHIFT)
          zoom(-1.0f);   
      break;
      default:
        break;
    }
    
  }

  // ONLY IF CAMERA INTERACTION MODE
  if( interaction_type_ == CAMERA )
  {
    
    const SbVec2s viewport_size(get_viewport_region().getViewportSizePixels());
    const SbVec2s viewport_origin(get_viewport_region().getViewportOriginPixels());
    const SbVec2s curr_pos = SbVec2s(e.wx, e.wy) - viewport_origin;
    const SbVec2f curr_pos_norm((float) curr_pos[0] / (float) vcl_max((int)(viewport_size[0] - 1), 1),
                                (float) curr_pos[1] / (float) vcl_max((int)(viewport_size[1] - 1), 1));
    const SbVec2f last_pos_norm = last_pos_;
    float aspect_ratio = get_viewport_region().getViewportAspectRatio();
  
    if( e.type == vgui_KEY_PRESS && e.key == 's' )
    {
      last_process_ = SEEK;
    }
    // SEEK
    else if( last_process_ == SEEK && e.type == vgui_MOUSE_DOWN )
    {
      reset_log();
      seek_to_point(curr_pos);
      last_process_ = IDLE;
    }
    // MOUSE DOWN
    else if( e.type == vgui_MOUSE_DOWN )
    {
      if (e.wx >= 0 && e.wx <= cmap_width_*2)
      {// drag on sliding bar 
        int mouse_pos = static_cast<int>((viewport_size[1] - e.wy)*255.0/cmap_height_);

        if(mouse_pos >= this->min-10 && mouse_pos <= this->min + 10)
          min_bar_grabbed_ = true;
        if(mouse_pos >= this->max -10 && mouse_pos <= this->max + 10)
          max_bar_grabbed_ = true;
      }
      
      reset_log();
      last_down_button_ = e.button;
      if (e.button == vgui_RIGHT) {
      if( e.modifier == vgui_CTRL)
      {
              vcl_cerr << "ctrl right-click\n";
              SbVec2s pos = SbVec2s(e.wx, e.wy) - viewport_origin;
              SbVec2s evt(e.wx, e.wy);
              vcl_cerr << "click = " <<  evt[0] << " " << evt[1] ;
              vcl_cerr << "origin = " << viewport_origin[0] << " " << viewport_origin[1] ;
              vcl_cerr << "pos = " << pos[0] << " " << pos[1] << "\n";


              SbViewportRegion v = get_viewport_region();

              SbVec2s size = v.getViewportSizePixels();
              vcl_cerr << "viewport size = " << size[0] << " " << size[1] << "\n";

              v.setViewportPixels( SbVec2s(0,0), v.getViewportSizePixels() ); 
              SoRayPickAction rpaction( v );
              rpaction.setPoint( pos );
              rpaction.setRadius( 1 );
              rpaction.apply( user_scene_root_ );
              SoPickedPoint * point = rpaction.getPickedPoint();
              if (!point) {
                      vcl_cerr << "no hit\n";
              }
              else{
                      SbVec3f v = point->getPoint();
                      SbVec3f nv = point->getNormal();
                      (void)fprintf(stdout, "point=<%f, %f, %f>, normvec=<%f, %f, %f>\n",
                                    v[0], v[1], v[2], nv[0], nv[1], nv[2]);

                      const SoDetail * d = point->getDetail();
                      if (d->getTypeId() == SoVolumeRenderDetail::getClassTypeId()) {
                              SoVolumeRenderDetail * detail = (SoVolumeRenderDetail *)d;
                              const int nrprofilepoints = detail->getProfileDataPos();
                              (void)fprintf(stdout, "hit %d voxels\n", nrprofilepoints);
                              (void)fprintf(stdout, "voxel values: ");
                              for (int i=0; i < nrprofilepoints; i++) {
                                      SbVec3f objpos;
                                      unsigned int voxelval = detail->getProfileValue(i, NULL, &objpos);
                                      (void)fprintf(stdout, "0x%08x ", voxelval);
                              }
                              (void)fprintf(stdout, "\n");
                      }
              }


              /*
          SbVec3f curr_wpt = screenToWorld(SbVec2f((float)curr_pos_norm[0],
                                                   (float)curr_pos_norm[1]));
          SbVec3f last_wpt = screenToWorld(SbVec2f((float)last_pos_norm[0],
                                                   (float)last_pos_norm[1]));
          SbVec3f trans = curr_wpt - last_wpt;
          SbVec3f new_pos = scene_camera_->position.getValue() + trans;
          scene_camera_->position.setValue(new_pos);
          scene_camera_->pointAt(curr_wpt);
          */
      }
      }
    }

    // MOUSE UP
    else if( e.type == vgui_MOUSE_UP )
    {
      last_down_button_ = vgui_BUTTON_NULL;
      last_process_ = IDLE;
      if( e.timestamp - last_timestamp_ > 100 )
        reset_log();

      int mouse_pos = static_cast<int>((viewport_size[1] - e.wy)*255.0/cmap_height_);
      if(min_bar_grabbed_)
      {
        transfer_callback(true, mouse_pos, this->max, this->map);
        min_bar_grabbed_ = false;
      }
      if(max_bar_grabbed_)
      {
        transfer_callback(true, this->min, mouse_pos, this->map);
        max_bar_grabbed_ = false;
      }

      if( e.modifier == vgui_CTRL)
      {
        interaction_type_ = SCENEGRAPH;
        bool b = bgui3d_fullviewer_tableau::handle(e);
        interaction_type_ = CAMERA;
        return b;
      }
    }

    // MOUSE MOTION
    else if( e.type == vgui_MOUSE_MOTION )
    {
      if( e.modifier == vgui_CTRL && last_down_button_ == vgui_LEFT)
      {
        interaction_type_ = SCENEGRAPH;
        ///bool b = 
        bgui3d_fullviewer_tableau::handle(e);
        interaction_type_ = CAMERA;
        //return b;
      }

      const SbVec2s curr_pos = SbVec2s(e.wx, e.wy) - viewport_origin;

      last_pos_ = curr_pos_norm;
  
      // MOUSE DOWN HANDLING
      switch(last_down_button_)
      {
        case vgui_LEFT:
          if( e.modifier != vgui_CTRL && 
              !min_bar_grabbed_ && !max_bar_grabbed_ ) {
            spin(curr_pos_norm, last_pos_norm);
            update_log( curr_pos_norm );
            last_process_ = DRAG;
          }
          break;

        case vgui_MIDDLE:
          if(e.modifier == vgui_CTRL)
          {
            zoom( (curr_pos_norm[1] - last_pos_norm[1]) * 20.0f );
            last_process_ = ZOOM;
          }
          else
          {
            pan(curr_pos_norm, last_pos_norm, aspect_ratio); 
            last_process_ = PAN;
          }

          break;

        default:
          break;
      }

    last_timestamp_ = e.timestamp;

    }
    if( idle_enabled_ )
    {
      idle();
    }
  }
  set_clipping_planes();
  return bgui3d_fullviewer_tableau::handle(e);
}


//: When idle, spin the scene based on the log
bool biov_examiner_tableau::idle()
{
  if( idle_enabled_ && last_process_ == IDLE && log_.size >2  )
    {
      SbVec2f p = log_.pos3 + (log_.pos1 - log_.pos3)/5.0;
      spin(p, log_.pos3);
      request_render();
    }
    return bgui3d_fullviewer_tableau::idle();
}

//----------------------------------------------------------------------------
//: A vgui command used to toggle axis visibility
class biov_axis_visible_command : public vgui_command
{
  public:
  biov_axis_visible_command(biov_examiner_tableau* tab) 
   : bgui3d_examiner_tab(tab) {}
  void execute()
  {
    bool visible = bgui3d_examiner_tab->axis_visible();
    bgui3d_examiner_tab->set_axis_visible(!visible);
  }

  biov_examiner_tableau *bgui3d_examiner_tab;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//: A vgui command used to toggle interaction type
class biov_seek_distance_command : public vgui_command
{
  public:
  biov_seek_distance_command(biov_examiner_tableau* tab,
                               biov_examiner_tableau::SeekDistance seek) 
                              : bgui3d_examiner_tab(tab), seek_distance(seek) {}
  void execute()
  {
    bgui3d_examiner_tab->set_seek_distance(seek_distance);
  }

  biov_examiner_tableau *bgui3d_examiner_tab;
  biov_examiner_tableau::SeekDistance seek_distance;
};

//: Builds a popup menu
void biov_examiner_tableau::get_popup(const vgui_popup_params& params,
                                            vgui_menu &menu)
{
  bgui3d_fullviewer_tableau::get_popup(params, menu);

  vcl_string axis_item;
  if( this->axis_visible() )
    axis_item = "Disable Axis";
  else
    axis_item = "Enable Axis";

  menu.add(axis_item, new biov_axis_visible_command(this));
  
  vcl_string check_on = "[x]";
  vcl_string check_off = "[ ]";

  vgui_menu seek_menu;
  SeekDistance seek = seek_distance_;

  seek_menu.add(((seek==SEEK_FAR)?check_on:check_off) + " 3/4 ", 
              new biov_seek_distance_command(this,SEEK_FAR));
  seek_menu.add(((seek==SEEK_HALF)?check_on:check_off) + " 1/2",
                  new biov_seek_distance_command(this,SEEK_HALF));
  seek_menu.add(((seek==SEEK_NEAR)?check_on:check_off) + " 1/4",
                  new biov_seek_distance_command(this,SEEK_NEAR));
  seek_menu.add(((seek==SEEK_ZERO)?check_on:check_off) + " 0 (focus)",
                  new biov_seek_distance_command(this,SEEK_ZERO));

  menu.add( "Seek Distance", seek_menu );
}

//: Creates the menu items
vgui_menu biov_examiner_tableau::create_menus()
{
  vgui_menu file;
//  file.add("Open",dummy,(vgui_key)'O',vgui_CTRL);
//  file.add("Quit",quit,(vgui_key)'R',vgui_SHIFT);

  vgui_menu view;
  view.add("Axes",new biov_axis_visible_command(this));
  view.add("Transfer function", btf);

  vgui_menu bar;
  bar.add("File",file);
  bar.add("View",view);

  return bar;
}

//: Called by biov_transfer_function if a change is made
void
biov_examiner_tableau::transfer_callback(const bool & remap, const int & n_min,
                                              const int & n_max, const int & n_map)
{
    if (remap && n_min<=n_max && n_min>=0 && n_max <=255) {
        this->min = n_min;
        this->max = n_max;

        if(this->min > this->max)
        {
          int tmp = this->min;
          this->min = this->max;
          this->max = tmp;
        }

        if (n_map>SoTransferFunction::NONE) {
            this->map = n_map;
            transfunc->predefColorMap = this->map;
            loadColorMap();
        }
        transfunc->reMap(this->min,this->max);
    }
}

//: Converts the 2D image plane point ([0,1],[0,1]) to 3D world coordinates (handy for SoCamera::pointAt())
SbVec3f
biov_examiner_tableau::screenToWorld(const SbVec2f & screenpos)
{
 if (scene_camera_ == NULL) return SbVec3f();
 float aspect_ratio = this->camera()->aspectRatio.getValue();
 SbViewVolume vv = this->camera()->getViewVolume(aspect_ratio);
 SbPlane image_plane = vv.getPlane(this->camera()->focalDistance.getValue());
 SbLine line;
 vv.projectPointToLine(screenpos, line);
 SbVec3f worldpos;
 image_plane.intersect(line, worldpos);
 return worldpos;
}

//: Pan the camera
void biov_examiner_tableau::pan(const SbVec2f& currpos, const SbVec2f &prevpos, const float aspect_ratio)
{
  if (scene_camera_ == NULL) return; // can happen for empty scenegraph
   
  if (currpos == prevpos) return; // useless invocation
  
  SbViewVolume vv = scene_camera_->getViewVolume(aspect_ratio);
  SbPlane panningplane = vv.getPlane(scene_camera_->focalDistance.getValue());
     
  vv = scene_camera_->getViewVolume(aspect_ratio);
  SbLine line;
  vv.projectPointToLine(currpos, line);
  SbVec3f current_planept;
  panningplane.intersect(line, current_planept);
  vv.projectPointToLine(prevpos, line);
  SbVec3f old_planept;
  panningplane.intersect(line, old_planept);


  scene_camera_->position = scene_camera_->position.getValue() - (current_planept - old_planept);   
  
}

//: Zoom (actually dolly) the camera
void biov_examiner_tableau::zoom(float diffvalue)
{
 if (scene_camera_ == NULL) return; // can happen for empty scenegraph
  
  SbVec3f direction;
  scene_camera_->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
  scene_camera_->position = diffvalue*direction + scene_camera_->position.getValue();
  scene_camera_->focalDistance = -diffvalue*direction.length() + scene_camera_->focalDistance.getValue();
}

//: Spin the scene based on the current mouse position and the last mouse position
void biov_examiner_tableau::spin(const SbVec2f& currpos, const SbVec2f &prevpos)
{
  spin_projector_->project(prevpos);
  SbRotation r;
  spin_projector_->projectAndGetRotation(currpos, r);
  r.invert();
  reorient_camera(r);
  
}

//: Reorient the camera based on specified rotation
void biov_examiner_tableau::reorient_camera(const SbRotation & rot)
{
  SoCamera * cam = scene_camera_;
  if (cam == NULL) return;

  // Find global coordinates of focal point.
  SbVec3f direction;
  cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
  SbVec3f focalpoint = cam->position.getValue() +
          cam->focalDistance.getValue() * direction;

  // Set new orientation value by accumulating the new rotation.
  cam->orientation = rot * cam->orientation.getValue();
  SbVec3f lookat(0, 0, -1); // init to default view direction vector
  
  cam->orientation.getValue().multVec(lookat, lookat);
  
  
  // Reposition camera so we are still pointing at the same old focal point.
  cam->orientation.getValue().multVec(SbVec3f(0, 0, -1), direction);
  SbVec3f distance = cam->focalDistance.getValue() * direction;
  
  cam->position = focalpoint - distance;
    
}

//: Update the log so that it can keep track of where the mouse has been
void biov_examiner_tableau::update_log(SbVec2f pos)
{
  log_.size++;
  log_.pos3 = log_.pos2;
  log_.pos2 = log_.pos1;
  log_.pos1 = pos;
}

//: Reset the log 
void biov_examiner_tableau::reset_log()
{
  log_.size = 0;
}


void
seeksensorCB(void * data, SoSensor * s)
{
  SbTime currenttime = SbTime::getTimeOfDay();

  biov_examiner_tableau * thisp = (biov_examiner_tableau *)data;
  SoTimerSensor * sensor = (SoTimerSensor *)s;

  float t = float((currenttime - sensor->getBaseTime()).getValue()) / 1.0f;
  if ((t > 1.0f) || (t + sensor->getInterval().getValue() > 1.0f)) t = 1.0f;
  SbBool end = (t == 1.0f);
  
  t = (float) ((1.0 - cos(M_PI*t)) * 0.5);
  
  thisp->camera()->position = thisp->_fromPos + (thisp->_toPos - thisp->_fromPos) * t;
  thisp->camera()->orientation = SbRotation::slerp( thisp->_fromRot, thisp->_toRot, t);
  if( end )
    s->unschedule();

}

// Seek to a specified point on the screen
void biov_examiner_tableau::seek_to_point( SbVec2s pos )
{
  if (! scene_camera_)
    return;
  
  // SoRayPickAction needs the the viewport to have origin (0,0)
  SbViewportRegion v = get_viewport_region();
  v.setViewportPixels( SbVec2s(0,0), v.getViewportSizePixels() ); 

  SoRayPickAction rpaction( v );
  rpaction.setPoint( pos );
  rpaction.setRadius( 1 );
  rpaction.apply( user_scene_root_ );

  SoPickedPoint * picked = rpaction.getPickedPoint();
  if (!picked) 
    return;
  float factor;
  switch( seek_distance_ )
  {
   case SEEK_FAR:
     factor = 0.75f;
     break;
   case SEEK_HALF:
     factor = 0.5f;
     break;
   case SEEK_NEAR:
   default:
     factor = 0.25f;
     break;
   case SEEK_ZERO:
     factor = 0.0f;
     break;
  }
  
  
  SbVec3f hitpoint = picked->getPoint();
  SbVec3f cameraposition = scene_camera_->position.getValue();
  SbVec3f diff = hitpoint - cameraposition;
  _fromPos = cameraposition;
  _toPos = cameraposition += factor*diff;
  
  SbRotation camrot = scene_camera_->orientation.getValue();
  SbVec3f lookat(0, 0, -1); // init to default view direction vector
  camrot.multVec(lookat, lookat);
  SbRotation rot(lookat, diff);
  
   _fromRot = camrot;
  _toRot = camrot*rot ;
 
  scene_camera_->focalDistance = diff.length()*(1.0-factor);
  _seekSensor->setBaseTime( SbTime::getTimeOfDay() );
  _seekSensor->schedule();
  
}


//: Set the visibility of the axis cross
void 
biov_examiner_tableau::set_axis_visible(bool state)
{
  axis_visible_ = state;
}


//: Return true if the axis cross is visible
bool 
biov_examiner_tableau::axis_visible() const
{
  return axis_visible_;
}

//: Changes the distance the viewer goes when seeking
void 
biov_examiner_tableau::set_seek_distance( SeekDistance seek )
{
  seek_distance_ = seek;
}


//: Render the scene graph (called on draw events)
bool
biov_examiner_tableau::render()
{
   positionCMap();
  
  // call the super class method
  bool result = bgui3d_fullviewer_tableau::render();
  if(!result)
    return false;

  if(axis_visible_)
    this->draw_axis_cross();


  return true;
}

//: Populate the colormap array and set the image
void biov_examiner_tableau::loadColorMap()
{
    float cmap_offset = 255.0f/cmap_height_;
    unsigned char * img = new unsigned char[cmap_width_*cmap_height_*4];
    const float * cmap_values = transfunc->colorMap.getValues(0);
    int k=0;
    CvrGIMPGradient * gg = CvrGIMPGradient::read(getCMapString().c_str());
    unsigned char intgradient[256][4];
    gg->convertToIntArray(intgradient);
    float curr_offset = 0;
    for (int j=0; j < cmap_height_; j++) {
        k = (int)curr_offset;
//        cmap_values = transfunc->colorMap.getValues(k);
        for (int i=0; i<cmap_width_*4; i+=4) {
            img[j*cmap_width_*4+i] = intgradient[256-k][0];
            img[j*cmap_width_*4+i+1] = intgradient[256-k][1];
            img[j*cmap_width_*4+i+2] = intgradient[256-k][2];
            img[j*cmap_width_*4+i+3] = intgradient[256-k][3];
        }
        curr_offset += cmap_offset;
    }
    cmap_image_->image.setValue(SbVec2s(cmap_width_,cmap_height_), 4, img);
}

//: Given the current map, retrieve the string that represents it.
vcl_string
biov_examiner_tableau::getCMapString()
{
    if (map == SoTransferFunction::NONE)
        return PHYSICS_gradient;
    else if (map == SoTransferFunction::GREY)
        return GREY_gradient;
    else if (map == SoTransferFunction::TEMPERATURE)
        return TEMPERATURE_gradient;
    else if (map == SoTransferFunction::PHYSICS)
        return PHYSICS_gradient;
    else if (map == SoTransferFunction::STANDARD)
        return STANDARD_gradient;
    else if (map == SoTransferFunction::GLOW)
        return GLOW_gradient;
    else if (map == SoTransferFunction::BLUE_RED)
        return BLUE_RED_gradient;
    else if (map == SoTransferFunction::SEISMIC)
        return SEISMIC_gradient;
    else
        return PHYSICS_gradient;
}

//: Position the color map bar in the top left corner of screen
//  and scale it screen height.
void
biov_examiner_tableau::positionCMap()
{
#if 1 

   //set up the view volume of the color map camera
   SbVec2s viewport = get_viewport_region().getViewportSizePixels();

   if(viewport != last_viewport_sz_){
     float aspect = float(viewport[0]) / float(viewport[1]);
     float factor = float(100) / float(viewport[1]) * 3.0f;
     float wsx =1, wsy = 1;
     if ( aspect > 1.0f ) 
       wsx *= aspect;
     else {
       wsy /= aspect;
       factor /= aspect;
     }
     
     cmap_transform->translation.setValue(SbVec3f(-0.5f * wsx, 0.5f*wsy, .0f));

     float wcmap_width = wsx * cmap_width_ / float( viewport[0]);
     float wcmap_height = wsy * cmap_height_ / float(viewport[1]);
     
     float min_height = wsy * cmap_height_ / viewport[1] * this->min / 255;
     min_transform->translation.setValue(SbVec3f(wcmap_width, -min_height, 0.f));
     min_transform->scaleFactor.setValue( SbVec3f(factor, factor, 1.0f));
  
     float max_height = wsy * cmap_height_ / float( viewport[1]) * this->max/255.0f;
     max_transform->translation.setValue(SbVec3f(wcmap_width, -max_height, 0.f));
     max_transform->scaleFactor.setValue( SbVec3f(factor, factor, 1.0f));
    }
#endif

}

void
biov_examiner_tableau::draw_axis_cross()
{
  // FIXME: convert this to a superimposition scenegraph instead of
  // OpenGL calls. 20020603 mortene.

  // Store GL state.
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  GLfloat depthrange[2];
  glGetFloatv(GL_DEPTH_RANGE, depthrange);
  GLdouble projectionmatrix[16];
  glGetDoublev(GL_PROJECTION_MATRIX, projectionmatrix);

  glDepthFunc(GL_ALWAYS);
  glDepthMask(GL_TRUE);
  glDepthRange(0, 0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_BLEND); // Kills transparency.

  // Set the viewport in the OpenGL canvas. Dimensions are calculated
  // as a percentage of the total canvas size.
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  const int view_x = vp[2];
  const int view_y = vp[3];
  const int pixelarea =
    int(float(25)/100.0f * vcl_min(view_x, view_y));
  // lower right of canvas
  glViewport(vp[0]+vp[2] - pixelarea, vp[1], pixelarea, pixelarea);



  // Set up the projection matrix.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  const float NEARVAL = 0.1f;
  const float FARVAL = 10.0f;
  const float dim = NEARVAL * float(tan(M_PI / 8.0)); // FOV is 45° (45/360 = 1/8)
  glFrustum(-dim, dim, -dim, dim, NEARVAL, FARVAL);


  // Set up the model matrix.
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  SbMatrix mx;
  SoCamera * cam = this->camera();

  // If there is no camera (like for an empty scene, for instance),
  // just use an identity rotation.
  if (cam) { mx = cam->orientation.getValue(); }
  else { mx = SbMatrix::identity(); }

  mx = mx.inverse();
  mx[3][2] = -3.5; // Translate away from the projection point (along z axis).
  glLoadMatrixf((float *)mx);


  // Find unit vector end points.
  SbMatrix px;
  glGetFloatv(GL_PROJECTION_MATRIX, (float *)px);
  SbMatrix comb = mx.multRight(px);

  SbVec3f xpos;
  comb.multVecMatrix(SbVec3f(1,0,0), xpos);
  xpos[0] = (1 + xpos[0]) * view_x/2;
  xpos[1] = (1 + xpos[1]) * view_y/2;
  SbVec3f ypos;
  comb.multVecMatrix(SbVec3f(0,1,0), ypos);
  ypos[0] = (1 + ypos[0]) * view_x/2;
  ypos[1] = (1 + ypos[1]) * view_y/2;
  SbVec3f zpos;
  comb.multVecMatrix(SbVec3f(0,0,1), zpos);
  zpos[0] = (1 + zpos[0]) * view_x/2;
  zpos[1] = (1 + zpos[1]) * view_y/2;


  // Render the cross.
  {
    glLineWidth(2.0);

    enum { XAXIS, YAXIS, ZAXIS };
    int idx[3] = { XAXIS, YAXIS, ZAXIS };
    float val[3] = { xpos[2], ypos[2], zpos[2] };

    // Bubble sort.. :-}
    if (val[0] < val[1]) { vcl_swap(val[0], val[1]); vcl_swap(idx[0], idx[1]); }
    if (val[1] < val[2]) { vcl_swap(val[1], val[2]); vcl_swap(idx[1], idx[2]); }
    if (val[0] < val[1]) { vcl_swap(val[0], val[1]); vcl_swap(idx[0], idx[1]); }
    assert((val[0] >= val[1]) && (val[1] >= val[2])); // Just checking..

    for (int i=0; i < 3; i++) {
      glPushMatrix();
      if (idx[i] == XAXIS) {                       // X axis.
        glColor3f(0.500f, 0.125f, 0.125f);
      } else if (idx[i] == YAXIS) {                // Y axis.
        glRotatef(90, 0, 0, 1);
        glColor3f(0.125f, 0.500f, 0.125f);
      } else {                                     // Z axis.
        glRotatef(-90, 0, 1, 0);
        glColor3f(0.125f, 0.125f, 0.500f);
      }
      this->draw_arrow();
      glPopMatrix();
    }
  }

  // Render axis notation letters ("X", "Y", "Z").
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, view_x, 0, view_y, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLint unpack;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glColor3fv(SbVec3f(0.8f, 0.8f, 0.0f).getValue());

  glRasterPos2d(xpos[0], xpos[1]);
  glBitmap(8, 7, 0, 0, 0, 0, xbmp);
  glRasterPos2d(ypos[0], ypos[1]);
  glBitmap(8, 7, 0, 0, 0, 0, ybmp);
  glRasterPos2d(zpos[0], zpos[1]);
  glBitmap(8, 7, 0, 0, 0, 0, zbmp);

  glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);
  glPopMatrix();

  // Reset original state.

  // FIXME: are these 3 lines really necessary, as we push
  // GL_ALL_ATTRIB_BITS at the start? 20000604 mortene.
  glDepthRange(depthrange[0], depthrange[1]);
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(projectionmatrix);

  glPopAttrib();
}

//: Draw an arrow for the axis representation directly through OpenGL.
void
biov_examiner_tableau::draw_arrow()
{
  glBegin(GL_LINES);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glEnd();
  glDisable(GL_CULL_FACE);
  glBegin(GL_TRIANGLES);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, +0.5f / 4.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, -0.5f / 4.0f, 0.0f);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, 0.0f, +0.5f / 4.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, 0.0f, -0.5f / 4.0f);
  glEnd();
  glBegin(GL_QUADS);
  glVertex3f(1.0f - 1.0f / 3.0f, +0.5f / 4.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, 0.0f, +0.5f / 4.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, -0.5f / 4.0f, 0.0f);
  glVertex3f(1.0f - 1.0f / 3.0f, 0.0f, -0.5f / 4.0f);
  glEnd();
}
